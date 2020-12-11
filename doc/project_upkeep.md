Intro
-----

The canonical (optional) IDE of this repo is STM32CubeIDE. The STM32CubeMX is integrated.

Goodies:

- Usability
	- *Omnisearch* Top-right corner, The `Quick Access` input.
	- *Hotkey list* Focus interested pane. Press `Ctrl+Shift+L`.
	- *Panes tools* `Window > Show View > Other` opens dialog of tools.
- CLI
	- *clean linefeeds* `find -type f | egrep '\.([chs]|cpp)$' | xargs -d'\n' sed -i 's/\r$//'`
	- *clean repo* `git clean -xdf; git checkout .`
- Configurability
	- *Project configs* Project > Properties
	- *Workspace configs* Window > Preferences
- Paths
	- *Project path* Project > Properties > Resource > Location
	- *Workspace path* Project > Properties > Linked Resources > Path Variables
	- *File path* `Alt+Enter` or File > Properties > Resource > Path
	- *Source path* Project > Properties > C/C++ General > Paths and Symbols > Source Location
	- *Linker path* Project > Properties > C/C++ Build > Settings > Tool Settings > MCU GCC Linker > General > Linker Script (-T)
	- *Include path* Project > Properties > C/C++ Build > Settings > Tool Settings > MCU GCC Complier > Include paths > Include paths (-I)
	- *STM32CubeMX repo path* Window > Preferences > STM32Cube > Firmware Updater > Firmware installation repository > Browse

Terminology:

- Dev Tools
	- *STM32CubeIDE* A distribution of Eclipse IDE. Tailored for C/C++ MCU development, device configuration and code generation.
	- *CDT* (C/C++ development tooling) plugin for Eclipse. Necessary for any C development in Eclipse.
	- *Eclipse* a runtime environment for plugins. Everything seen within the IDE is the result of plugins.
	- *CMSIS* (cortex microcontroller software interface standard). Vendor-independent HAL (hardware abstraction layer) for MCUs based on ARM Cortex processors. Facilitates software re-use.
		- Comprised of discrete components, like: CMSIS-Core (Cortex-M) v5.4.0, CMSIS-Core (Cortex-A) v1.2.0, CMSIS-Driver v2.8.0, CMSIS-DSP v1.8.0, etc.
- LoRa
	- *LPWAN* (low-power wide-are-network). Long-range, low-bitrate, low-power communication. Such as sensors operated on a battery. 300 bits/s to 50 kbits/s per channel. Up to 10 km in rural areas. Up to 20 years on inexpensive batteries. Uses licese-free or licensed bands. Star topology, where terminals (nodes) communicate with a gateway (base station). Communication between gateways and network servers happen via IP connection. Technologies: LoRa (proprietary), Sigfox, DASH7, Wize (open, royalty-free).
	- *LoRa* (long range) is LPWAN protocol developed by Semtech. LoRa protocol covers the physical layer. LoRaWAN covers the upper layers. Proprietary spread spectrum modulation derived from CSS. Uses license-free bands: 433MHz, 868MHz (Europe), 915MHz (Australia, North America), 923MHz (Asia). Long-rane: up to 10 km in rural area, with low power consumption. High wireless link budgets: 155 dB to 170 dB.
	- *LoRaWAN* A cloud-based MAC (medium access control) layer protocol. Manages communication frequencies, data rata, and power for devices.

Sources
-------

Metadata:

| Path            | Created by   | Useful    | Desc                                                                                                                           | URL                                                                                                             |
|-----------------|--------------|-----------|--------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| ./ibt-1.ioc     | STM32CubeMX  | Manditory | INI. Configuration explaining target MCU device, used to generate sources.                                                     |                                                                                                                 |
| ./.mxproject    | STM32CubeMX  | No        | INI. Describes previous header, source paths, and defines. Unknown use.                                                        | N/A                                                                                                             |
| ./.editorconfig | Human        | Yes       | INI. Common config, describes text file conventions: tabs, linefeeds, etc.                                                     | https://editorconfig.org/                                                                                       |
| ./.project      | STM32CubeIDE | Manditory | XML. Eclipse plugin-independent config: project name, builders and natures.                                                    | https://help.eclipse.org/latest/topic/org.eclipse.platform.doc.isv/reference/misc/project_description_file.html |
| ./.cproject     | STM32CubeIDE | Manditory | XML. Eclipse CDT config: build profiles, target device/toolchain, include paths, defines, compiler/linker/assembler/tool knobs | https://help.eclipse.org/latest/org.eclipse.cdt.doc.isv/reference/extension-points/index.html                   |
| ./.settings/    | STM32CubeIDE | No        | INI&XML. Target MCU, CDT version, atolic truestudio info, .svd paths, cdt checkers, build profile info                         |                                                                                                                 |
| ./mx.scratch    | STM32CubeIDE | No        | XML. Workspace info: absolute/relative paths, workspace names, defines, linker info.                                           |                                                                                                                 |

