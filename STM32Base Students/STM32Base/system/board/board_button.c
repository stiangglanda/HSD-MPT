#include "board_button.h"
#include "stm32f0xx_gpio.h"

void Button_Initialize(void)
{
    GPIO_InitTypeDef initStruct;

    /* GPIO Ports Clock Enable */
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;

    /* initialize the GPIO struct with default values */
    GPIO_StructInit(&initStruct);
    /* use pin 0 and pin 1 of port D as inputs with pull-down */
    initStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    initStruct.GPIO_Mode = GPIO_Mode_IN;
    initStruct.GPIO_PuPd = GPIO_PuPd_DOWN;

    /* write the settings into the GPIO registers */
    GPIO_Init(GPIOD, &initStruct);
}

uint32_t Button_GetState(void)
{
    uint32_t retVal = 0;

    /* check if PD0 is pressed */
    if ((GPIOD->IDR & GPIO_Pin_0) != 0)
    {
        retVal |= BUTTON_USER0_MASK;
    }

    /* check if PD1 is pressed */
    if ((GPIOD->IDR & GPIO_Pin_1) != 0)
    {
        retVal |= BUTTON_USER1_MASK;
    }

    return retVal;
}