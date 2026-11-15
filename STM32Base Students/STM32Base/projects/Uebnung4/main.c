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

void TIM3_InitForCompareOutput(void)
{
  //configure PC8 as AF mode
  GPIO_InitTypeDef initStruct;

  // enable clock for PC8
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

  //configure PC8 as AF
  GPIO_StructInit(&initStruct);
  initStruct.GPIO_Pin = GPIO_Pin_8;
  initStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOC, &initStruct);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_0);

  //setup timer 3 channel 3 to toggle PC8 mit 1Hz
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //activate clock

  //configure output
  TIM3->CCMR2|= (3<<4);

  //enable output compare channel 3 for pin
  TIM3->CCER |= TIM_CCER_CC3E;
}


void TIM3_ChangeFrequency(uint32_t frequency)
{
  if (frequency == 0)
  {
    return;
  }

  TIM3->CR1 &= ~TIM_CR1_CEN; //disable timer

  // 9(Hz) * 2(toggle) = 18
  // 1/18 = 0.055555556s

  // 24.000.000 Hz / ((1000 - 1) +1) = 24.000 Hz

  // 24000 * 0.055555556s = 1333.33333

  // 1333 / 2 = 666

  uint32_t arr = 24000 / (frequency * 2);

  TIM3->PSC = 1000 - 1;
  TIM3->ARR = arr - 1;
  TIM3->CCR3 = arr / 2;

  TIM3->CNT = 0; //reset counter

  TIM3->EGR |= TIM_EGR_UG; //update registers

  TIM3->CR1 |= TIM_CR1_CEN; //enable timer
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

    SysTick_Init();
    Systemconfig_Clock();
    Button_Initialize();
    TIM3_InitForCompareOutput();

    while (1)
    {
      uint32_t buttonState = Button_GetState();

      if (buttonState & BUTTON_USER0_MASK)
      {
        TIM3_ChangeFrequency(2);
      }
      else if (buttonState & BUTTON_USER1_MASK)
      {
        TIM3_ChangeFrequency(4);
      }
      else if (buttonState & BUTTON_WAKEUP_MASK)
      {
        TIM3_ChangeFrequency(9);
      }
    }
}