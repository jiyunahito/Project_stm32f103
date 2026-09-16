#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

tusartTXBuffer tusartTX1;
tusartRXBuffer tusartRX1;
static char cusartLine_Buffer[CMD_SIZE]; // 分析指令用的buffer
static uint8_t u8usartLine_Index = 0; // 分析指令用buffer的index
volatile uint16_t uusartDMABuffer_pos = 0;
uint8_t u8usartDMABuffer[DMA_BUF_SIZE];

/******************************************************************************/
/* UART RX by DMA 流程                                                        */
/* 當RXNE flag set, DMA請求發生.                                               */
/* 接著把資料從DR移到u8usartDMABuffer[DMA_BUF_SIZE]                            */
/* 當HT TC IDLE發生 才會把u8usartDMABuffer[DMA_BUF_SIZE]的資料移到tusartRX1     */
/******************************************************************************/

static inline bool busartRingBuffer_Pop(tusartRXBuffer *buffer, uint8_t *ch){
    if(buffer->tail != buffer->head){
        *ch = buffer->rx_buffer[buffer->tail++];
        return true;
    }
    else{
        return false;
    }
}

inline static bool busartRingBuffer_Overflow_Check(tusartRXBuffer *buffer, uint16_t len){ // 檢查要移到tusartRX的資料 是否會覆寫
    uint16_t available_space = 0; // 表示有多少位置可以寫入(下面實作方法會有一個Byte的空間使用不到)
    if(buffer->tail - buffer->head){
        available_space = buffer->tail - buffer->head - 1; // 若不能確保head = tail 資料為空時 一定要減1(表示一定會浪費一個Byte的空間)
    }
    else{
        // available_space = DMA_BUF_SIZE - 1 - (buffer->head - buffer->tail); // 同下行
        available_space = DMA_BUF_SIZE - 1 + buffer->tail - buffer->head;
    }
    // uint16_t available_space = (buffer->tail - buffer->head - 1 + DMA_BUF_SIZE) & (DMA_BUF_SIZE - 1); // 同上if-else

    return (available_space < len); // 若buffer可用空間小於len 回傳true
}

inline static void busartRingBuffer_Write(tusartRXBuffer *buffer, const uint8_t *data, uint16_t len){ // 將DMA Buffer的資料複製到tusartRX
    if(busartRingBuffer_Overflow_Check(buffer, len) == false){        
        for(uint16_t i = 0; i < len; i++){
            buffer->rx_buffer[buffer->head] = data[i];

            uint16_t next_head = buffer->head + 1;
            if(next_head >= UART_BUF_SIZE){ // 超過buffer size就回原點
                next_head = 0;
            }

            buffer->head = next_head;
        }
    }
    else{
        // 會覆寫? 要如何處理則寫在這...
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

    // for(uint16_t i = 0; i < size; i++){
    //     tusartTX1.tx_buffer[i] = (uint8_t)text[i];
    // }
    tusartTX1.tx_buffer[size++] = '\r'; // 補上回開頭
    tusartTX1.tx_buffer[size++] = '\n'; // 補上跳行

    tusartTX1.tx_size = size;
    tusartTX1.tx_index = 0;
    tusartTX1.is_tx_busy = 1; 

    // LL_USART_EnableIT_TXE(USART1); // 用DMA版本 這行必須關掉(使用DMA就不依賴USART的中斷)
    /* 以下為DMA版本 */
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4); // 為了修改DMA_CNDTRx 必須關閉
    // LL_USART_EnableIT_TC(USART1); // This bit is set by hardware if the transmission of a frame containing data is complete and if
    //                                  TXE is set (TXE沒開時 這行應該沒作用)

    // LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)tusartTX1.tx_buffer, 
                        // LL_USART_DMA_GetRegAddr(USART1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH); // 也許可以放在初始化裡
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, (uint32_t)tusartTX1.tx_size);
    
    // LL_USART_EnableDMAReq_TX(USART1); // 可放在初始化
    // LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4); // 可放在初始化?
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4); // 
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

void vusartUSART_TX_By_DMA_Init(void){
    LL_USART_EnableDMAReq_TX(USART1); // 可放在初始化
    
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, 
                            (uint32_t)tusartTX1.tx_buffer, LL_USART_DMA_GetRegAddr(USART1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH); // 也許可以放在初始化裡    
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4); // 可放在初始化?
}

void vusartUSART_RX_By_DMA_Init(void){    
    LL_USART_EnableIT_IDLE(USART1);
    LL_USART_EnableDMAReq_RX(USART1);

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, LL_USART_DMA_GetRegAddr(USART1), (uint32_t)u8usartDMABuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, DMA_BUF_SIZE);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
}

void vusartProcess_DMABuffer(void){
    uint16_t current_pos = DMA_BUF_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5); // DMA Buffer現在準備要從哪開始的index
    // 相等的情形 只有len = 0 或者 len = DMA_BUF_SIZE的情境... 但中斷觸發機制 len 最多只有 DMA_BUF_SIZE / 2
    // 或是 IDLE和HT 或 IDLE和TC中斷一起觸發 後觸發的不會因為相等 而在浪費時間執行
    if(current_pos != uusartDMABuffer_pos){
        if(current_pos > uusartDMABuffer_pos){ // 會繞回buffer開頭嗎? 不會繞回
            uint16_t len = current_pos - uusartDMABuffer_pos;
            busartRingBuffer_Write(&tusartRX1, &u8usartDMABuffer[uusartDMABuffer_pos], len);
        }
        else{ //  會繞回
            uint16_t len1 = DMA_BUF_SIZE - uusartDMABuffer_pos;
            busartRingBuffer_Write(&tusartRX1, &u8usartDMABuffer[uusartDMABuffer_pos], len1); // 把不會繞回的部分複製完
            if(current_pos > 0){
                busartRingBuffer_Write(&tusartRX1, &u8usartDMABuffer[0], current_pos); // 再把繞回的部分複製完
            }
        }
        uusartDMABuffer_pos = current_pos;
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