Code:

- `Core/` generated by *stm32cubemx*
- `Drivers/` copied from *stm32cubel0*
- `ST25DV/` inherited from *stm25dvfw*

- *stm32cubel0* `STM32Cube_FW_L0_V1.11.2/` MCU Package for STM32L0 series.
	- *Obtained:* (via STM32CubeIDE) Help > Manage embedded software packages > STM32Cube MCU Packgages > STM32L0 > STM32Cube MCU Package for STM32L0 Series > Install Now
	- *Local Path:* (via STM32CubeIDE) Window > Preferences > STM32Cube > Firmware Updater > Firmware Installation repository > Browse
	- *Upstream:* <https://www.st.com/en/embedded-software/stm32cubel0.html>
- *cmsis*
	- *Obtained:* Bundled with *stm32cubel0*
	- *Upstream:* <https://github.com/ARM-software/CMSIS_5>
- *stm25dvfw*
	- *Obtained:* Manual import `en.stsw-st25dv001.zip/ST25DV_DISCOVERY_FW_V1.1.2/Drivers/BSP/Components/ST25DV/` via `https://www.st.com/en/embedded-software/stsw-st25dv001.html` from <https://www.st.com/en/embedded-software/stsw-st25dv001.html> (login required)
	- *Upstream:* <https://www.st.com/en/embedded-software/stsw-st25dv001.html>

Tools:

- *stm32cubemx*
	- *Obtained:* <https://www.st.com/en/development-tools/stm32cubemx.html>
- *stm32cubeide*
	- *Obtained:* <https://www.st.com/en/development-tools/stm32cubeide.html>

From Scratch
------------

Walkthrough of how project was *initially* made, and how can be made again. Used STM32CubeIDE method.

Create a project:

a. Via STM32CubeMX v5.6.1 2020
	1. File > New Project...
		- MCU/MPU Selector > MCU/MPU Filters > Part Number Search: STM32L071KZ
		- Select STM32L071KZUx, the UFQFPN32 Package.
		- `Start Project` button in top-right corner.
	2. Pinout view > Rotate 90 clockwise
	3. File > Save Project
b. Via STM32CubeIDE v1.3.1 6291_20200406_0752.
	1. File > New > STM32 Project
		- MCU/MPU Selector > MCU/MPU Filters > Part Number Search: STM32L071KZ
		- Select STM32L071KZUx, the UFQFPN32 Package.
		- `Next` button in bottom-right corner.
	2. Fill out next modal
		- Project Name: ibt-1
		- Use default location: false
		- Location: /tmp/repo
		- `Next` button in bottom-right corner.
	3. Configure IDE
		- Click on blue `'Firmware Updater'` (opens dialog as from Window > Preferences)
			- Window > Preferences > STM32Cube > Firmware Updater > Firmware installation repository: /somewhere/big/Repository
			- Window > Preferences > STM32Cube > Device Configuration Tool > Do code generation on .ioc file save?: true
			- Window > Preferences > STM32Cube > Device Configuration Tool > Do switch to dedicated perspective?: true
			- `Apply & Close` button in bottom-right corner.
		- `Finish` button in bottom-right corner.
	4. Focus on STM32CubeMX pane opened via `ibt-1.ioc`.
		- Pinout view > Rotate 90 clockwise



Setup *Pinout & Configuration*:

1. Enable CRC
	- `bootloader        | VP_CRC_VS_CRC.Mode=CRC_Activate                                      ` Computing > CRC > Mode > Activated: true
	- `bootloader        | CRC.InputDataFormat=CRC_INPUTDATA_FORMAT_WORDS                       ` Computing > CRC > Configuration > Advanced Parameters > Input Data Format: Words
