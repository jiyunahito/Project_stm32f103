#ifndef LED_H
#define LED_H

#include <main.h>

#define LED_EXECUTE_INTERVAL 500U // LED最少幾ms執行一次

void vledLed_On(void);
void vledLed_Off(void);
void vledLed_Toggle(void);

#endif