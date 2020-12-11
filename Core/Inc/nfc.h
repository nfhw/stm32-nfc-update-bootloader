/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFC_H
#define __NFC_H

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include "st25dv.h"
#include "st25dv_reg.h"
#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef struct {
  uint8_t *pData;       /* Pointer to buffer data to transfer */
  uint32_t fulllength;  /* Informs on the total length of data to transfer */
  uint16_t chunknb;     /* Informs for a specific frame the current chunk number of that frame */
  uint16_t framesize;   /* Size of current frame to transfer */
  uint16_t totalchunk;  /* Informs on the total number of chunk that will need to perform the transfer */
  uint8_t chaining;     /* Defines if the frame is a simple frame (0) or chained frame (1) */
  uint8_t cmdresp;      /* Defines current frame command, answer, acknowledge */
  uint8_t error;        /* Error code if an error is detected */
  uint8_t fctcode;      /* Function code used to define the action of the requester */
  uint8_t framelength;  /* Informs the data length of the current frame */
} MB_HEADER_T;

struct NFC_State {
  uint8_t it_sts;
  uint8_t mb_ctrl;
  uint8_t mb_len;
  uint8_t mb[256];
};
static_assert(alignof(struct NFC_State) == 1, "Support raw buffer write from i2c");

/* NAME
 *        ST25DV_AllRegs - st25dv state snapshot held in SRAM
 *
 * DESCRIPTION
 *        rw - read-only
 *        ro - read-only
 *        rc - read-only, cleared by read
 *        ow - write-only
 *        rS - read-write, security session to write
 *        So - read-only, security session to read
 */
struct ST25DV_AllRegs {
  uint8_t buf_sysregs[0x024];
  uint8_t buf_dynregs[0x108];
  uint8_t buf_nfctag[0x200];
  uint8_t buf_mailbox[0x100];

  ST25DV_GPO rS0000_gpo;
  ST25DV_PULSE_DURATION rS0001_it_time;
  ST25DV_EH_MODE_STATUS rS0002_eh_mode;
  ST25DV_RF_MNGT rS0003_rf_mngt;
  ST25DV_RF_PROT_ZONE rS0004_rfa1ss;
  ST25DV_RF_PROT_ZONE rS0006_rfa2ss;
  ST25DV_RF_PROT_ZONE rS0008_rfa3ss;
  ST25DV_RF_PROT_ZONE rS000a_rfa4ss;
  uint8_t rS0005_enda1;
  uint8_t rS0007_enda2;
  uint8_t rS0009_enda3;
  ST25DV_I2C_PROT_ZONE rS000b_i2css;
  ST25DV_LOCK_CCFILE rS000c_lock_ccfile;
  ST25DV_EN_STATUS rS000d_mb_mode;
  enum {
    ST25DV_DISABLED,
    ST25DV_30_MS,
    ST25DV_60_MS,
    ST25DV_120_MS,
    ST25DV_240_MS,
    ST25DV_480_MS,
    ST25DV_960_MS,
    ST25DV_1920_MS,
  } rS000e_mb_wdg;
  ST25DV_LOCK_STATUS rS000f_lock_cfg;
  ST25DV_LOCK_STATUS ro0010_lock_dsfid;
  ST25DV_LOCK_STATUS ro0011_lock_afi;
  uint8_t ro0012_dsfid;
  uint8_t ro0013_afi;
  enum {
    ST25DV_MEM_04K = 0x007f,
    ST25DV_MEM_16K = 0x01ff,
    ST25DV_MEM_64K = 0x07ff,
  } ro0014_mem_size;
  uint8_t ro0016_blk_size;
  enum {
    ST25DV_IC_04K = 0x24,
    ST25DV_IC_16K_64K = 0x26,
  } ro0017_ic_ref;

  struct {
    uint64_t uid;
    uint8_t serial_nr[5];
    enum {
      ST25DV04K_IE_PC = 0x24,
      ST25DV04K_JF_PC = 0x25,
      ST25DV16K_IE_PC = 0x26,
      ST25DV64K_IE_PC = 0x26,
      ST25DV16K_JF_PC = 0x27,
      ST25DV64K_JF_PC = 0x27,
    } product_nr;
    enum {
      ST25DV_FIXED_MFG_UID = 0x02,
    } mfg;
    enum {
      ST25DV_FIXED_MSB_UID = 0xe0,
    } msb;
  } ro0018_uid;
  uint8_t ro0020_ic_rev;
  uint64_t SS0900_i2c_pwd;

