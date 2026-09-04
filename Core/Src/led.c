#include "led.h"

tledLED LED0 = { .port = LED0_GPIO_Port, .pin = LED0_Pin};
tledLED LED1 = { .port = LED1_GPIO_Port, .pin = LED1_Pin};

/**
 * @file led.c
 * @note active-low LEDs
 */

void vledLed_On(tledLED *led){
    LL_GPIO_ResetOutputPin(led->port, led->pin);
}

void vledLed_Off(tledLED *led){
    LL_GPIO_SetOutputPin(led->port, led->pin);
}

void vledLed_Toggle(tledLED *led){
    LL_GPIO_TogglePin(led->port, led->pin);
}