#include "spectrum.h"
#include <stdint.h>
#include <lcd.h>

// static const uint16_t uspectrumSin_Table[POINT_NUM] = {
//     2047, 2097, 2147, 2197, 2247, 2297, 2347, 2397, 2447, 2497, 2546, 2595, 2644, 2692, 2740, 2788,
//     2835, 2882, 2928, 2974, 3019, 3064, 3108, 3151, 3194, 3236, 3277, 3317, 3356, 3395, 3433, 3470,
//     3506, 3541, 3575, 3608, 3640, 3671, 3701, 3729, 3757, 3783, 3808, 3832, 3854, 3875, 3895, 3913,
//     3930, 3945, 3959, 3971, 3981, 3990, 3997, 4003, 4007, 4009, 4010, 4009, 4007, 4003, 3997, 3990,
//     3981, 3971, 3959, 3945, 3930, 3913, 3895, 3875, 3854, 3832, 3808, 3783, 3757, 3729, 3701, 3671,
//     3640, 3608, 3575, 3541, 3506, 3470, 3433, 3395, 3356, 3317, 3277, 3236, 3194, 3151, 3108, 3064,
//     3019, 2974, 2928, 2882, 2835, 2788, 2740, 2692, 2644, 2595, 2546, 2497, 2447, 2397, 2347, 2297,
//     2247, 2197, 2147, 2097, 2047, 1997, 1947, 1897, 1847, 1797, 1747, 1697, 1647, 1597, 1548, 1499,
//     1450, 1402, 1354, 1306, 1259, 1212, 1166, 1120, 1075, 1030,  986,  943,  900,  858,  817,  777,
//      738,  699,  661,  624,  588,  553,  519,  486,  454,  423,  393,  365,  337,  311,  286,  262,
//      240,  219,  199,  181,  164,  149,  135,  123,  113,  104,   97,   91,   86,   84,   84,   86,
//       91,   97,  104,  113,  123,  135,  149,  164,  181,  199,  219,  240,  262,  286,  311,  337,
//      365,  393,  423,  454,  486,  519,  553,  588,  624,  661,  699,  738,  777,  817,  858,  900,
//      943,  986, 1030, 1075, 1120, 1166, 1212, 1259, 1306, 1354, 1402, 1450, 1499, 1548, 1597, 1647,
//     1697, 1747, 1797, 1847, 1897, 1947, 1997, 2047
// };

const uint16_t uspectrumSin_Table[3] = {2047, 10, 4000};

void vspectrumDrawGrid(void){
    vlcdLCD_Clear(BACKGROUND_COLOR);

    vlcdLCD_Draw_Rectangle(X_FRAME_MIN, X_FRAME_MAX, Y_FRAME_MIN, Y_FRAME_MAX, WHITE);

    for(uint16_t y = Y_FRAME_MIN + 50; y < Y_FRAME_MAX; y += 50){
        for(uint16_t x = X_FRAME_MIN + 1; x < X_FRAME_MAX; x += 4){
            vlcdLCD_Draw_Point(x, y, GRAY);
        }
    }
    for(uint16_t x = X_FRAME_MIN + 25; x < X_FRAME_MAX; x += 25){
        uint16_t color = (x == X_FRAME_CENTER) ? GREEN : GRAY;        
        for (uint16_t y = Y_FRAME_MIN + 1; y < Y_FRAME_MAX; y += 4){
            vlcdLCD_Draw_Point(x, y, color);
        }
    }
}

void vspectrumDAC_DMA_Init(void){
    LL_DAC_EnableDMAReq(DAC, LL_DAC_CHANNEL_1);
    LL_DMA_ConfigAddresses(DMA2, LL_DMA_CHANNEL_3, 
                            (uint32_t)uspectrumSin_Table, LL_DAC_DMA_GetRegAddr(DAC, LL_DAC_CHANNEL_1, LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED), 
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_3);
    LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_3, 3);
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_3);
}

void vspectrumFrequency_Change(uint32_t PSC, uint32_t ARR){ // if ARR = 0, 訊號頻率(Hz) = 8M / (7+1) / 256 = 3906.25Hz 
    // 訊號頻率(Hz) = APB1 Timer(Hz) / (PSC + 1) / (ARR + 1) / N
    // N : 產生完整的訊號總共需要幾個樣本點
    // 利用 TIM2 當作觸發源    
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetAutoReload(TIM2, ARR);
    LL_TIM_EnableCounter(TIM2);
}

void vspectrumUpdate_Waveform(const uint16_t *wavedata){ //*wavedata : ADC轉換後的資料經由DMA傳送到一個array
    static uint16_t pre_x_points[POINT_NUM];
    for(uint16_t i = 0; i < 255; i++){
        uint16_t y0 = Y_FRAME_MIN + i;
        uint16_t x0 = pre_x_points[i];
        uint16_t y1 = Y_FRAME_MIN + i + 1;
        uint16_t x1 = pre_x_points[i + 1];

        vlcdLCD_Draw_Line(x0, y0, x1, y1, BACKGROUND_COLOR);

        if(x0 == X_FRAME_CENTER || x1 == X_FRAME_CENTER){
            vlcdLCD_Draw_Point(X_FRAME_CENTER, y0, GREEN);
        }        
    }

    for(uint16_t i = 0; i < 256; i++){
        int32_t diff = (int32_t)wavedata[i] - 2048;

        int16_t x_offset = (diff * 90) >> 11;
        uint16_t curr_x = X_FRAME_CENTER + x_offset;

        if(curr_x <= X_FRAME_MIN) curr_x = X_FRAME_MIN + 1;
        if(curr_x >= X_FRAME_MAX) curr_x = X_FRAME_MAX - 1;

        pre_x_points[i] = curr_x;
    }

    for(uint16_t i = 0; i < 255; i++){
        uint16_t y0 = Y_FRAME_MIN + i;
        uint16_t x0 = pre_x_points[i];
        uint16_t y1 = Y_FRAME_MIN + i + 1;
        uint16_t x1 = pre_x_points[i + 1];

        vlcdLCD_Draw_Line(x0, y0, x1, y1, YELLOW);
    }
}