/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "i2c.h"
#include "nfc.h"
#include "st25dv.h"
#include <string.h>  /* memcpy */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#ifdef DEBUG
#undef NFC_WaitACK
#endif
/* Private function prototypes -----------------------------------------------*/
static int32_t NFC_IsReady(uint16_t DevAddr, const uint32_t Trials);
static int32_t nop(void);

/* Global variables ----------------------------------------------------------*/
ST25DV_Object_t St25Dv_Obj;

/* Public functions ----------------------------------------------------------*/
/* NAME
 *        NFC_Init - project-level abstraction to initialize st25dv.
 *
 * DESCRIPTION
 *        Setup St25Dv_Obj global, and ST25DV device registers.
 *
 *        System register configuration requires to disable mailbox,
 *        so keep mailbox last.
 *
 * BUGS
 *        Doesn't init host I2C1/GPIO and assumes prior setup.
 *        Hencemore, assumes GPO Open Drain, rather than GPO CMOS.
 *
 * SEE ALSO
 *        st25dx_discovery_nfctag.c:BSP_NFCTAG_Init of STSW-ST25DV001 firmware:
 *        https://www.st.com/en/embedded-software/stsw-st25dv001.html
 *
 *        Datasheet for ST25DV04K-IER6C3 UFDFPN8:
 *        https://www.st.com/resource/en/datasheet/st25dv04k.pdf
 */
int32_t NFC_Init(bool keepmailbox) {
  uint16_t v;
  int32_t r, c;
  ST25DV_IO_t io;
  ST25DV_I2CSSO_STATUS i2csso = ST25DV_SESSION_CLOSED;

  /* Scenarios:
   * a. Nominal power-up boot:
   *      ACK within 1 ms
   * b. Mainfw restart to bootldr due fwupd msg:
   *      ACK within 1 ms
   * c. Mainfw restart to bootldr due fwupd msg with 100 ms delay added:
   *      ACK after 1156 ms
   *
   * Thus, to account for the arcane arts of mainfw+st25dv, we wait.
   */
  NFC_WaitACK(2000);

  /* Initialize ST25DV_Object_t */
  io.Init = nop;
  io.DeInit = NULL;
  io.IsReady = NFC_IsReady;
  io.Write = NFC_WriteReg;
  io.Read = NFC_ReadReg;
  io.GetTick = (int32_t (*)(void))HAL_GetTick;
  if((r = ST25DV_RegisterBusIO(&St25Dv_Obj, &io))) {c = 0x1; goto err;};
  if((r = St25Dv_Drv.Init(&St25Dv_Obj))) {c = 0x2; goto err;};

  /* Open I2CSS Security Session on ST25DV */
  if((r = ST25DV_ReadI2CSecuritySession_Dyn(&St25Dv_Obj, &i2csso))) {c = 0x3; goto err;};
  if(i2csso == ST25DV_SESSION_CLOSED)
    if((r = ST25DV_PresentI2CPassword(&St25Dv_Obj, (ST25DV_PASSWD){0}))) {c = 0x4; goto err;};
  if((r = ST25DV_ReadI2CSecuritySession_Dyn(&St25Dv_Obj, &i2csso))) {c = 0x5; goto err;};
  if(i2csso == ST25DV_SESSION_CLOSED) {c = 0x6; goto err;};

  /* Enable GPO interrupt status bits */
  GPO_Activated = 0;
  if((r = St25Dv_Drv.GetITStatus(&St25Dv_Obj, &v))) {c = 0x7; goto err;};
  if(v != (ST25DV_GPO_ALL_MASK))
    if((r = St25Dv_Drv.ConfigIT(&St25Dv_Obj, ST25DV_GPO_ALL_MASK))) {c = 0x8; goto err;};

  /* Allow RF comms */
  if((r = ST25DV_WriteRFMngt_Dyn(&St25Dv_Obj, 0))) {c = 0x9; goto err;};

  /* Disable Mailbox Watchdog */
  if((r = ST25DV_ReadMBWDG(&St25Dv_Obj, (uint8_t*)&v))) {c = 0xa; goto err;};
  if((uint8_t)v)
    if((r = ST25DV_WriteMBWDG(&St25Dv_Obj, 0))) {c = 0xb; goto err;};

  /* When FTM is active, system memory should rather be left alone, in datasheet theory.
   * In practice, we've been setting it for long time with no repercussions.
   */
  if(keepmailbox) return 0;

  /*
   * Enable Mailbox
   */
  if((r = ST25DV_ResetMBEN_Dyn(&St25Dv_Obj))) {c = 0xc; goto err;};
  if((r = ST25DV_SetMBEN_Dyn(&St25Dv_Obj))) {c = 0xd; goto err;};

  return 0;
err:
  DBG_PRINTF("NFC ERR ST25DV ret:0x%x cond:0x%x err:0x%x Init Failed!\n", r, c, hi2c1.ErrorCode);
  return r;
}

