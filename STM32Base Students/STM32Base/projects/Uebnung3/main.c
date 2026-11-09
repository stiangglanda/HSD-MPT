/**
  ******************************************************************************
  * @file    main.c 
  * @author  Leander Kieweg
  * @version V1.0
  * @date    25-Oct-2026
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
  * @brief  Initialize the sys tick timer (M0 core) which is used for delay.
  * @param  None
  * @retval None
  */
void SysTick_Init(void) 
{
    /* init the sys tick timer to be called every 1ms */
    SysTick_Config(SystemCoreClock / 1000);
} 

/**
  * @brief  This is the SysTick interrupt handler which is called every 1ms. 
            We have to increment the HAL tick counter which is used for SysDelay
  * @param  None
  * @retval None
  */
void SysTick_Handler()
{
    SysDelay_IncTicks();
}

static void Systemconfig_Clock(void)
{
    // Switch SYSCLK to HSI first, PLL can only be configured while it is off.
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    {
        // wait until HSI is used as system clock
    }

    // Disable PLL and wait until it is stopped.
    RCC->CR &= ~RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) != 0)
    {
        // wait until PLL is unlocked
    }

    // PREDIV = /1 -> PLL input = 8 MHz
    RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV1) | RCC_CFGR2_PREDIV1_DIV1;

    // PLL source = HSI/PREDIV, PLLMUL = x3 -> PLL output = 24 MHz
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLSRC_HSI_PREDIV | RCC_CFGR_PLLMUL3;

    // Enable PLL and wait until it is locked.
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
        // wait until PLL is ready
    }

    // Select PLL as system clock and wait until the switch is done.
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
        // wait until PLL is used as system clock
    }

    // Update CMSIS variable (SysTick is intentionally NOT reconfigured)
    SystemCoreClock = 24000000U;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    uint32_t counter = 0;
    bool alarm = false;
    bool clockChanged = false;   // oben bei den Variablen

    /* Initialize the timer, buttons, and LEDs */
    SysTick_Init();
    Button_Initialize();
    LED_Initialize();

    while (1)
    {
        uint32_t buttonState = Button_GetState();

        if (buttonState & BUTTON_USER0_MASK)
        {
          LED_On(LED4);
          alarm = true;
          /* Wait 200 ms */
          SysDelay_Delay(200);
        }
        else if (buttonState & BUTTON_USER1_MASK)
        {
          LED_On(LED5);
          alarm = true;
          /* Wait 200 ms */
          SysDelay_Delay(200);
        }
        else if (buttonState & BUTTON_WAKEUP_MASK)
        {
            LED_Off(LED4);
            LED_Off(LED5);
            LED_Off(LED3);
            alarm = false;
        
            if (!clockChanged)
            {
                Systemconfig_Clock();
                clockChanged = true;
            }
            SysDelay_Delay(200);
        }

        if (alarm)
        {
          LED_Toggle(LED3);
          SysDelay_Delay(200);
          
        }
    }
}
