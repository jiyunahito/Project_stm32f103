#ifndef USART_H
#define USART_H

#include "main.h"
#include <stdint.h>

typedef enum{
    USART_CMD_NONE = 0,
    USART_CMD_LED_ON,
} eusartUSART_Cmd;

void vusartUSART_Init(void);
void vusartUSART_Print(const char *text);
void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len);
void vusartUSART_StartRX(void);
eusartUSART_Cmd eusartUSART_Poll(void);

#endif