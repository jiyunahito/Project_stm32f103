#ifndef USART_H
#define USART_H

#include "main.h"
#include "stdbool.h"
#include <stdint.h>

/****************************************/
/* PA9 -> USART1_TX, PA10 -> USART1_RX  */
/* USART1_TX <=> DMA1_CH4               */
/* USART1_RX <=> DMA1_CH5               */
/****************************************/
#define UART_BUF_SIZE 128U
#define DMA_BUF_SIZE 128U // 用於DMA的buffer size
#define CMD_SIZE 64 // 串口指令的大小
#define FRAME_READY 1
#define FRAME_NOT_READY 0

#ifndef DEBUG_LOG
#define TRACE(message) ((void)0)
#else
#define TRACE(message) vusartUSART_Print(message)
#endif

typedef struct{
    uint8_t tx_buffer[UART_BUF_SIZE]; // 存放要傳輸的指令或資料
    volatile uint8_t is_tx_busy; // 確認TX是否在動作 避免傳輸途中tx_buffer被overwrite
    uint16_t tx_size; // 用於紀錄要傳輸的指令有多少個Byte(已包括'\r', '\n' 請查vusartUSART_Print)
    volatile uint16_t tx_index; // 用於中斷(原本用於中斷 但改為DMA後 此變數沒使用)
} tusartTXBuffer;

typedef struct{
    volatile uint8_t rx_buffer[UART_BUF_SIZE];
    union {
        volatile uint8_t frame_ready_flag;
        volatile uint8_t overrun_count;
    };        
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

void vusartUSART_TX_By_DMA_Init(void);
void vusartUSART_RX_By_DMA_Init(void);
// void busartRingBuffer_Write(tusartRXBuffer *buffer, const uint8_t *data, uint16_t len);
void vusartProcess_DMABuffer(void);

#endif