void NFC_DeInit(void) {
  /* Disable mailbox and GPO Interrupts */
  ST25DV_ResetMBEN_Dyn(&St25Dv_Obj);
  St25Dv_Drv.ConfigIT(&St25Dv_Obj, 0);
  ST25DV_ReadITSTStatus_Dyn(&St25Dv_Obj, 0);
  GPO_Activated = 0;

  /* Close I2CSS Security Session on ST25DV */
  ST25DV_PresentI2CPassword(&St25Dv_Obj, (ST25DV_PASSWD){0x12345678, 0x13245678});
}

static int32_t nop(void) {
  return 0;
}

static int32_t NFC_IsReady(uint16_t DevAddr, const uint32_t Trials) {
  return HAL_I2C_IsDeviceReady(&hi2c1, DevAddr, Trials, BUS_I2C1_POLL_TIMEOUT);
}

/* NAME
 *        NFC_ReadReg - top-level i2c comms with st25dv04k, with RTT logs and retries.
 *
 * DESCRIPTION
 *        Assuming I2CCLK is 2^21 Hz, then Fscl is maybe ~236 kHz. Thus reading
 *        260 bytes takes ~10 ms. 512 bytes takes ~20 ms.
 *
 *        NACKs tend to return immediately for HAL, and its timeouts rather
 *        apply to transfer completion and stop conditions.
 *
 *    Retries due to NACKs
 *        ST25DV may NACK due to internal EEPROM programming (wait 5.ms), or
 *        due to being busy on RF (may only serve I2C or RF at a time).
 *
 *        Retries with <3.ms delays perpetuate NACKs
 *        Retries with >100.ms delays occasionally NACK
 *        According to Trial & Error.
 *
 *    SCL Clock
 *        Ti2cclk = 1 / 2^21
 *                = 476.ns (2'097'152.Hz)
 *        Ti2cclk_sync = (1 <= ((PRESC+1) * (SCLL+1) + 2.ticks) / 4 ? 2.ticks : 3.ticks) * Ti2cclk
 *                     = 1'430.ns (699'050.Hz)
 *        Tfilters = Tf   + Tr   + (Tdnf + Tanf   + Ti2cclk_sync) * 2.levels
 *                 = 0.ns + 0.ns + (0.ns + 250.ns + 1430.ns     ) * 2.levels
 *                 = 3'361.ns (297'528.Hz)
 *        Tscll = Ti2cclk * (PRESC+1) * (SCLL+1) = 476.ns
 *        Tsclh = Ti2cclk * (PRESC+1) * (SCLH+1) = 476.ns
 *        Tscl = Tfilters + Tscll + Tsclh
 *             = 4'314.ns (231'765.Hz)
 *
 *    Largest read transfer
 *        (SCL period  ) * ( Regs  + DevAddr + RegAddr + DevAddr) * Byte&Ack + STARTs/STOP)
 *        (1/230'000.Hz) * ((260.B + 4.B                        ) * 9.b      + 4.b        )
 *
 * NOTES
 *    Interrupt Context
 *        ST25DV requires a delay (theory unknown), and for lack of millisecond scheduler,
 *        we block using HAL_Delay. Also HAL_I2C_Mem_Read is used instead of HAL_I2C_Mem_Read_IT.
 *        Thus invoke within EXTI0_1_IRQ, whose priority is below all other interrupts.
 *        In practice, it means main is blocked for a couple milliseconds.
 *
 *    Indefinite NACKs
 *        I've seen scenarios, where sequential retries with 4 ms delay fail indefinitely
 *        (as far as 7 seconds). Yet if in same scenario one polls with NFC_WaitACK(2000),
 *        the former request succeeds as much as on first try.
 */
