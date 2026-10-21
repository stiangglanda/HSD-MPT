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
#include "board_button.h"

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

    Button_Initialise();
  

    while (1)
    {
      uint32_t res = Button_GetState();

      if(res & BUTTON_USER0_MASK)
      {
        //increment/decrement counter
      }

      //wait 200ms
      SysDelay_Delay(200);
    }
}
