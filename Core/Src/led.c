#include "led.h"

/**
 * @file led.c
 * @note active-low LEDs
 */

void vledLed_On(void){
    LL_GPIO_ResetOutputPin(LED0_GPIO_Port, LED0_Pin);
}

void vledLed_Off(void){
    LL_GPIO_SetOutputPin(LED0_GPIO_Port, LED0_Pin);
}

void vledLed_Toggle(void){
    LL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
}