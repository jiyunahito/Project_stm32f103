#ifndef LED_H
#define LED_H

#include <main.h>

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

#endif