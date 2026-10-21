/**
  ******************************************************************************
  * @file    led.c 
  * @author  Andreas Oyrer
  * @version V1.0
  * @date    30-September-2022
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "sysdelay.h"

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

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    GPIO_InitTypeDef initStruct;
 
    /* init the sys tick counter (needed for SysDelay) */
    SysTick_Init();
    
    /* enable clock for GPIO C */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    /* Configure GPIO pin for LED3 LED4 LED5 as output (connected to PC0 PC1 PC2) */
    GPIO_StructInit(&initStruct);
    initStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    initStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &initStruct);
	

		const uint16_t leds[] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2};
    const int num_leds = sizeof(leds) / sizeof(leds[0]);

    GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2); // turn LEDs off

    while (1)
    {
        /* left to right */
        for (int i = 0; i < num_leds; i++)
        {
            GPIO_ResetBits(GPIOC, leds[i]);
            SysDelay_Delay(200);
            GPIO_SetBits(GPIOC, leds[i]);
        }
      
        /* right to left avoid the outer LEDs */
        for (int i = num_leds - 2; i >= 1; i--)
        {
            GPIO_ResetBits(GPIOC, leds[i]);
            SysDelay_Delay(200);
            GPIO_SetBits(GPIOC, leds[i]);
        }
    }
}