int32_t NFC_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length) {
  uint32_t try = 3, r = 1, ts = HAL_GetTick();

  /* First try, Hooray! */
  if(!HAL_I2C_Mem_Read(&hi2c1, DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length, 50))
    return 0;

  /* Ok, well, maybe st25dv was preoccupied by serving phone, or nasal demons */
  NFC_WaitACK(5);

  /* If this NACKs, either it's
   * 1. A rare coincidence,
   * 2. The request is invalid (under context),
   * 3. This is that time where device goes for a hike for 1156 ms */
  while(try-- && (r = HAL_I2C_Mem_Read(&hi2c1, DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length, 50)))
    DBG_PRINTF("NFC I2C <RX TRY dur:%3d try:%2d ret:0x%x err:0x%x dev:0x%02x reg:0x%04x len:%x\n", HAL_GetTick() - ts, try, r, hi2c1.ErrorCode, DevAddr, Reg, Length), NFC_WaitACK(50);
  if(r)
    DBG_PRINTF("NFC I2C <RX ERR dur:%3d ret:0x%x err:0x%x dev:0x%02x reg:0x%04x len:0x%x caller:%p\n", HAL_GetTick() - ts, r, hi2c1.ErrorCode, DevAddr, Reg, Length, __builtin_return_address(0));
  return r;
}

/* NAME
 *        NFC_WriteReg - top-level i2c comms with st25dv04k, with RTT logs and retries.
 *
 * SEE ALSO
 *        NFC_ReadReg
 */
int32_t NFC_WriteReg(uint16_t DevAddr, uint16_t Reg, const uint8_t *pData, uint16_t Length) {
  uint32_t try = 3, r = 1, ts = HAL_GetTick();

  /* First try, Hooray! */
  if(!HAL_I2C_Mem_Write(&hi2c1, DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, (void*)pData, Length, 50))
    return 0;

  /* Ok, well, maybe st25dv was preoccupied by serving phone, or nasal demons */
  NFC_WaitACK(5);

  /* If this NACKs, either it's
   * 1. A rare coincidence,
   * 2. The request is invalid (under context),
   * 3. This is that time where device goes for a hike for 1156 ms */
  while(try-- && (r = HAL_I2C_Mem_Write(&hi2c1, DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, (void*)pData, Length, 50)))
    DBG_PRINTF("NFC I2C >TX TRY dur:%3d try:%2d ret:0x%x err:0x%x dev:0x%02x reg:0x%04x len:%x\n", HAL_GetTick() - ts, try, r, hi2c1.ErrorCode, DevAddr, Reg, Length), NFC_WaitACK(50);
  if(r)
    DBG_PRINTF("NFC I2C >TX ERR dur:%3d ret:0x%x err:0x%x dev:0x%02x reg:0x%04x len:0x%x caller:%p\n", HAL_GetTick() - ts, r, hi2c1.ErrorCode, DevAddr, Reg, Length, __builtin_return_address(0));
  return r;
}

/* NAME
 *        NFC_WaitACK - project-level abstraction for extended wait on st25dv,
 *        when duration is too long for NFC_ReadReg, NFC_WriteReg.
 *
 * DESCRIPTION
 *        +--------------------------------------------+---+---+---+
 *        | ST25DV Inter-Integrated Circuit            | _ | = | X |
 *        +--------------------------------------------+---+---+---+
 *        | The application is not responding. The program may     |
 *        | respond again if you wait.                             |
 *        |                                                        |
 *        |                                            +--------+  |
 *        |                                            | Cancel |  |
 *        |                                            +--------+  |
 *        +--------------------------------------------------------+
 *
 *        Reflects HAL_I2C_IsDeviceReady, but timeout-based, rather than
 *        trial-based. And reduced for ST25DV use.
 *
 * BUGS
 *        We, just like HAL, don't wait nor clear STOP flag on timeout.
 *
 *        Do you think compiler would reorder the hi2c->Lock assigns?
 *        There's sequence points, but no explicit dependency across them.
 */