2. Enable NFC
	- `mainfw,bootloader | Mcu.IP1=I2C1                                                         ` Connectivity > I2C1 > Mode > I2C: I2C
	- `mainfw,bootloader | I2C1.I2C_Speed_Mode=I2C_Fast_Plus                                    ` Connectivity > I2C1 > Configuration > Parameter Settings > Timing configuration > I2C Speed Mode: Fast Mode Plus
	- `mainfw,bootloader | Dma.I2C1_RX.0.Instance=DMA1_Channel3                                 ` Connectivity > I2C1 > Configuration > DMA Settings > Add > I2C1_RX
	- `mainfw,bootloader | NVIC.DMA1_Channel2_3_IRQn=true\:0\:0\:false\:false\:true\:false\:true` System Core > NVIC > Configuration > NVIC > DMA1 channel 2 and channel 3 interrupt > Enabled: true
	- `mainfw,bootloader | NVIC.I2C1_IRQn=true\:0\:0\:false\:false\:true\:true\:true            ` System Core > NVIC > Configuration > NVIC > I2C1 event global interrupt / I2C1 wake-up interrupt through EXTI line 23 > Enabled: true
	- `mainfw,bootloader | PB6.Signal=I2C1_SCL                                                  ` Pinout view > PB6: I2C1_SCL
	- `mainfw,bootloader | PB7.Signal=I2C1_SDA                                                  ` Pinout view > PB7: I2C1_SDA
	- `N/A               | PB6.GPIO_Label=ST25DVx I2C SCL                                       ` Connectivity > I2C1 > Configuration > GPIO Settings > PB6 > User Label: ST25DVx I2C SCL
	- `N/A               | PB7.GPIO_Label=ST25DVx I2C SDA                                       ` Connectivity > I2C1 > Configuration > GPIO Settings > PB7 > User Label: ST25DVx I2C SDA
	- Rest of NFC pins
		- `mainfw,bootloader | PA1.Signal=GPXTI1                            ` Pinout view > PA1: GPIO_EXTI1
		- `mainfw,bootloader | PA1.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_FALLING` System Core > GPIO > PA1 > GPIO mode: External Interrupt Mode with Falling edge trigger detection
		- `mainfw,bootloader | PA1.GPIO_PuPd=GPIO_PULLUP                    ` System Core > GPIO > PA1 > GPIO Pull-up/Pull-down: Pull-up
		- `N/A               | PA1.GPIO_Label=ST25DVx NFC Int               ` System Core > GPIO > PA1 > User Label: ST25DVx NFC Int
