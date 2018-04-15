/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "stddef.h"
#include "stdbool.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#if __clang__
// clang is very picky about the use of "register"
// tell it to ignore for the STM32 includes instead of modyfing the orginal files
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/CMSIS/Device/ST/STM32F0xx/Include/stm32f0xx.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_rcc.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_gpio.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_tim.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_adc.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_spi.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_i2c.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_rtc.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_pwr.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_dma.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_usart.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_flash.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_dbgmcu.h"
#include "STM32F0xx_StdPeriph_Lib_V1.5.0/Libraries/STM32F0xx_StdPeriph_Driver/inc/stm32f0xx_misc.h"

#if __clang__
// Restore warnings about registers
#pragma clang diagnostic pop
#endif


#include "usb_driver.h"
/*
#if !defined(SIMU)
  #include "usbd_cdc_core.h"
  #include "usbd_msc_core.h"
  #include "usbd_hid_core.h"
  #include "usbd_usr.h"
  #include "usbd_desc.h"
  #include "usb_conf.h"
  #include "usbd_conf.h"
#endif
*/

#include "hal.h"

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#define FLASHSIZE                       0x80000
#define BOOTLOADER_SIZE                 0x0
//#define BOOTLOADER_SIZE                 0x8000
#define FIRMWARE_ADDRESS                0x08000000

#define LUA_MEM_MAX                     (0)    // max allowed memory usage for complete Lua  (in bytes), 0 means unlimited


#define PERI1_FREQUENCY               24000000
#define PERI2_FREQUENCY               48000000


#define TIMER_MULT_APB1                 2
#define TIMER_MULT_APB2                 2

#define strcpy_P strcpy
#define strcat_P strcat

extern uint16_t sessionTimer;

// Board driver
void boardInit(void);
void boardOff(void);

// Delays driver
#ifdef __cplusplus
extern "C" {
#endif
void delaysInit(void);
void delay_01us(uint16_t nb);
void delay_us(uint16_t nb);
void delay_ms(uint32_t ms);
#ifdef __cplusplus
}
#endif
#define usbPlugged() (false)
#define usbStarted() (false)
#define getSelectedUsbMode() (USB_UNSELECTED_MODE)



// CPU Unique ID
#define LEN_CPU_UID                     (3*8+2)
void getCPUUniqueID(char * s);

// SD driver
#define BLOCK_SIZE                      512 /* Block Size in Bytes */
#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdIsHC(void);
uint32_t sdGetSpeed(void);
#define SD_IS_HC()                      (sdIsHC())
#define SD_GET_SPEED()                  (sdGetSpeed())
#define SD_GET_FREE_BLOCKNR()           (sdGetFreeSectors())
#else
#define SD_IS_HC()                      (0)
#define SD_GET_SPEED()                  (0)
#endif
#define __disk_read                     disk_read
#define __disk_write                    disk_write
#if defined(SIMU) || !defined(SDCARD)
  #if !defined(SIMU_DISKIO)
    #define sdInit()
    #define sdDone()
  #endif
  #define sdMount()
  #define SD_CARD_PRESENT()               true
#else
void sdInit(void);
void sdMount(void);
void sdDone(void);
void sdPoll10ms(void);
uint32_t sdMounted(void);
#define SD_CARD_PRESENT()               ((SD_GPIO_PRESENT_GPIO->IDR & SD_GPIO_PRESENT_GPIO_PIN) == 0)
#endif

// Flash Write driver
#define FLASH_PAGESIZE 256
void unlockFlash(void);
void lockFlash(void);
void flashWrite(uint32_t * address, uint32_t * buffer);
uint32_t isFirmwareStart(const uint8_t * buffer);
uint32_t isBootloaderStart(const uint8_t * buffer);

// Pulses driver
#define INTERNAL_MODULE_ON()            GPIO_SetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define INTERNAL_MODULE_OFF()           GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_ON()            GPIO_SetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define EXTERNAL_MODULE_OFF()           GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN)
#define IS_INTERNAL_MODULE_ON()         (GPIO_ReadInputDataBit(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN) == Bit_SET)
#define IS_EXTERNAL_MODULE_ON()         (GPIO_ReadInputDataBit(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN) == Bit_SET)
#if defined(INTMODULE_USART)
  #define IS_UART_MODULE(port)          (port == INTERNAL_MODULE)
#else
  #define IS_UART_MODULE(port)          false