int32_t NFC_WaitACK(uint32_t Timeout) {
  I2C_HandleTypeDef * const hi2c = &hi2c1;
  uint32_t tickstart = HAL_GetTick();

  /* Grab I2C Handle */
  if(hi2c->State != HAL_I2C_STATE_READY)
    return HAL_BUSY;
  if(hi2c->Instance->ISR & I2C_ISR_BUSY)
    return HAL_BUSY;
  if(hi2c->Lock == HAL_LOCKED)
    return HAL_BUSY;
  else
    hi2c->Lock = HAL_LOCKED;
  hi2c->State = HAL_I2C_STATE_BUSY;
  hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

  do {
    /* Generate Start */
    hi2c->Instance->CR2 = I2C_CR2_START | I2C_CR2_AUTOEND | (I2C_CR2_SADD & ST25DV_ADDR_DATA_I2C);

    /* Wait till AUTOEND mode generates STOP, or timeout */
    while(~hi2c->Instance->ISR & I2C_ISR_STOPF)
      if(Timeout != HAL_MAX_DELAY && (HAL_GetTick() - tickstart > Timeout || !Timeout))
        goto timeout;

    /* We've been ACK'ed, cleanup and go home */
    if(~hi2c->Instance->ISR & I2C_ISR_NACKF) {
      hi2c->Instance->ICR |= I2C_ISR_STOPF;
      /* Release I2C_Handle */
      hi2c->State = HAL_I2C_STATE_READY;
      hi2c->Lock = HAL_UNLOCKED;
      if(HAL_GetTick() - tickstart > 100)
        DBG_PRINTF("NFC I2C WAIT INFO dur:%5d timeout:%5d caller:0x%p\n", HAL_GetTick() - tickstart, Timeout, __builtin_return_address(0));
      return HAL_OK;
    }

    /* We've been NACK'ed, clear interrupt status, continue loop */
    hi2c->Instance->ICR |= I2C_ISR_NACKF | I2C_ISR_STOPF;
  } while(Timeout == HAL_MAX_DELAY || (HAL_GetTick() - tickstart < Timeout && Timeout));

timeout:
  /* Release I2C Handle */
  hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
  hi2c->State = HAL_I2C_STATE_READY;
  hi2c->Mode = HAL_I2C_MODE_NONE;
  hi2c->Lock = HAL_UNLOCKED;
  return HAL_ERROR;
}

/* NAME
 *        MBDecodeHeader - extract info of FTM protocol header
 *
 * DESCRIPTION
 *        Needed to provide proper endianness and alignment to integer values.
 *
 * CONFORMING TO
 *        mailboxfunc.c:MBDecodeHeader of STSW-ST25DV001 firmware:
 *        https://www.st.com/en/embedded-software/stsw-st25dv001.html
 */