3. Enable LoRa
	- `mainfw,bootloader | Mcu.IP5=SPI1                                             ` Connectivity > SPI1 > Mode > Mode: Full-Duplex Master
	- `mainfw,bootloader | SPI1.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16          ` Connectivity > SPI1 > Configuration > Parameter Settings > Clock Parameters > Prescaler (for Baud Rate): 16
	- `mainfw,bootloader | NVIC.SPI1_IRQn=true\:0\:0\:false\:false\:true\:true\:true` Connectivity > SPI1 > Configuration > NVIC Settings > SPI1 global interrupt > Enabled: true
	- `mainfw,bootloader | PA5.GPIO_Label=SX126x SPI SCK                            ` Connectivity > SPI1 > Configuration > GPIO Settings > PA5 > User Label: SX126x SPI SCK
	- `mainfw,bootloader | PA6.GPIO_Label=SX126x SPI MISO                           ` Connectivity > SPI1 > Configuration > GPIO Settings > PA6 > User Label: SX126x SPI MISO
	- `mainfw,bootloader | PA7.GPIO_Label=SX126x SPI MOSI                           ` Connectivity > SPI1 > Configuration > GPIO Settings > PA7 > User Label: SX126x SPI MOSI
	- Rest of LoRa pins
		- `mainfw,bootloader | PB5.Signal=GPXTI5                                            ` Pinout view > PB5: GPIO_EXTI5
		- `mainfw,bootloader | PB5.GPIO_Label=SX126x DIO1                                   ` System Core > GPIO > PB5 > User Label: SX126x DIO1
		- `maonfw,bootloader | NVIC.EXTI4_15_IRQn=true\:0\:0\:false\:false\:true\:true\:true` System Core > NVIC > Configuration > NVIC > EXTI line 4 to 15 interrupts > Enabled: true
		- `mainfw,bootloader | PB4.Signal=GPIO_Output                                       ` Pinout view > PB4: GPIO_Output
		- `mainfw,bootloader | PB4.GPIO_Label=SX126x Reset                                  ` System Core > GPIO > PB4 > User Label: SX126x Reset
		- `mainfw,bootloader | PA12.Signal=GPIO_Output                                      ` Pinout view > PA12: GPIO_Output
		- `mainfw,bootloader | PA12.GPIO_Label=SX126x DIO3                                  ` System Core > GPIO > PA12 > User Label: SX126x DIO3
		- `mainfw,bootloader | PA12.GPIO_Speed=GPIO_SPEED_FREQ_HIGH                         ` System Core > GPIO > PA12 > Maximum output speed: High
		- `mainfw,bootloader | PA11.Signal=GPIO_Input                                       ` Pinout view > PA11: GPIO_Input
		- `mainfw,bootloader | PA11.GPIO_Label=SX126x Busy                                  ` System Core > GPIO > PA11 > User Label: SX126x Busy
		- `mainfw,bootloader | PA11.GPIO_PuPd=GPIO_PULLUP                                   ` System Core > GPIO > PA11 > GPIO Pull-up/Pull-down: Pull-up
		- `mainfw,bootloader | PA4.Signal=GPIO_Output                                       ` Pinout view > PA4: GPIO_Output
		- `mainfw,bootloader | PA4.GPIO_Label=SX126x SPI NSS                                ` System Core > GPIO > PA4 > User Label: SX126x SPI NSS
		- `mainfw,bootloader | PA4.GPIO_Speed=GPIO_SPEED_FREQ_HIGH                          ` System Core > GPIO > PA4 > Maximum output speed: High
3. Enable Main Button
	- `mainfw            | PA0.Signal=GPXTI0                                          ` Pinout view > PA0: GPIO_EXTI0
	- `mainfw            | PA0.GPIO_Label=Button0                                     ` System Core > GPIO > PA0 > User Label: Button0
	- `mainfw            | PA0.GPIO_ModeDefaultEXTI=GPIO_MODE_IT_RISING_FALLING       ` System Core > GPIO > PA0 > GPIO mode: External Interrupt Mode with Rising/Falling edge trigger detection
	- `mainfw            | PA0.GPIO_PuPd=GPIO_PULLDOWN                                ` System Core > GPIO > PA0 > GPIO Pull-up/Pull-down: Pull-down
	- `mainfw,bootloader | NVIC.EXTI0_1_IRQn=true\:0\:0\:true\:false\:true\:true\:true` System Core > NVIC > EXTI line 0 and line 1 interrupts > Enabled: true
	- `mainfw,bootloader | NVIC.EXTI0_1_IRQn=true\:1\:0\:true\:false\:true\:true\:true` System Core > NVIC > EXTI line 0 and line 1 interrupts > Preemption Priority: 1
4. Enable LEDs
	- `mainfw,bootloader | PA8.Signal=GPIO_Output` Pinout view > PA8: GPIO_Output
	- `mainfw,bootloader | PA8.GPIO_Label=LED 1  ` System Core > GPIO > PA8 > User Label: LED 1
	- `mainfw,bootloader | PA9.Signal=GPIO_Output` Pinout view > PA9: GPIO_Output
	- `mainfw,bootloader | PA9.GPIO_Label=LED 2  ` System Core > GPIO > PA9 > User Label: LED 2
