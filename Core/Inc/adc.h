#ifndef ADC_H
#define ADC_H

#include "main.h"

void vadcADC1_Init(void);
void vadcADC1_Read(void);
uint16_t uadcADC1_GetVoltage(void);

#endif