#endif
void init_no_pulses(uint32_t port);
void disable_no_pulses(uint32_t port);
void init_ppm( uint32_t module_index );
void disable_ppm( uint32_t module_index );
void init_pxx( uint32_t module_index );
void disable_pxx( uint32_t module_index );
void init_dsm2( uint32_t module_index );
void disable_dsm2( uint32_t module_index );
void init_crossfire( uint32_t module_index );
void disable_crossfire( uint32_t module_index );
void init_sbusOut(uint32_t module_index);
void disable_sbusOut(uint32_t module_index);
void setupPulsesSbus(uint8_t port);
// Trainer driver
#define SLAVE_MODE()                    (g_model.trainerMode == TRAINER_MODE_SLAVE)
#if defined(PCBX9E)
  #define TRAINER_CONNECTED()           (true)
#elif defined(PCBX7)
  #define TRAINER_CONNECTED()           (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_SET)
#elif defined(PCBXLITE)
  #define TRAINER_CONNECTED()           false // there is no Trainer jack on Taranis X-Lite
#else
  #define TRAINER_CONNECTED()           (GPIO_ReadInputDataBit(TRAINER_DETECT_GPIO, TRAINER_DETECT_GPIO_PIN) == Bit_RESET)
#endif
#if defined(TRAINER_GPIO)
  void init_trainer_ppm(void);
  void stop_trainer_ppm(void);
  void init_trainer_capture(void);
  void stop_trainer_capture(void);
#else
  #define init_trainer_ppm()
  #define stop_trainer_ppm()
  #define init_trainer_capture()
  #define stop_trainer_capture()
#endif
#if defined(TRAINER_MODULE_HEARTBEAT)
  void init_cppm_on_heartbeat_capture(void);
  void stop_cppm_on_heartbeat_capture(void);
  void init_sbus_on_heartbeat_capture(void);
  void stop_sbus_on_heartbeat_capture(void);
#else
  #define init_cppm_on_heartbeat_capture()
  #define stop_cppm_on_heartbeat_capture()
  #define init_sbus_on_heartbeat_capture()
  #define stop_sbus_on_heartbeat_capture()
#endif

// SBUS
int sbusGetByte(uint8_t * byte);

// Keys driver
enum EnumKeys
{
  KEY_MENU,
  KEY_ENTER=KEY_MENU,
  KEY_EXIT,
  KEY_DOWN,
  KEY_MINUS = KEY_DOWN,
  KEY_UP,
  KEY_PLUS = KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,
  TRM_LAST = TRM_RH_UP,

  NUM_KEYS
};

enum EnumSwitches
{
  SW_SA,
  SW_SB,
  SW_SC,
  SW_SD,
  SW_SE,
  SW_SF,
  SW_SG,
  SW_SH
};

#define SW_ID0 SW_SA
#define SW_ID1 SW_SB


#define IS_3POS(x)                      ((x) == SW_SC)
#define IS_TOGGLE(x)					((x) != SW_SC)
enum EnumSwitchesPositions
{
  SW_SA0,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
#if !defined(PCBX7) && !defined(PCBXLITE)
  SW_SE0,
  SW_SE1,
  SW_SE2,
#endif
#if !defined(PCBXLITE)
  SW_SF0,
  SW_SF1,
  SW_SF2,
#endif
#if !defined(PCBX7) && !defined(PCBXLITE)
  SW_SG0,
  SW_SG1,
  SW_SG2,
#endif
#if !defined(PCBXLITE)
  SW_SH0,
  SW_SH1,
  SW_SH2,
#endif
#if defined(PCBX9E)
  SW_SI0,
  SW_SI1,
  SW_SI2,
  SW_SJ0,
  SW_SJ1,
  SW_SJ2,
  SW_SK0,
  SW_SK1,
  SW_SK2,
  SW_SL0,
  SW_SL1,
  SW_SL2,
  SW_SM0,
  SW_SM1,
  SW_SM2,
  SW_SN0,
  SW_SN1,
  SW_SN2,
  SW_SO0,
  SW_SO1,
  SW_SO2,
  SW_SP0,
  SW_SP1,
  SW_SP2,
  SW_SQ0,
  SW_SQ1,
  SW_SQ2,
  SW_SR0,
  SW_SR1,
  SW_SR2,
#endif
};


#define NUM_SWITCHES                  4

