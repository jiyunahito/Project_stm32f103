#include "adc.h"

volatile uint16_t uadcADC1_Value = 0U;
volatile uint8_t uadcADC1_CanGetFlag = 0;
// float fadcvoltage = 0.0f;
static uint16_t uadcVoltage_mV = 0;

void vadcADC1_Init(void){
    LL_ADC_Enable(ADC1);
    LL_mDelay(1);
    
    LL_ADC_StartCalibration(ADC1);
    while(LL_ADC_IsCalibrationOnGoing(ADC1));

    LL_ADC_EnableIT_EOS(ADC1);
}

void vadcADC1_Read(void){
    LL_ADC_REG_StartConversionSWStart(ADC1);
}

uint16_t uadcADC1_GetVoltage(void){
    if(uadcADC1_CanGetFlag){
        //fadcvoltage = ((float)uadcADC1_Value / 4095.0f) * 3.3f; // 將ADC轉換為實際電壓(以下是效能優化版本)
        /**
         * 先放大65536(3.3*4095*65536 約等於52.812) 避免小數再放大1000倍(所以單位從V變為mV)
         * 再將ADC * 52812U 後 右移16 即得實際電壓值(mV)
         */        
        uadcVoltage_mV = (uint16_t)((uadcADC1_Value * 52812U) >> 16);
        // uadcVoltage_mV = (uint16_t)(((uadcADC1_Value * 52812U) + 32768U) >> 16); // 避免向下取整 可加上位移量的一半達到四捨五入
        uadcADC1_CanGetFlag = 0; // 執行完事件後 要把事件清除
    }
    return uadcVoltage_mV;    
}