5. Enable ADC (Measure temperature or battery)
	- `mainfw            | VP_ADC_Vref_Input.Signal=ADC_Vref_Input                            ` Analog > ADC > Mode > Vrefint Channel: true
	- `mainfw            | NVIC.DMA1_Channel1_IRQn=true\:0\:0\:false\:false\:true\:false\:true` Analog > ADC > Configuration > DMA Settings > Add: ADC
		- `mainfw            | Dma.ADC.1.Mode=DMA_CIRCULAR                                ` Analog > ADC > Configuration > DMA Settings > ADC > Mode: Circular
	- `mainfw            | ADC.ContinuousConvMode=ENABLE                                              ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > Continuous Conversion Mode: Enabled
	- `mainfw            | ADC.DMAContinuousRequests=ENABLE                                   ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > DMA Continuous Requests: Enabled
	- `mainfw            | ADC.EOCSelection=ADC_EOC_SEQ_CONV                                  ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > End Of Conversion Selection: End of sequence of conversion
	- `mainfw,bootloader | ADC.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4                        ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > Clock Prescaler: Synchronous clock mode divided by 4
	- `mainfw,bootloader | ADC.LowPowerAutoWait=ENABLE                                        ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > Low Power Auto Wait: Enabled
	- `mainfw,bootloader | ADC.LowPowerFrequencyMode=ENABLE                                   ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > Low Frequency Mode: Enabled
	- `mainfw,bootloader | ADC.LowPowerAutoPowerOff=ENABLE                                    ` Analog > ADC > Configuration > Parameter Settings > ADC_Settings > Auto Off: Enabled
	- `mainfw,bootloader | ADC.SamplingTime=ADC_SAMPLETIME_160CYCLES_5                        ` Analog > ADC > Configuration > Parameter Settings > ADC_Regular_ConversionMode > Samling Time: 160.5 Cycles
	- `mainfw,bootloader | NVIC.ADC1_COMP_IRQn=true\:0\:0\:false\:false\:true\:true\:true     ` System Core > NVIC > Configuration > NVIC > DMA1 channel 1 interrupt > Enabled: true
7. Enable Internal Watchdog.
	- `mainfw,bootloader | VP_IWDG_VS_IWDG.Mode=IWDG_Activate` System Core > IWDG > Mode > Activated: true
	- `mainfw,bootloader | IWDG.Prescaler=IWDG_PRESCALER_256 ` System Core > IWDG > Configuration > Parameter Settings > Watchdog Cloacking > IWDG counter clock prescaler: 256
8. Enable HW Debugging
	- `mainfw,bootloader |                             ` System Core > SYS > Mode > Debug Serial Wire: true
	- `mainfw,bootloader | PA13.GPIO_Label=SWDIO STLink` System Core > GPIO > SYS > PA13 > User Label: SWDIO STLink
	- `mainfw,bootloader | PA14.GPIO_Label=SWCLK STLink` System Core > GPIO > SYS > PA14 > User Label: SWCLK STLink
9. Enable LSE Clock
	- `mainfw,bootloader |                                                          ` System Core > RCC > Mode > Low Speed Clock (LSE): Crystal/Ceramic Resonator
	- `mainfw            | RCC.VDD_VALUE=2.8                                        ` System Core > RCC > Configuration > Parameter Settings > System Parameters > VDD Voltage (V): 2.8V
	- `mainfw,bootloader | NVIC.RCC_IRQn=true\:0\:0\:false\:false\:true\:true\:false` System Core > RCC > Configuration > NVIC Settings > RCC global interrupt > Enabled: true
10. Enable LPTIM1 Clock
	- `mainfw,bootloader | VP_LPTIM1_VS_LPTIM_counterModeInternalClock.Mode=Counts__internal_clock_event_00` Timers > LPTIM1 > Mode > Mode: Counts internal clock events
	- `mainfw,bootloader | LPTIM1.ClockPrescaler=LPTIM_PRESCALER_DIV128                                    ` Timers > LPTIM1 > Configuration > Parameter Settings > Clock > Clock Prescaler > Prescaler Div128
	- `mainfw,bootloader | NVIC.LPTIM1_IRQn=true\:0\:0\:false\:false\:true\:true\:true                     ` Timers > LPTIM1 > Configuration > NVIC Settings > LPTIM1 global interrupt / LPTIM1 wake-up interrupt through EXTI line 29 > Enabled: true
11. Enable RTC Clock
	- `mainfw,bootloader | VP_RTC_VS_RTC_Activate.Mode=RTC_Enabled                 ` Timers > RTC > Mode > Active Clock Source: true
	- `mainfw,bootloader | VP_RTC_VS_RTC_WakeUp_intern.Mode=WakeUp                 ` Timers > RTC > Mode > Internal WakeUp: true
	- `mainfw            | RTC.WakeUpClock=RTC_WAKEUPCLOCK_CK_SPRE_16BITS          ` Timers > RTC > Configuration > Parameter Settings > Wake UP > Wake Up Clock: 1 Hz
	- `mainfw,bootloader | NVIC.RTC_IRQn=true\:0\:0\:false\:false\:true\:true\:true` Timers > RTC > Configuration > NVIC Settings > RTC global interrupt through EXTI lines 17, 19 and 20 and LSE CSS interrupt through EXTI line 19 > Enabled: true
