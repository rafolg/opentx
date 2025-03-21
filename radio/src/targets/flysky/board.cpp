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

#include "opentx.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C"
{
#endif
#include "usb_dcd_int.h"
#include "usb_bsp.h"
#if defined(__cplusplus) && !defined(SIMU)
}
#endif

extern "C"
{
  __attribute__((naked)) __attribute__((used)) void HardFault_HandlerAsm(void)
  {

    __asm(".syntax unified\n"
          "MOVS R0, #4 \n"
          "MOV R1, LR \n"
          "TST R0, R1 \n"
          "BEQ _MSP \n"
          "MRS R0, PSP \n"
          "B HardFault_HandlerC \n"
          "_MSP: \n"
          "MRS R0, MSP \n"
          "B HardFault_HandlerC \n"
          ".syntax divided\n");
  }

  __attribute__((used)) void HardFault_HandlerC(unsigned long *hardfault_args)
  {
    /*
  volatile unsigned long stacked_r0 ;
  volatile unsigned long stacked_r1 ;
  volatile unsigned long stacked_r2 ;
  volatile unsigned long stacked_r3 ;
  volatile unsigned long stacked_r12 ;
  volatile unsigned long stacked_lr ;
  volatile unsigned long stacked_pc ;
  volatile unsigned long stacked_psr ;
  volatile unsigned long _CFSR ;
  volatile unsigned long _HFSR ;
  volatile unsigned long _DFSR ;
  volatile unsigned long _AFSR ;
  volatile unsigned long _BFAR ;
  volatile unsigned long _MMAR ;

  stacked_r0 = ((unsigned long)hardfault_args[0]) ;
  stacked_r1 = ((unsigned long)hardfault_args[1]) ;
  stacked_r2 = ((unsigned long)hardfault_args[2]) ;
  stacked_r3 = ((unsigned long)hardfault_args[3]) ;
  stacked_r12 = ((unsigned long)hardfault_args[4]) ;
  stacked_lr = ((unsigned long)hardfault_args[5]) ;
  stacked_pc = ((unsigned long)hardfault_args[6]) ;
  stacked_psr = ((unsigned long)hardfault_args[7]) ;

  // Configurable Fault Status Register
  // Consists of MMSR, BFSR and UFSR
  _CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

  // Hard Fault Status Register
  _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

  // Debug Fault Status Register
  _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

  // Auxiliary Fault Status Register
  _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

  // Read the Fault Address Registers. These may not contain valid values.
  // Check BFARVALID/MMARVALID to see if they are valid values
  // MemManage Fault Address Register
  _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
  // Bus Fault Address Register
  _BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;
*/
    __asm("BKPT #0\n"); // Break into the debugger
  }
}

#if defined(STM32F0) && defined(BOOT)
volatile uint32_t __attribute__((section(".ram_vector,\"aw\",%nobits @"))) ram_vector[VECTOR_TABLE_SIZE];
extern volatile uint32_t g_pfnVectors[VECTOR_TABLE_SIZE];
#endif

//audio
void buzzerInit()
{
  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Pin = BUZZER_GPIO_PIN;
  gpio_init.GPIO_Mode = GPIO_Mode_AF;
  gpio_init.GPIO_OType = GPIO_OType_PP;
  gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(BUZZER_GPIO_PORT, &gpio_init);

  GPIO_PinAFConfig(BUZZER_GPIO_PORT, BUZZER_GPIO_PinSource, GPIO_AF_2);
}

void referenceSystemAudioFiles()
{
}

#define __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH()  do {SYSCFG->CFGR1 &= ~(SYSCFG_CFGR1_MEM_MODE); \
                                             SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE_0;  \
                                            }while(0)

void SystemBootloaderJump() {
    typedef void (*pFunction)(void);
    pFunction JumpToApplication;

    RCC_DeInit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    __disable_irq();

    __DSB();
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    __DSB();
    __ISB();

    JumpToApplication = (void (*)(void)) (*((uint32_t *) ((0x1FFFC800 + 4))));

    __set_MSP(*(__IO uint32_t*) 0x1FFFC800);

    __enable_irq();

    JumpToApplication();
}