void keysInit(void);
uint8_t keyState(uint8_t index);
uint32_t switchState(uint8_t index);
uint32_t readKeys(void);
uint32_t readTrims(void);

#define TRIMS_PRESSED()                 (readTrims())
#define KEYS_PRESSED()                  (readKeys())

// WDT driver
#define WDTO_500MS                      500
#if defined(WATCHDOG_DISABLED) || defined(SIMU)
  #define wdt_enable(x)
  #define wdt_reset()
#else
  #define wdt_enable(x)                 watchdogInit(x)
  #define wdt_reset()                   IWDG->KR = 0xAAAA
#endif
#define wdt_disable()
void watchdogInit(unsigned int duration);
#define WAS_RESET_BY_SOFTWARE()             (RCC_GetFlagStatus(RCC_FLAG_SFTRST))
#define WAS_RESET_BY_WATCHDOG()             (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) || RCC_GetFlagStatus(RCC_FLAG_IWDGRST) )
#define WAS_RESET_BY_WATCHDOG_OR_SOFTWARE() (RCC_GetFlagStatus(RCC_FLAG_SFTRST) || RCC_GetFlagStatus(RCC_FLAG_WWDGRST) || RCC_GetFlagStatus(RCC_FLAG_IWDGRST))

// ADC driver
enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT_FIRST,
  POT1 = POT_FIRST,
  POT2,
  POT_LAST = POT2,
  TX_VOLTAGE,
  NUM_ANALOGS
};

#define NUM_POTS                        (POT_LAST-POT_FIRST+1)
#define NUM_XPOTS                       0						//disable xpot for now
#define NUM_SLIDERS                     (TX_VOLTAGE-POT_LAST-1)

enum CalibratedAnalogs {
  CALIBRATED_STICK1,
  CALIBRATED_STICK2,
  CALIBRATED_STICK3,
  CALIBRATED_STICK4,
  CALIBRATED_POT_FIRST,
  CALIBRATED_POT_LAST = CALIBRATED_POT_FIRST + NUM_POTS - 1,
  CALIBRATED_SLIDER_FIRST,
  CALIBRATED_SLIDER_LAST = CALIBRATED_SLIDER_FIRST + NUM_SLIDERS - 1,
  NUM_CALIBRATED_ANALOGS
};

#if defined(PCBX9D)
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT2) // POT3 is only defined in software
#else
  #define IS_POT(x)                     ((x)>=POT_FIRST && (x)<=POT_LAST)
#endif
#define IS_SLIDER(x)                    ((x)>POT_LAST && (x)<TX_VOLTAGE)
void adcInit(void);
void adcRead(void);
extern uint16_t adcValues[NUM_ANALOGS];
uint16_t getAnalogValue(uint8_t index);
uint16_t getBatteryVoltage();   // returns current battery voltage in 10mV steps

#define BATT_SCALE                    150

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

// Power driver
#define SOFT_PWR_CTRL
void pwrInit(void);
uint32_t pwrCheck(void);
void pwrOn(void);
void pwrOff(void);
uint32_t pwrPressed(void);
#if defined(PWR_PRESS_BUTTON)
uint32_t pwrPressedDuration(void);
#endif

#if defined(SIMU)
#define UNEXPECTED_SHUTDOWN()           false
#else
#define UNEXPECTED_SHUTDOWN()           (WAS_RESET_BY_WATCHDOG() || g_eeGeneral.unexpectedShutdown)
#endif

// Backlight driver
void backlightInit(void);
void backlightDisable(void);
#define BACKLIGHT_DISABLE()             backlightDisable()
uint8_t isBacklightEnabled(void);
void backlightEnable(uint8_t level);
#define BACKLIGHT_ENABLE()            backlightEnable(g_eeGeneral.backlightBright)

#if !defined(SIMU)
  void usbJoystickUpdate();
#endif
#define USB_NAME                        "FlySky I6X"
#define USB_MANUFACTURER                'F', 'l', 'y', 'S', 'k', 'y', ' ', ' '  /* 8 bytes */
#define USB_PRODUCT                     'I', '6', 'X', ' ', ' ', ' ', ' ', ' '  /* 8 Bytes */

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

// I2C driver: EEPROM + Audio Volume

