/**
  ******************************************************************************
  * @file    main.c 
  * @author  Leander Kieweg
  * @version V1.0
  * @date    15-Nov-2026
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
#define TIM3_TICK_HZ 24000U   // Counter frequency of TIM3
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

  //setup timer 3 channel 3 as PWM on PC8
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //activate clock

  //configure output
  TIM3->CCMR2 |= (6 << 4);        // PWM mode 1
  TIM3->CCMR2 |= TIM_CCMR2_OC3PE; // preload for CCR3

  //enable output compare channel 3 for pin
  TIM3->CCER |= TIM_CCER_CC3E;
}


void TIM3_SetBlink(uint32_t freq, uint32_t high, uint32_t low)
{
  if (freq == 0 || (high + low) == 0)
  {
    return;
  }

  // TIM3 clock = 48 MHz, PSC = 48 MHz / 24 kHz - 1 = 1999
  // Period in ticks = 24000 / freq

  // USER0, 2 Hz, 1:5:  Period = 12000, CCR3 = 12000 * 1 / 6 = 2000
  // USER1, 4 Hz, 1:3:  Period = 6000,  CCR3 = 6000 * 1 / 4 = 1500
  // WAKEUP, 9 Hz, 3:2: Period = 2666,  CCR3 = 2666 * 3 / 5 = 1599
  // 9 Hz is rounded down, the real frequency is 24000 / 2666 = 9.002 Hz

  uint32_t period = TIM3_TICK_HZ / freq;

  TIM3->CR1 &= ~TIM_CR1_CEN;                  // stop timer

  TIM3->PSC  = (SystemCoreClock / TIM3_TICK_HZ) - 1;
  TIM3->ARR  = period - 1;
  TIM3->CCR3 = period * high / (high + low);

  TIM3->CNT = 0;                              // reset counter
  TIM3->EGR |= TIM_EGR_UG;                    // update registers

  TIM3->CR1 |= TIM_CR1_CEN;                   // start timer
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    SysTick_Init();
    Button_Initialize();
    TIM3_InitForCompareOutput();

    uint32_t mode = 0;      // 0 = no button pressed yet

    while (1)
    {
        uint32_t buttonState = Button_GetState();

        if ((buttonState & BUTTON_USER0_MASK) && mode != 1)
        {
            mode = 1;
            TIM3_SetBlink(2, 1, 5);
        }
        else if ((buttonState & BUTTON_USER1_MASK) && mode != 2)
        {
            mode = 2;
            TIM3_SetBlink(4, 1, 3);
        }
        else if ((buttonState & BUTTON_WAKEUP_MASK) && mode != 3)
        {
            mode = 3;
            TIM3_SetBlink(9, 3, 2);
        }
    }
}