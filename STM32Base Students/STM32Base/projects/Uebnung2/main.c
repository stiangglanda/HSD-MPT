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

        if (buttonState & BUTTON_USER0_MASK)
        {
          /* Count up, 7 + 1 wraps to 0 */
          counter = (counter + 1) & 0x07;

          /* Show the counter value on the three LEDs */
          LED_SetOut(counter);
          /* Wait 200 ms */
          SysDelay_Delay(200);
        }
        else if (buttonState & BUTTON_USER1_MASK)
        {
          /* Count down, 0 - 1 wraps to 7 */
          counter = (counter - 1) & 0x07;

          /* Show the counter value on the three LEDs */
          LED_SetOut(counter);
          /* Wait 200 ms */
          SysDelay_Delay(200);
        }
    }
}
