#include "usart.h"
#include <stdio.h>
#include <string.h>

tusartTXBuffer tusartTX1;
tusartRXBuffer tusartRX1;

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
    tusartTX1.tx_buffer[size] = '\r';
    tusartTX1.tx_buffer[size + 1] = '\n';

    tusartTX1.tx_size = size + 2;
    tusartTX1.tx_index = 0;
    tusartTX1.is_tx_busy = 1;

    LL_USART_EnableIT_TXE(USART1);
}

void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len){
    if(data == NULL || len == 0) return;    

}

void vusartUSART_StartRX(void){
    
}

// eusartUSART_Cmd eusartUSART_Poll(void){

// }

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