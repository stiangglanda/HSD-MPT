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

  //set ARR register
  TIM3->ARR = (SystemCoreClock / 2UL / 1000UL) - 1UL;
  TIM3->PSC = 1000 - 1; //set prescaler to 1000
  TIM3->CCR3 = (SystemCoreClock / 2UL / 1000UL) -1UL;

  //configure output
  TIM3->CCMR2|= (3<<4);

  //enable output compare channel 3 for pin
  TIM3->CCER |= TIM_CCER_CC3E;

  //enable timer 3
  TIM3->CR1 |= TIM_CR1_CEN;
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

    TIM3_InitForCompareOutput();

    while (1)
    {
      SysDelay_Delay(200);
    }
}