void watchdogInit(unsigned int duration)
{
  IWDG->KR = 0x5555;    // Unlock registers
  IWDG->PR = 3;         // Divide by 32 => 1kHz clock
  IWDG->KR = 0x5555;    // Unlock registers
  IWDG->RLR = duration; // 1.5 seconds nominal
  IWDG->KR = 0xAAAA;    // reload
  IWDG->KR = 0xCCCC;    // start
}

void initBuzzerTimer()
{
   PWM_TIMER->PSC = 48 - 1; // 48MHz -> 1MHz
   /* set counter mode */
   PWM_TIMER->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
   PWM_TIMER->CR1 |= TIM_CounterMode_Up;
   /* Auto-Reload Register */
   PWM_TIMER->ARR = 400; // count up to
   /* Set Clock Division */
   PWM_TIMER->CR1 &= ~ TIM_CR1_CKD;
   PWM_TIMER->CR1 |= TIM_CKD_DIV1;
   PWM_TIMER->CCR1 = 200; // ARR/2 = PWM duty 50%
   /* Set repetition counter */
   PWM_TIMER->RCR = 0;

  // Timer output mode PWM
  /* Select the Output Compare (OC) Mode 1 */
  PWM_TIMER->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // = TIM_OCMODE_PWM1
  /* Reset and set the Output N Polarity level to LOW */
  // TIM1->CCER &= ~TIM_CCER_CC1P; 
  PWM_TIMER->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1E; // = TIM_OCPOLARITY_LOW + enable Capture compare channel
  /* Enable the main output */
  PWM_TIMER->BDTR |= TIM_BDTR_MOE;
}

void boardInit()
{
#if defined(STM32F0) && defined(BOOT)
  // Move vect table to beginning of RAM
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  for (uint32_t i = 0; i < VECTOR_TABLE_SIZE; i++) {
    ram_vector[i] = g_pfnVectors[i];
  }
  SYSCFG->CFGR1 = (SYSCFG->CFGR1 & ~SYSCFG_CFGR1_MEM_MODE) | (SYSCFG_CFGR1_MEM_MODE__SRAM * SYSCFG_CFGR1_MEM_MODE_0);  // remap 0x0000000 to RAM
#endif

#if !defined(SIMU)
  RCC_AHBPeriphClockCmd(RCC_AHB1_LIST, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1_LIST, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2_LIST, ENABLE);

  pwrInit();
  keysInit();

  if (readTrims() == BOOTLOADER_KEYS) {
    SystemBootloaderJump();
  }

#if defined(DEBUG) && defined(AUX_SERIAL_GPIO)
  auxSerialInit(UART_MODE_DEBUG, 0); // default serial mode (None if DEBUG not defined)
  TRACE("\ni6X board started :)");
  // TRACE("RCC->CSR = %08x", RCC->CSR);
#endif

  adcInit();
  delaysInit();
  lcdInit(); // delaysInit() must be called before
  initBuzzerTimer();
  init2MhzTimer();
  init5msTimer();
  __enable_irq();
#if defined(FLYSKY_GIMBAL)
  flysky_gimbal_init();
#endif
  buzzerInit();
  i2cInit();
  usbInit();

  //storageEraseAll(false);
  //TRACE("i2c test");
  //i2c_test();
  //while (true) ;

#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP | DBGMCU_TIM1_STOP | DBGMCU_TIM2_STOP | DBGMCU_TIM3_STOP | DBGMCU_TIM6_STOP | DBGMCU_TIM14_STOP, ENABLE);
#endif

  backlightInit();

#endif // !defined(SIMU)
}

void boardOff()
{
#if !defined(PWR_BUTTON_SWITCH) // not really useful on i6X
  BACKLIGHT_DISABLE();

//#if defined(PWR_BUTTON_PRESS)
//  while (pwrPressed())
//  {
//    wdt_reset();
//  }
//#endif
  lcdOff();
  SysTick->CTRL = 0; // turn off systick
  pwrOff();

  // disable interrupts
  __disable_irq();
#endif // PWR_BUTTON_SWITCH
  // this function must not return!
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = adcValues[TX_VOLTAGE];
  instant_vbat = (instant_vbat * 100 * (128 + g_eeGeneral.txVoltageCalibration)) / (421 * 128);
  instant_vbat += 20; // add 0.2V because of the diode
  return (uint16_t)instant_vbat;
}