void MBDecodeHeader(const uint8_t * const pData, MB_HEADER_T * const mb_header) {
  mb_header->fctcode = pData[MB_FCTCODE];
  mb_header->cmdresp = pData[MB_CMDRESP];
  mb_header->error = pData[MB_ERROR];
  mb_header->chaining = pData[MB_CHAINING];
  if (mb_header->chaining == MB_NOTCHAINED) {
    /* If simple message header is 5-byte long */
    mb_header->framelength = pData[MB_LENGTH];
    mb_header->framesize = MB_DATA + mb_header->framelength;
  } else {
    /* If simple message header is 13-byte long */
    mb_header->fulllength = pData[MB_CH_FULLLENGTH - 3] & 0xFF;
    mb_header->fulllength = (mb_header->fulllength << 8)
        | pData[MB_CH_FULLLENGTH - 2];
    mb_header->fulllength = (mb_header->fulllength << 8)
        | pData[MB_CH_FULLLENGTH - 1];
    mb_header->fulllength = (mb_header->fulllength << 8)
        | pData[MB_CH_FULLLENGTH];
    mb_header->totalchunk = pData[MB_CH_TOTALCHUNK - 1] & 0xFF;
    mb_header->totalchunk = (mb_header->totalchunk << 8)
        | pData[MB_CH_TOTALCHUNK];
    mb_header->chunknb = pData[MB_CH_NBCHUNK - 1] & 0xFF;
    mb_header->chunknb = (mb_header->chunknb << 8) | pData[MB_CH_NBCHUNK];
    mb_header->framelength = pData[MB_CH_LENGTH];
    mb_header->framesize = MB_CH_LENGTH + mb_header->framelength;
  }
}

void NFC_ClearMailbox(void) {
  NFC_WriteReg(ST25DV_ADDR_DATA_I2C, ST25DV_MB_CTRL_DYN_REG, &(uint8_t){0}, 1);
  NFC_WriteReg(ST25DV_ADDR_DATA_I2C, ST25DV_MB_CTRL_DYN_REG, &(uint8_t){1}, 1);
}

/* NAME
 *        NFC_ReadRegAll - devtool to check all contents of st25dv
 *
 * BUGS
 *        IT_STS register clears as side-effect of reading it.
 *        Assumes device is ST25DV04K and NFCTAG size is 512 bytes.
 *        Assumes I2C security session is open, to read password register.
 *
 * SEE ALSO
 *        Table 11, 12 and 13:
 *        https://www.st.com/resource/en/datasheet/st25dv04k.pdf#page=17
 */
