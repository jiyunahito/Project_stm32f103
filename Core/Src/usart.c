#include "usart.h"
#include <stdio.h>
#include <string.h>

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

void vusartUSART_Print(const char *text){    
    if(text == NULL || tusartTX1.is_tx_busy) return;

    uint16_t size = (uint16_t)strlen(text);

    for(uint16_t i = 0; i < size; i++){
        tusartTX1.tx_buffer[i] = (uint8_t)text[i];
    }
    tusartTX1.tx_buffer[size++] = '\r';
    tusartTX1.tx_buffer[size++] = '\n';

    tusartTX1.tx_size = size;
    tusartTX1.tx_index = 0;
    tusartTX1.is_tx_busy = 1;

    LL_USART_EnableIT_TXE(USART1);
}

void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len){
    if(data == NULL || len == 0) return;    

}

void vusartUSART_StartRX(void){
    LL_USART_EnableIT_RXNE(USART1);
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
                cusartLine_Buffer[u8usartLine_Index] = (char)ch;
            }
            else{
                cusartLine_Buffer[u8usartLine_Index] = '\0';                
                tusartRX1.frame_ready_flag = FRAME_READY;
                u8usartLine_Index = 0;
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