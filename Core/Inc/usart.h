#ifndef USART_H
#define USART_H

#include "main.h"
#include <stdint.h>

#define UART_BUF_SIZE 128U
#define FRAME_READY 1
#define FRAME_NOT_READY 0

typedef struct{
    uint8_t tx_buffer[UART_BUF_SIZE];
    volatile uint8_t is_tx_busy;
    uint16_t tx_size;
    volatile uint16_t tx_index;
} tusartTXBuffer;

typedef struct{
    uint8_t rx_buffer[UART_BUF_SIZE];
    volatile uint8_t frame_ready_flag;
    volatile uint16_t head;
    volatile uint16_t tail;
} tusartRXBuffer;

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