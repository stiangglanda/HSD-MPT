#include "board_button.h"
#include "stm32f0xx_gpio.h"

void Button_Initialize(void)
{
    GPIO_InitTypeDef initStruct;

    /* GPIO Ports Clock Enable */
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;

    GPIO_StructInit(&initStruct);
    initStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    initStruct.GPIO_Mode = GPIO_Mode_IN;
    initStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &initStruct);
}

uint32_t Button_GetState(void)
{
    uint32_t retVal = 0;

    /* check if PD0 (=button0 = SW2= is pressed */
    if ((GPIOD->IDR & GPIO_Pin_0) != 0)
    {
        retVal |= BUTTON_USER0_MASK;
    }

    if ((GPIOD->IDR & GPIO_Pin_1) != 0)
    {
        retVal |= BUTTON_USER1_MASK;
    }

    return retVal;
}