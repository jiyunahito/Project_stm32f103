#ifndef ADC_H
#define ADC_H

#include "main.h"
/*********************************************/
/* PA1 -> ADC1_IN1                           */
/*********************************************/
void vadcADC1_Init(void);
void vadcADC1_Read(void);
uint16_t uadcADC1_GetVoltage(void);

#endif