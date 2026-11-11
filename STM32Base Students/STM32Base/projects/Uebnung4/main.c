/**
  ******************************************************************************
  * @file    main.c 
  * @author  Leander Kieweg
  * @version V1.0
  * @date    09-Nov-2026
  * @brief   Main program
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "sysdelay.h"
#include "board_button.h"
#include "board_led.h"
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the SysTick timer.
  * @param  None
  * @retval None
  */
void SysTick_Init(void) 
{
    /* Tick every 1 ms. */
    SysTick_Config(SystemCoreClock / 1000);
} 

/**
  * @brief  SysTick interrupt handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler()
{
    SysDelay_IncTicks();
}

/**
  * @brief  Switch SYSCLK from 48 MHz to 24 MHz.
  * @param  None
  * @retval None
  */
static void Systemconfig_Clock(void)
{
    // Switch to HSI before changing the PLL.
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    {
        // Wait for HSI.
    }

    // Disable PLL.
    RCC->CR &= ~RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) != 0)
    {
        // Wait for PLL to stop.
    }

    // PREDIV /1: PLL input = 8 MHz.
    RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV1) | RCC_CFGR2_PREDIV1_DIV1;

    // HSI/PREDIV x3: PLL output = 24 MHz.
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLSRC_HSI_PREDIV | RCC_CFGR_PLLMUL3;

    // Keep the existing flash latency.

    // Enable PLL.
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
        // Wait for PLL.
    }

    // Select PLL as system clock.
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
        // Wait for the switch.
    }

    // Update CMSIS clock variable.
    SystemCoreClock = 24000000U;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    bool alarm = false;          // Alarm active
    bool clockChanged = false;   // Clock already changed

    /* Initialize timer, buttons, and LEDs. */
    SysTick_Init();
    Button_Initialize();
    LED_Initialize();

    while (1)
    {
        uint32_t buttonState = Button_GetState();

        // Handle only the first pressed button.
        if (buttonState & BUTTON_USER0_MASK)
        {
          // Alarm zone 1.
          LED_On(LED4);
          alarm = true;
          /* Debounce delay. */
          SysDelay_Delay(200);
        }
        else if (buttonState & BUTTON_USER1_MASK)
        {
          // Alarm zone 2.
          LED_On(LED5);
          alarm = true;
          /* Debounce delay. */
          SysDelay_Delay(200);
        }
        else if (buttonState & BUTTON_WAKEUP_MASK)
        {
            // Disable alarm.
            LED_Off(LED4);
            LED_Off(LED5);
            LED_Off(LED3);
            alarm = false;

            // Change the clock once.
            if (!clockChanged)
            {
                Systemconfig_Clock();
                clockChanged = true;
            }
            /* Debounce delay. */
            SysDelay_Delay(200);
        }

        if (alarm)
        {
          // Blink siren LED.
          LED_Toggle(LED3);
          SysDelay_Delay(200);
        }
    }
}