#define EEPROM_SIZE                   (128*1024)
#define EEPROM_BLOCK_SIZE     			(4*1024)
void i2cInit(void);
void eepromInit();
void eepromReadBlock(uint8_t * buffer, size_t address, size_t size);
void eepromStartWrite(uint8_t * buffer, size_t address, size_t size);

void eepromBlockErase(uint32_t address);
void eepromStartRead(uint8_t * buffer, size_t address, size_t size);
uint8_t eepromIsTransferComplete();
uint8_t eepromReadStatus();



// Debug driver
void debugPutc(const char c);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate, uint8_t mode);
void telemetryPortSetDirectionOutput(void);
void sportSendBuffer(uint8_t * buffer, uint32_t count);
uint8_t telemetryGetByte(uint8_t * byte);
extern uint32_t telemetryErrors;

#define HAS_SPORT_UPDATE_CONNECTOR()  false

// Sport update driver
#if defined(SPORT_UPDATE_PWR_GPIO)
void sportUpdateInit(void);
void sportUpdatePowerOn(void);
void sportUpdatePowerOff(void);
#define SPORT_UPDATE_POWER_ON()         sportUpdatePowerOn()
#define SPORT_UPDATE_POWER_OFF()        sportUpdatePowerOff()
#else
#define sportUpdateInit()
#define SPORT_UPDATE_POWER_ON()         EXTERNAL_MODULE_ON()
#define SPORT_UPDATE_POWER_OFF()        EXTERNAL_MODULE_OFF()
#endif

// Audio driver
void audioInit(void);
void audioEnd(void);
void dacStart(void);
void dacStop(void);

#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
#if !defined(SOFTWARE_VOLUME)
void setScaledVolume(uint8_t volume);
void setVolume(uint8_t volume);
int32_t getVolume(void);
#endif
void audioConsumeCurrentBuffer();
void setSampleRate(uint32_t frequency);
void referenceSystemAudioFiles();
#define audioDisableIrq()               __disable_irq()
#define audioEnableIrq()                __enable_irq()

// Haptic driver
void hapticInit(void);
void hapticOff(void);

// Second serial port driver
#if defined(SERIAL_GPIO)
#define DEBUG_BAUDRATE                  115200
#define SERIAL2
extern uint8_t serial2Mode;
void serial2Init(unsigned int mode, unsigned int protocol);
void serial2Putc(char c);
#define serial2TelemetryInit(protocol) serial2Init(UART_MODE_TELEMETRY, protocol)
void serial2SbusInit(void);
void serial2Stop(void);
#endif

// BT driver
#define BLUETOOTH_DEFAULT_BAUDRATE      115200

void bluetoothInit(uint32_t baudrate);
void bluetoothWriteWakeup(void);
uint8_t bluetoothIsWriting(void);
void bluetoothDone(void);

// LED driver
void ledInit(void);
void ledOff(void);
void ledRed(void);
void ledGreen(void);
void ledBlue(void);

// LCD driver
#define LCD_W                           128
#define LCD_H                           64
#define LCD_DEPTH                       1
#define IS_LCD_RESET_NEEDED()           true
#define LCD_CONTRAST_MIN                10
#define LCD_CONTRAST_MAX                30
#define LCD_CONTRAST_DEFAULT            20

void lcdInit(void);
void lcdInitFinish(void);
void lcdOff(void);

// TODO lcdRefreshWait() stub in simpgmspace and remove LCD_DUAL_BUFFER
#if defined(LCD_DMA) && !defined(LCD_DUAL_BUFFER) && !defined(SIMU)
void lcdRefreshWait();
#else
#define lcdRefreshWait()
#endif
#if defined(PCBX9D) || defined(SIMU) || !defined(__cplusplus)
void lcdRefresh(void);
#else
void lcdRefresh(bool wait=true); // TODO uint8_t wait to simplify this
#endif
void lcdSetRefVolt(unsigned char val);
void lcdSetContrast(void);

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

extern uint8_t currentTrainerMode;
void checkTrainerSettings(void);

#if defined(__cplusplus)
#include "fifo.h"
#include "dmastream.h"
#include "dmafifo.h"

#if defined(CROSSFIRE)
#define TELEMETRY_FIFO_SIZE             128
#else
#define TELEMETRY_FIFO_SIZE             64
#endif

extern Fifo<uint8_t, TELEMETRY_FIFO_SIZE> telemetryFifo;
extern DMAFifo<32> serial2RxFifo;
#endif

#endif // _BOARD_H_
