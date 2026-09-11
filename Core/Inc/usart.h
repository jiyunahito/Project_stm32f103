#ifndef USART_H
#define USART_H

#include "main.h"
#include "stdbool.h"
#include <stdint.h>

#define UART_BUF_SIZE 128U
#define CMD_SIZE 64
#define FRAME_READY 1
#define FRAME_NOT_READY 0

#ifndef DEBUG_LOG
#define TRACE(message) ((void)0)
#else
#define TRACE(message) vusartUSART_Print(message)
#endif

typedef struct{
    uint8_t tx_buffer[UART_BUF_SIZE];
    volatile uint8_t is_tx_busy;
    uint16_t tx_size;
    volatile uint16_t tx_index;
} tusartTXBuffer;

typedef struct{
    volatile uint8_t rx_buffer[UART_BUF_SIZE];
    volatile uint8_t frame_ready_flag;
    volatile uint16_t head;
    volatile uint16_t tail;
} tusartRXBuffer;

typedef enum{
    USART_CMD_NONE = 0,
    USART_CMD_LED_ON,
    USART_CMD_UNKNOWN
} eusartUSART_Cmd;

void vusartUSART_Init(void);
void vusartUSART_Print(const char *text, ...) __attribute__((format(printf, 1, 2))); //內部呼叫的API沒有型別檢查 所以強制編譯器進行型別檢查
void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len);
void vusartUSART_StartRX(void);
eusartUSART_Cmd eusartUSART_Poll(void);

#endif