12. Enable MISC
	- `mainfw            | PB0.Signal=GPIO_Output     ` Pinout view > PB0: GPIO_Output
	- `mainfw            | PB0.GPIO_Label=DC Conv Mode` System Core > GPIO > PB0 > User Label: DC Conv Mode
	- `mainfw            | PB0.GPIO_PuPd=GPIO_PULLDOWN` System Core > GPIO > PB0 > GPIO Pull-up/Pull-down: Pull-down
	- `mainfw,bootloader | PA2.Signal=GPIO_Output     ` Pinout view > PA2: GPIO_Output
	- `mainfw,bootloader | PA2.GPIO_Label=RF Switch   ` System Core > GPIO > PA2 > User Label: RF Switch
	- `mainfw,bootloader | PA2.GPIO_PuPd=GPIO_PULLDOWN` System Core > GPIO > PA2 > GPIO Pull-up/Pull-down: Pull-down

Setup *Clock Configuration*:

1. `mainfw,bootloader | RCC.RTCClockSelection=RCC_RTCCLKSOURCE_LSE     ` RTC Source Mux: LSE (Top-left quadrant)
2. `mainfw,bootloader | RCC.LptimClockSelection=RCC_LPTIM1CLKSOURCE_LSE` LPTIM Source Mux: LSE (Bottom-right quadrant)

Setup *Additional Software* (skipped):

1. Import ST25DV Drivers
	- Pack > STMicroelectronics.X-CUBE-NFC4 > Board Component NFC > NFC4/ST25DV > Selection: true
	- Pack > STMicroelectronics.X-CUBE-NFC4 > Board Component NFC > Version: 1.5.2
	- Notes: Skipped due to STM32CubeMX dependency issue that wontfix, Drivers/ST25DV/ not imported, Core/ license switch BSD-3 to SLA4.

Setup *Project Manager*:

1. `ProjectManager.TargetToolchain=STM32CubeIDE` Project > Project Settings > Toolchain / IDE: STM32CubeIDE
2. `ProjectManager.HeapSize=0x0                ` Project > Linker Settigs > Minimum Heap Size: 0x0
3. `ProjectManager.StackSize=0x100             ` Project > Linker Settigs > Minimum Stack Size: 0x100
4. `ProjectManager.CoupleFile=true             ` Code Generator > Generaterd files > Generate peripheral initialization as a pair of '.c/.h' files per peripheral: true
5. `ProjectManager.FreePins=true               ` Code Generator > HAL Settings > Set all free pins as analog (to optimize the power sonsumption): true
6. `ProjectManager.functionlistsort=           ` Advanced Settings > Static Visibility: MX_GPIO_Init, MX_DMA_Init, MX_CRC_Init
7. `ProjectManager.functionlistsort=           ` Advanced Settings > Rank: GPIO, DMA, RCC, ADC, I2C1, IWDG, LPTIM1, RTC, SPI1, CRC
	- CRC Last
8. `ProjectManager.functionlistsort=           ` Advanced Settings > Not Generalte Function Call: All unchecked
9. `ProjectManager.functionlistsort=           ` Advanced Settings > Driver Selector: All HAL

a. Via STM32CubeMX v5.6.1 2020
	- Press the large blue `GENERATE CODE` at Top-right quadrant of the window.
b. Via STM32CubeIDE v1.3.1 6291_20200406_0752.
	- Project > Generate Code
	- Project > Generate Report

Generated filesystem (99 files):


