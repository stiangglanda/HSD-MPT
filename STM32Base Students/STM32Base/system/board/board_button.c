#include "board_button.h"

void board_button_init(void) {
    // Initialize the button
    GPIO_InitTypeDef initStruct;
    uint32_t idx = 0;

    /* GPIO Ports Clock Enable */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    /* initialize the GPIO struct */
    GPIO_StructInit(&initStruct);
    
    for (idx = 0; idx < sizeof(LED_PIN) / sizeof(GPIO_PIN); idx++)
    {
	    /* disable the LEDs before activating output driver */
    	GPIO_SetBits(LED_PIN[idx].port, LED_PIN[idx].pin);

	    /* Configure GPIO pin */
    	initStruct.GPIO_Pin = LED_PIN[idx].pin;
	    initStruct.GPIO_Mode = GPIO_Mode_OUT;
    	GPIO_Init(LED_PIN[idx].port, &initStruct);
    }
}

uint32_t Button_GetState(void) {
    uint32_t retVal;
    // check if PD0 (=button0 = SW2= is pressed
    if((GPIOD->IDR & 0x01) == 0x01)
    {
        retVal |= BUTTON_USER0_MASK;
    }

    if((GPIOD->IDR & 0x02) == 0x02)
    {
        retVal |= BUTTON_USER1_MASK;
    }

    if((GPIOD->IDR & 0x02) == 0x02)
    {
        retVal |= BUTTON_USER0_MASK;
    }
}