#ifndef LED_H
#define LED_H

#include <main.h>

/********************************************/
/* PB5 => LED0, PE5 => LED1                 */
/* PB5 -> TIM3_CH2(PWM)                     */
/********************************************/
#define LED_EXECUTE_INTERVAL 500U // LED最少幾ms執行一次

typedef struct{
    GPIO_TypeDef *port;
    uint16_t pin;
} tledLED;

extern tledLED LED0;
extern tledLED LED1;

void vledLed_On(tledLED *led);
void vledLed_Off(tledLED *led);
void vledLed_Toggle(tledLED *led);
void vledLed_PWM_CCR_Change(uint8_t duty_cycle_ratio);

#endif