```
    171 ./.cproject
     43 ./.mxproject
     33 ./.project
     27 ./.settings/language.settings.xml
     58 ./Core/Inc/adc.h
     58 ./Core/Inc/crc.h
     56 ./Core/Inc/dma.h
     57 ./Core/Inc/gpio.h
     58 ./Core/Inc/i2c.h
     58 ./Core/Inc/iwdg.h
     58 ./Core/Inc/lptim.h
    111 ./Core/Inc/main.h
     58 ./Core/Inc/rtc.h
     58 ./Core/Inc/spi.h
    302 ./Core/Inc/stm32l0xx_hal_conf.h
     74 ./Core/Inc/stm32l0xx_it.h
    127 ./Core/Src/adc.c
     82 ./Core/Src/crc.c
     66 ./Core/Src/dma.c
    143 ./Core/Src/gpio.c
    149 ./Core/Src/i2c.c
     48 ./Core/Src/iwdg.c
     90 ./Core/Src/lptim.c
    209 ./Core/Src/main.c
     99 ./Core/Src/rtc.c
    116 ./Core/Src/spi.c
     89 ./Core/Src/stm32l0xx_hal_msp.c
    280 ./Core/Src/stm32l0xx_it.c
    159 ./Core/Src/syscalls.c
     58 ./Core/Src/sysmem.c
    279 ./Core/Src/system_stm32l0xx.c
    296 ./Core/Startup/startup_stm32l071kzux.s
   6497 ./Drivers/CMSIS/Device/ST/STM32L0xx/Include/stm32l071xx.h
    238 ./Drivers/CMSIS/Device/ST/STM32L0xx/Include/stm32l0xx.h
    109 ./Drivers/CMSIS/Device/ST/STM32L0xx/Include/system_stm32l0xx.h
    136 ./Drivers/CMSIS/Include/arm_common_tables.h
     79 ./Drivers/CMSIS/Include/arm_const_structs.h
   7154 ./Drivers/CMSIS/Include/arm_math.h
    734 ./Drivers/CMSIS/Include/cmsis_armcc.h
   1800 ./Drivers/CMSIS/Include/cmsis_armcc_V6.h
   1373 ./Drivers/CMSIS/Include/cmsis_gcc.h
    798 ./Drivers/CMSIS/Include/core_cm0.h
    914 ./Drivers/CMSIS/Include/core_cm0plus.h
   1763 ./Drivers/CMSIS/Include/core_cm3.h
   1937 ./Drivers/CMSIS/Include/core_cm4.h
   2512 ./Drivers/CMSIS/Include/core_cm7.h
     87 ./Drivers/CMSIS/Include/core_cmFunc.h
     87 ./Drivers/CMSIS/Include/core_cmInstr.h
     96 ./Drivers/CMSIS/Include/core_cmSimd.h
    926 ./Drivers/CMSIS/Include/core_sc000.h
   1745 ./Drivers/CMSIS/Include/core_sc300.h
   3424 ./Drivers/STM32L0xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h
    456 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal.h
   1192 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_adc.h
    216 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_adc_ex.h
    365 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_cortex.h
    344 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_crc.h
    153 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_crc_ex.h
    190 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_def.h
    675 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_dma.h
    378 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_flash.h
    811 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_flash_ex.h
    108 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_flash_ramfunc.h
    326 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_gpio.h
   2506 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_gpio_ex.h
    782 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_i2c.h
    179 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_i2c_ex.h
    239 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_iwdg.h
    725 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_lptim.h
    120 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_lptim_ex.h
    460 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_pwr.h
     99 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_pwr_ex.h
   1738 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_rcc.h
   2021 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_rcc_ex.h
    942 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_rtc.h
   1348 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_rtc_ex.h
    730 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_spi.h
   1705 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_tim.h
    246 ./Drivers/STM32L0xx_HAL_Driver/Inc/stm32l0xx_hal_tim_ex.h
    618 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
   2494 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc.c
    348 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.c
    415 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
    518 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc.c
    225 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc_ex.c
    885 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
    769 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
   1274 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
    521 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c
    531 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
   6502 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c
    337 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c
    264 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_iwdg.c
   2109 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_lptim.c
    703 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
    184 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
   1490 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
   1215 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
   1843 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.c
   1833 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.c
   3855 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.c
   6297 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c
    424 ./Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c
    175 ./STM32L071KZUX_FLASH.ld
    276 ./ibt-1.ioc
   2742 ./ibt-1.pdf
    112 ./ibt-1.txt
     87 ./mx.scratch
  94079 total
```

Project metadata (advanced)
---------------------------

The `.cproject`:

- Exposition:
	- `$a` is `com.st.stm32cube.ide.mcu`
	- `$s` references the aforementioned *superClass*, useful for enumerated *valueType*.
	- The trailing slash `/` at node-path implies whether it's a delimited node or is single.
	- The trailing verbatim after GUI value, is the content of XML `@value` attribute. If it's missing, it's same as GUI value.