uint32_t NFC_ReadRegAll(struct ST25DV_AllRegs * const save) {
  int r;
  if((r = NFC_ReadReg(ST25DV_ADDR_SYST_I2C, ST25DV_GPO_REG, save->buf_sysregs, sizeof save->buf_sysregs))) return r | 0x80000000;
  if((r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, ST25DV_GPO_DYN_REG, save->buf_dynregs, sizeof save->buf_dynregs))) return r | 0x40000000;
  if((r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, ST25DV_I2CPASSWD_REG, (void*)&save->SS0900_i2c_pwd, sizeof save->SS0900_i2c_pwd))) return r | 0x20000000;

  /* Read mailbox (volatile) */
  memcpy(save->buf_mailbox, save->buf_dynregs + 0x8, sizeof save->buf_mailbox);

  /* Read (system configuration) static registers (non-volatile) */
  save->rS0000_gpo.GPO_RFUser_en       = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFUSERSTATE_MASK);
  save->rS0000_gpo.GPO_RFActivity_en   = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFACTIVITY_MASK);
  save->rS0000_gpo.GPO_RFInterrupt_en  = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFINTERRUPT_MASK);
  save->rS0000_gpo.GPO_FieldChange_en  = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_FIELDCHANGE_MASK);
  save->rS0000_gpo.GPO_RFPutMsg_en     = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFPUTMSG_MASK);
  save->rS0000_gpo.GPO_RFGetMsg_en     = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFGETMSG_MASK);
  save->rS0000_gpo.GPO_RFWrite_en      = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_RFWRITE_MASK);
  save->rS0000_gpo.GPO_Enable          = !!(save->buf_sysregs[ST25DV_GPO_REG] & ST25DV_GPO_ENABLE_MASK);

  save->rS0001_it_time              = save->buf_sysregs[ST25DV_ITTIME_REG]                                      & 0x7;
  save->rS0002_eh_mode              = !!(save->buf_sysregs[ST25DV_EH_MODE_REG]                                  & ST25DV_EH_MODE_MASK);
  save->rS0003_rf_mngt.RfDisable    = !!(save->buf_sysregs[ST25DV_RF_MNGT_REG] >> ST25DV_RF_MNGT_RFDIS_SHIFT    & ST25DV_RF_MNGT_RFDIS_MASK);
  save->rS0003_rf_mngt.RfSleep      = !!(save->buf_sysregs[ST25DV_RF_MNGT_REG] >> ST25DV_RF_MNGT_RFSLEEP_SHIFT  & ST25DV_RF_MNGT_RFSLEEP_MASK);

  save->rS0004_rfa1ss.PasswdCtrl    = (save->buf_sysregs[ST25DV_RFA1SS_REG] & ST25DV_RFA1SS_PWDCTRL_MASK)  >> ST25DV_RFA1SS_PWDCTRL_SHIFT;
  save->rS0004_rfa1ss.RWprotection  = (save->buf_sysregs[ST25DV_RFA1SS_REG] & ST25DV_RFA1SS_RWPROT_MASK )  >> ST25DV_RFA1SS_RWPROT_SHIFT;
  save->rS0006_rfa2ss.PasswdCtrl    = (save->buf_sysregs[ST25DV_RFA2SS_REG] & ST25DV_RFA2SS_PWDCTRL_MASK)  >> ST25DV_RFA2SS_PWDCTRL_SHIFT;
  save->rS0006_rfa2ss.RWprotection  = (save->buf_sysregs[ST25DV_RFA2SS_REG] & ST25DV_RFA2SS_RWPROT_MASK )  >> ST25DV_RFA2SS_RWPROT_SHIFT;
  save->rS0008_rfa3ss.PasswdCtrl    = (save->buf_sysregs[ST25DV_RFA3SS_REG] & ST25DV_RFA3SS_PWDCTRL_MASK)  >> ST25DV_RFA3SS_PWDCTRL_SHIFT;
  save->rS0008_rfa3ss.RWprotection  = (save->buf_sysregs[ST25DV_RFA3SS_REG] & ST25DV_RFA3SS_RWPROT_MASK )  >> ST25DV_RFA3SS_RWPROT_SHIFT;
  save->rS000a_rfa4ss.PasswdCtrl    = (save->buf_sysregs[ST25DV_RFA4SS_REG] & ST25DV_RFA4SS_PWDCTRL_MASK)  >> ST25DV_RFA4SS_PWDCTRL_SHIFT;
  save->rS000a_rfa4ss.RWprotection  = (save->buf_sysregs[ST25DV_RFA4SS_REG] & ST25DV_RFA4SS_RWPROT_MASK )  >> ST25DV_RFA4SS_RWPROT_SHIFT;

  save->rS0005_enda1                = save->buf_sysregs[ST25DV_ENDA1_REG];
  save->rS0007_enda2                = save->buf_sysregs[ST25DV_ENDA2_REG];
  save->rS0009_enda3                = save->buf_sysregs[ST25DV_ENDA3_REG];

  save->rS000b_i2css.ProtectZone1   = (save->buf_sysregs[ST25DV_I2CSS_REG] & ST25DV_I2CSS_PZ1_MASK) >> ST25DV_I2CSS_PZ1_SHIFT;
  save->rS000b_i2css.ProtectZone2   = (save->buf_sysregs[ST25DV_I2CSS_REG] & ST25DV_I2CSS_PZ2_MASK) >> ST25DV_I2CSS_PZ2_SHIFT;
  save->rS000b_i2css.ProtectZone3   = (save->buf_sysregs[ST25DV_I2CSS_REG] & ST25DV_I2CSS_PZ3_MASK) >> ST25DV_I2CSS_PZ3_SHIFT;
  save->rS000b_i2css.ProtectZone4   = (save->buf_sysregs[ST25DV_I2CSS_REG] & ST25DV_I2CSS_PZ4_MASK) >> ST25DV_I2CSS_PZ4_SHIFT;

  save->rS000c_lock_ccfile.LckBck0  = !!(save->buf_sysregs[ST25DV_LOCKCCFILE_REG] & ST25DV_LOCKCCFILE_BLCK0_MASK);
  save->rS000c_lock_ccfile.LckBck1  = !!(save->buf_sysregs[ST25DV_LOCKCCFILE_REG] & ST25DV_LOCKCCFILE_BLCK1_MASK);

  save->rS000d_mb_mode              = !!(save->buf_sysregs[ST25DV_MB_MODE_REG] & ST25DV_MB_MODE_RW_MASK);
  save->rS000e_mb_wdg               = save->buf_sysregs[ST25DV_MB_WDG_REG] & ST25DV_MB_WDG_DELAY_MASK;

  save->rS000f_lock_cfg             = !!(save->buf_sysregs[ST25DV_LOCKCFG_REG] & ST25DV_LOCKCFG_B0_MASK);
  save->ro0010_lock_dsfid           = !!(save->buf_sysregs[ST25DV_LOCKDSFID_REG] & 1);
  save->ro0011_lock_afi             = !!(save->buf_sysregs[ST25DV_LOCKAFI_REG] & 1);
  save->ro0012_dsfid                = save->buf_sysregs[ST25DV_DSFID_REG];
  save->ro0013_afi                  = save->buf_sysregs[ST25DV_AFI_REG];

  save->ro0014_mem_size             = save->buf_sysregs[ST25DV_MEM_SIZE_MSB_REG] << 8 | save->buf_sysregs[ST25DV_MEM_SIZE_LSB_REG];
  save->ro0016_blk_size             = save->buf_sysregs[ST25DV_BLK_SIZE_REG];
  save->ro0017_ic_ref               = save->buf_sysregs[ST25DV_ICREF_REG];
  save->ro0020_ic_rev               = save->buf_sysregs[ST25DV_ICREV_REG];

  {
    uint8_t *p = save->buf_sysregs + ST25DV_UID_REG;
    typedef uint64_t t;
    save->ro0018_uid.uid =
        (t)p[7] << 56 | (t)p[6] << 48 | (t)p[5] << 40 | (t)p[4] << 32 |
        (t)p[3] << 24 | (t)p[2] << 16 | (t)p[1] << 8  | (t)p[0];
    memcpy(save->ro0018_uid.serial_nr, p, 5);
    save->ro0018_uid.product_nr = p[5];
    save->ro0018_uid.mfg = p[6];
    save->ro0018_uid.msb = p[7];
  }
  /* Read dynamic registers (volatile) */
  save->rw2000_gpo_dyn.GPO_RFUser_en       = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFUSERSTATE_MASK);
  save->rw2000_gpo_dyn.GPO_RFActivity_en   = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFACTIVITY_MASK);
  save->rw2000_gpo_dyn.GPO_RFInterrupt_en  = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFINTERRUPT_MASK);
  save->rw2000_gpo_dyn.GPO_FieldChange_en  = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_FIELDCHANGE_MASK);
  save->rw2000_gpo_dyn.GPO_RFPutMsg_en     = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFPUTMSG_MASK);
  save->rw2000_gpo_dyn.GPO_RFGetMsg_en     = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFGETMSG_MASK);
  save->rw2000_gpo_dyn.GPO_RFWrite_en      = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_RFWRITE_MASK);
  save->rw2000_gpo_dyn.GPO_Enable          = !!(save->buf_dynregs[ST25DV_GPO_DYN_REG - 0x2000] & ST25DV_GPO_DYN_ENABLE_MASK);

  save->rw2002_eh_ctrl_dyn.EH_EN_Mode      = !!(save->buf_dynregs[ST25DV_EH_CTRL_DYN_REG - 0x2000] & ST25DV_EH_CTRL_DYN_EH_EN_MASK);
  save->rw2002_eh_ctrl_dyn.EH_on           = !!(save->buf_dynregs[ST25DV_EH_CTRL_DYN_REG - 0x2000] & ST25DV_EH_CTRL_DYN_EH_ON_MASK);
  save->rw2002_eh_ctrl_dyn.Field_on        = !!(save->buf_dynregs[ST25DV_EH_CTRL_DYN_REG - 0x2000] & ST25DV_EH_CTRL_DYN_FIELD_ON_MASK);
  save->rw2002_eh_ctrl_dyn.VCC_on          = !!(save->buf_dynregs[ST25DV_EH_CTRL_DYN_REG - 0x2000] & ST25DV_EH_CTRL_DYN_VCC_ON_MASK);

  save->rw2003_rf_mngt_dyn.RfDisable       = !!(save->buf_dynregs[ST25DV_RF_MNGT_DYN_REG - 0x2000] & ST25DV_RF_MNGT_RFDIS_MASK);
  save->rw2003_rf_mngt_dyn.RfSleep         = !!(save->buf_dynregs[ST25DV_RF_MNGT_DYN_REG - 0x2000] & ST25DV_RF_MNGT_RFSLEEP_MASK);

  save->ro2004_i2c_sso_dyn                 = !!(save->buf_dynregs[ST25DV_I2C_SSO_DYN_REG - 0x2000] & ST25DV_I2C_SSO_DYN_I2CSSO_MASK);
  save->ro2004_i2c_sso_dyn                 = !!(save->buf_dynregs[ST25DV_I2C_SSO_DYN_REG - 0x2000] & ST25DV_I2C_SSO_DYN_I2CSSO_MASK);

  save->rc2005_it_sts_dyn.rf_user          = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFUSERSTATE_MASK);
  save->rc2005_it_sts_dyn.rf_activity      = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFACTIVITY_MASK);
  save->rc2005_it_sts_dyn.rf_interrupt     = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFINTERRUPT_MASK);
  save->rc2005_it_sts_dyn.field_falling    = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_FIELDFALLING_MASK);
  save->rc2005_it_sts_dyn.field_rising     = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_FIELDRISING_MASK);
  save->rc2005_it_sts_dyn.rf_put_msg       = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFPUTMSG_MASK);
  save->rc2005_it_sts_dyn.rf_get_msg       = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFGETMSG_MASK);
  save->rc2005_it_sts_dyn.rf_write         = !!(save->buf_dynregs[ST25DV_ITSTS_DYN_REG - 0x2000] & ST25DV_ITSTS_DYN_RFWRITE_MASK);

  save->rw2006_mb_ctrl_dyn.MbEnable        = !!(save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_MBEN_MASK);
  save->rw2006_mb_ctrl_dyn.HostPutMsg      = !!(save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_HOSTPUTMSG_MASK);
  save->rw2006_mb_ctrl_dyn.RfPutMsg        = !!(save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_RFPUTMSG_MASK);
  save->rw2006_mb_ctrl_dyn.HostMissMsg     = !!(save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_HOSTMISSMSG_MASK);
  save->rw2006_mb_ctrl_dyn.RFMissMsg       = !!(save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_RFMISSMSG_MASK);
  save->rw2006_mb_ctrl_dyn.CurrentMsg      = (save->buf_dynregs[ST25DV_MB_CTRL_DYN_REG - 0x2000] & ST25DV_MB_CTRL_DYN_CURRENTMSG_MASK) >> ST25DV_MB_CTRL_DYN_CURRENTMSG_SHIFT;
  save->ro2007_mb_len_dyn                  = save->buf_dynregs[ST25DV_MBLEN_DYN_REG - 0x2000];

  /* Read User Memory Areas 1,2,3,4 (volatile) */
  {
    uint16_t s, e;
    if(0x7f != save->ro0014_mem_size) return 0x10000000;
    if((s = 0, e = (save->rS0005_enda1 + 1) << 5, e - s && (r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, s, save->buf_nfctag + s, e - s)))) return r | 0x08000000;
    if((s = e, e = (save->rS0007_enda2 + 1) << 5, e - s && (r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, s, save->buf_nfctag + s, e - s)))) return r | 0x04000000;
    if((s = e, e = (save->rS0009_enda3 + 1) << 5, e - s && (r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, s, save->buf_nfctag + s, e - s)))) return r | 0x02000000;
    if((s = e, e = 0x200                        , e - s && (r = NFC_ReadReg(ST25DV_ADDR_DATA_I2C, s, save->buf_nfctag + s, e - s)))) return r | 0x01000000;
  }
  return 0;
}
