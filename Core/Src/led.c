#include "led.h"

tledLED LED0 = { .port = LED0_GPIO_Port, .pin = LED0_Pin}; // GPIO 此腳位支援PWM 若改用PWM 此宣告可以不必理會
tledLED LED1 = { .port = LED1_GPIO_Port, .pin = LED1_Pin}; // GPIO

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

void vledLed_PWM_CCR_Change(uint8_t duty_cycle_ratio){ // 輸入0~100
    duty_cycle_ratio = (duty_cycle_ratio > 100) ? 100 : duty_cycle_ratio;
    uint32_t value_of_CCR = 10U * duty_cycle_ratio; // ARR = 999 總共1000 1% = 10

    LL_TIM_OC_SetCompareCH2(TIM3, value_of_CCR);
}