- `folderInfo                                         N/A                                                                                ` N/A
- `folderInfo/toolChain                               com.st.stm32cube.ide.mcu.gnu.managedbuild.toolchain.exe.debug                      ` N/A
- `folderInfo/toolChain/option/                       com.st.stm32cube.ide.mcu.gnu.option.internal.toolchain.type                        ` C/C++ Build > Settings > Toolchain Version > Type: GNU Tools for STM32 `com.st.stm32cube.ide.mcu.gnu.managedbuild.toolchain.base.gnu-tools-for-stm32`
- `folderInfo/toolChain/option/                       com.st.stm32cube.ide.mcu.gnu.option.internal.toolchain.version                     ` C/C++ Build > Settings > Toolchain Version > Version: 7-2018-q2-update
- `folderInfo/toolChain/option/                       com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_mcu                        ` C/C++ Build > Settings > Tool Settings > MCU Settings > Mcu: STM32L071KZUx
- `folderInfo/toolChain/option/                       com.st.stm32cube.ide.mcu.gnu.managedbuild.option.target_board                      ` C/C++ Build > Settings > Tool Settings > MCU Settings > Board: genericBoard
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.assembler                           ` C/C++ Build > Settings > Tool Settings > MCU GCC Assembler
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.assembler.option.debuglevel         ` C/C++ Build > Settings > Tool Settings > MCU GCC Assembler > Debugging > Debug level: Maximum (-g3) `$s.value.g3`
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler                          ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.debuglevel        ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Debugging > Debug level: Maximum (-g3) `$s.value.g3`
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.optimization.level` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Optimization > Optimization level: None (-O0) *`missing`*
- `folderInfo/toolChain/tool/option                   com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.definedsymbols    ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define symbols (-D)
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define symbols (-D): USE_HAL_DRIVER
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define symbols (-D): DEBUG
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define symbols (-D): STM32L071xx
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define symbols (-D): \_\_weak="\_\_attribute\_\_((weak))" `__weak=&quot;__attribute__((weak))&quot;`
- `folderInfo/toolChain/tool/option                   com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.includepaths      ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Include paths > Include paths (-I)
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Include paths > Include paths (-I): ../Core/Inc
- `folderInfo/toolChain/tool/option/listOptionValue/  N/A                                                                                ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Include paths > Include paths (-I): "\${workspace_loc:/ibt-1/ST25DV}" `&quot;${workspace_loc:/ibt-1/ST25DV}&quot;`
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.ffunction         ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Optimization > Place functions in ther own sections (-ffunction-sections): true
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.compiler.option.fdata             ` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Optimization > Place data in ther own sections (-fdata-sections): true
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.cpp.compiler                        `
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker                            ` C/C++ Build > Settings > Tool Settings > MCU GCC Linker
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.script              ` C/C++ Build > Settings > Tool Settings > MCU GCC Linker > General > Linker Script (-T): ../STM32L071KB_FLASH.ld
- `folderInfo/toolChain/tool/option/                  com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.gcsections          ` C/C++ Build > Settings > Tool Settings > MCU GCC Linker > General > Discard unused sections (-Wl,--gc-sections): true
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.cpp.linker                          `
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.archiver                            `
- `folderInfo/toolChain/tool                          com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.size                                `

Paths are at:
- `rw` C/C++ General > Paths and Symbols > Includes
- `ro` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT Managed Build Setting Entries [Shared]
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Include Directory > [Blue] Project Path > Path: Core/Inc
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Include Directory > [Purple] Workspace Path > Path: /ibt-1/Core/Inc
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Include Directory > [Yellow] File System Path > Path: /mnt/Jiffy/tmp/Cache/workspace/nfuse/refactor/repoha/Core/Inc
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Include Directory > File System Path > Path: \${workspace_loc:ibt-1}/Core/Inc
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Include Directory > File System Path > Path: Core/Inc

Defines are at:
- `rw` C/C++ Build > Settings > Tool Settings > MCU GCC Compiler > Preprocessor > Define Symbols (-D)
- `ro` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT Managed Build Setting Entries [Shared]
- `ro` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT Managed Build Setting Entries [Shared]
- `rw` C/C++ General > Preprocessor Include Paths, Macros etc. > Entries > Languages > GNU C > CDT User Setting Entries > Add... > Preprocessor Macro

