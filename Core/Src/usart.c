#include "usart.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
USART_TypeDef
void vusartUSART_Init(void){

}

void vusartUSART_Print(const char *text){
    if(text == NULL) return;
    LL_USART_TransmitData8()
    HAL_UART_Transmit(&huart1, (uint8_t *)text, (uint16_t)strlen(text), HAL_MAX_DELAY);
}

void vusartUSART_PrintBytes(const uint8_t *data, uint16_t len){
    if(data == NULL || len == 0) return;
    HAL_UART_Transmit(&huart1, (uint8_t *)data, len, HAL_MAX_DELAY);
}

void vusartUSART_StartRX(void){
    
}

eusartUSART_Cmd eusartUSART_Poll(void){

}

int fputc(int ch, FILE *f){
    uint8_t data = (uint8_t)ch;
    
    if(ch == '\n'){
        uint8_t cr = 'r';
        HAL_UART_Transmit(&huart1, &cr, 1U, HAL_MAX_DELAY);
    }
    HAL_UART_Transmit(&huart1, &data, 1U, HAL_MAX_DELAY);

    return ch;
}