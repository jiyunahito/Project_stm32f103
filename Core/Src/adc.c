#include "adc.h"

volatile uint16_t uadcADC1_Value = 0; // 儲存轉換後的digital value
volatile uint8_t uadcADC1_CanGetFlag = 0; // 轉換完成 中斷產生後立起此flag 標記有新的值可以拿
// float fadcvoltage = 0.0f;
static uint16_t uadcVoltage_mV = 0; // 將digital value轉換成實際電壓
// uint16_t uadcADC_DMA_Buffer[512] = {0};

void vadcADC1_Init(void){
    /* 使用手冊提到(如下)
    Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for
    at least two ADC clock cycles */
    LL_ADC_Enable(ADC1);
    LL_mDelay(1);    
    LL_ADC_StartCalibration(ADC1);
    while(LL_ADC_IsCalibrationOnGoing(ADC1));
    /* 因此有了以上操作 */
    LL_ADC_EnableIT_EOS(ADC1); // 開啟轉換完成產生中斷(regular group)
}

void vadcADC1_Read(void){
    LL_ADC_REG_StartConversionSWStart(ADC1); // 軟體觸發conversion
}

uint16_t uadcADC1_GetVoltage(void){
    if(uadcADC1_CanGetFlag){
        //fadcvoltage = ((float)uadcADC1_Value / 4095.0f) * 3.3f; // 將ADC轉換為實際電壓(以下是效能優化版本)
        /**
         * 先放大65536(3.3/4095*65536 約等於52.812) 避免小數再放大1000倍(所以單位從V變為mV)
         * 再將ADC * 52812U 後 右移16 即得實際電壓值(mV)
         */        
        uadcVoltage_mV = (uint16_t)((uadcADC1_Value * 52812U) >> 16);
        // uadcVoltage_mV = (uint16_t)(((uadcADC1_Value * 52812U) + 32768U) >> 16); // 避免向下取整 可加上位移量的一半達到四捨五入
        uadcADC1_CanGetFlag = 0; // 執行完事件後 要把事件清除
    }
    // return uadcVoltage_mV;
    return uadcADC1_Value;
}

void vadcADC_DMA_Init(void){ // 暫停...
    LL_ADC_Disable(ADC1);    

    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    // LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, 
    //                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), uadcADC_DMA_Buffer, 
    //                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);    
    LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_3, 3);
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_3);

    LL_DAC_Enable(DAC, LL_DAC_CHANNEL_1);
}