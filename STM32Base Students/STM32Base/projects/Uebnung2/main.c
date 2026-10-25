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
#include "board_led.h"

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
    uint32_t counter = 0;

    /* Initialize the timer, buttons, and LEDs */
    SysTick_Init();
    Button_Initialize();
    LED_Initialize();

    while (1)
    {
        uint32_t buttonState = Button_GetState();

        /* USER0 counts up */
        if (buttonState & BUTTON_USER0_MASK)
        {
            if (counter >= 7)
            {
                /* After 7 wrap around to 0 */
                counter = 0;
            }
            else
            {
                counter++;
            }
        } /* USER1 counts down */
        else if (buttonState & BUTTON_USER1_MASK)
        {
            if (counter == 0)
            {
                /* Before 0 wrap around to 7 */
                counter = 7;
            }
            else
            {
                counter--;
            }
        }

        /* Show the counter value on the three LEDs */
        LED_SetOut(counter);

        /* Wait 200 ms */
        SysDelay_Delay(200);
    }
}
