#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

tusartTXBuffer tusartTX1;
tusartRXBuffer tusartRX1;
static char cusartLine_Buffer[CMD_SIZE]; // 分析指令用的buffer
static uint8_t u8usartLine_Index = 0; // 分析指令用buffer的index

static inline bool busartRingBuffer_Pop(tusartRXBuffer *buffer, uint8_t *ch){
    if(buffer->tail != buffer->head){
        *ch = buffer->rx_buffer[buffer->tail++];
        return true;
    }
    else{
        return false;
    }
}

static eusartUSART_Cmd eusartParse_Command(const char *line){
    if(strcmp(line, "led on") == 0){
        return USART_CMD_LED_ON;
    }
    else{
        return USART_CMD_UNKNOWN;
    }
}

void vusartUSART_Init(void){
    tusartTX1.tx_size = 0;
    tusartTX1.tx_index = 0;
    tusartTX1.is_tx_busy = 0;
    tusartRX1.head = 0;
    tusartRX1.tail = 0;
    tusartRX1.frame_ready_flag = FRAME_NOT_READY;
}

void vusartUSART_Print(const char *text, ...){    
    if(text == NULL || tusartTX1.is_tx_busy) return;

    va_list args;
    va_start(args, text);

    int len = vsniprintf((char *)tusartTX1.tx_buffer, sizeof(tusartTX1.tx_buffer) - 2, text, args);
    va_end(args);

    if(len < 0) return;
    
    uint16_t size = (uint16_t)len;

    for(uint16_t i = 0; i < size; i++){
        tusartTX1.tx_buffer[i] = (uint8_t)text[i];
    }
    tusartTX1.tx_buffer[size++] = '\r'; // 補上回開頭
    tusartTX1.tx_buffer[size++] = '\n'; // 補上跳行

    tusartTX1.tx_size = size;
    tusartTX1.tx_index = 0;
    tusartTX1.is_tx_busy = 1; 

    LL_USART_EnableIT_TXE(USART1); // 用DMA版本 這行必須關掉
    /* 以下為DMA版本 */
    // LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4); // 為了修改DMA_CNDTRx 必須關閉
    // LL_USART_EnableIT_TC(USART1);

    // LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, tusartTX1.tx_buffer, \
    //                     LL_USART_DMA_GetRegAddr(USART1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH); // 也許可以放在初始化裡
    // LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, (uint32_t)tusartTX1.tx_size);
    
    // LL_USART_EnableDMAReq_TX(USART1); // 可放在初始化
    // LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4); // 
}

void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len){
    if(data == NULL || len == 0) return;    

}

void vusartUSART_StartRX(void){
    LL_USART_EnableIT_RXNE(USART1);
    // LL_USART_EnableDirectionRx(USART1);
}

eusartUSART_Cmd eusartUSART_Poll(void){    
    uint8_t ch;
    
    while(busartRingBuffer_Pop(&tusartRX1, &ch)){
        if( ch == '\r' || ch == '\n'){
            if(u8usartLine_Index > 0){
                cusartLine_Buffer[u8usartLine_Index] = '\0';                
                tusartRX1.frame_ready_flag = FRAME_READY;
                u8usartLine_Index = 0;
                break;
            }
        }
        else{
            if(u8usartLine_Index < (CMD_SIZE - 1)){
                cusartLine_Buffer[u8usartLine_Index++] = (char)ch;
            }
            else{
                cusartLine_Buffer[u8usartLine_Index] = '\0';                
                tusartRX1.frame_ready_flag = FRAME_READY;
                u8usartLine_Index = 0;
                // 可以針對 line_buffer滿的情形 給一個CMD 例如:LINE_BUFFER_FULL, 可以區分是解到不認識的指令還是buffer full
                break;
            }
        }
    }
    if(tusartRX1.frame_ready_flag == FRAME_READY){
        tusartRX1.frame_ready_flag = FRAME_NOT_READY;
        return eusartParse_Command(cusartLine_Buffer);
    }
    else{
        return USART_CMD_NONE;
    }
}



// int _write(int file, char *ptr, int len){ // 不是走usart中斷 不需要enableIT_TXE
//     for(int i = 0; i < len; i++){
//         if(ptr[i] == '\n'){
//             while(!LL_USART_IsActiveFlag_TXE(USART1));
//             LL_USART_TransmitData8(USART1, '\r');
//         }
//     }
    
//     while(!LL_USART_IsActiveFlag_TXE(USART1));
//     LL_USART_TransmitData8(USART1, '\n');
// }