  ST25DV_GPO rw2000_gpo_dyn;
  ST25DV_EH_CTRL rw2002_eh_ctrl_dyn;
  ST25DV_RF_MNGT rw2003_rf_mngt_dyn;
  ST25DV_I2CSSO_STATUS ro2004_i2c_sso_dyn;
  struct {
    bool rf_user:1;
    bool rf_activity:1;
    bool rf_interrupt:1;
    bool field_falling:1;
    bool field_rising:1;
    bool rf_put_msg:1;
    bool rf_get_msg:1;
    bool rf_write:1;
  } rc2005_it_sts_dyn;
  ST25DV_MB_CTRL_DYN_STATUS rw2006_mb_ctrl_dyn;
  uint8_t ro2007_mb_len_dyn;
};

/* Exported constants --------------------------------------------------------*/
enum {MB_NOTCHAINED, MB_CHAINED};

#define MB_FCTCODE            0
#define MB_CMDRESP            1
#define MB_ERROR              2
#define MB_CHAINING           3
#define MB_LENGTH             4
#define MB_DATA               5
#define MB_CH_FULLLENGTH      7
#define MB_CH_TOTALCHUNK      9
#define MB_CH_NBCHUNK         11
#define MB_CH_LENGTH          12
#define MB_CH_DATA            13

#define MB_MAXFUNCTION        0xFF
#define MB_R2HSIMPLETRANSFER  0x01
#define MB_R2HCHAINEDTRANFER  0x02
#define MB_R2HDATATRANSFER    0x03
#define MB_R2HFIRMWAREUPDATE  0x04
#define MB_H2RSIMPLETRANSFER  0x05
#define MB_H2RCHAINEDTRANSFER 0x06
#define MB_H2RIMAGEUPLOAD     0x07
#define MB_R2HPRESENTPASSWORD 0x08
#define MB_R2HIMAGEDOWNLOAD   0x09
#define MB_H2RDATATRANSFER    0x0A
#define MB_R2HSTOPWATCH       0x0B
#define MB_R2HGETCONFIG       0x20
#define MB_R2HSETCONFIG       0x21

#define MB_CANCELCOMMAND      0xF0
#define MB_RESETCOMMUNICATION 0xF1

#define MB_COMMAND            0
#define MB_RESPONSE           1
#define MB_ACKNOWLEDGE        2

#define MB_NOERROR            0
#define MB_DEFAULTERROR       1
#define MB_UNKNOWNFUNCTION    2
#define MB_BADREQUEST         3
#define MB_LENGTHERROR        4
#define MB_CHUNKERROR         5
#define MB_PROTOCOLERROR      6

#define ST25_RETRY_NB          ((uint8_t) 15)
#define ST25_RETRY_DELAY       ((uint8_t)  4)  /* milliseconds */
#define BUS_I2C1_POLL_TIMEOUT             100  /* milliseconds */
#define NFC_PWTIMEOUT                  120000  /* 2 minute */


/* External variables --------------------------------------------------------*/
extern ST25DV_Object_t St25Dv_Obj;
extern volatile uint8_t GPO_Activated;

/* Exported macros -----------------------------------------------------------*/
#ifdef DEBUG
#define NFC_WaitACK(Timeout)  ((NFC_WaitACK)(Timeout) ? (DBG_PRINTF("NFC I2C WAIT ERR timeout:%5d caller:%s:%d\n", Timeout, __FILE__, __LINE__), HAL_ERROR) : HAL_OK)
#endif

/* Exported functions ------------------------------------------------------- */
int32_t (NFC_WaitACK)(uint32_t Timeout);
int32_t NFC_Init(bool keepmailbox);
int32_t NFC_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t NFC_WriteReg(uint16_t DevAddr, uint16_t Reg, const uint8_t *pData, uint16_t Length);
uint32_t NFC_ReadRegAll(struct ST25DV_AllRegs * const save);
void MBDecodeHeader(const uint8_t * const pData, MB_HEADER_T * const mb_header);
void NFC_ClearMailbox(void);
void NFC_DeInit(void);

#endif /* __NFC_H */
