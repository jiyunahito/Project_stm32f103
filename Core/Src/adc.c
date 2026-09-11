#include "adc.h"

volatile uint16_t uadcADC1_Value = 0U;
float fadcvoltage = 0.0f;

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

float fadcADC1_GetVoltage(void){
    if(LL_ADC_IsActiveFlag_EOS(ADC1)){
        fadcvoltage = ((float)uadcADC1_Value / 4095.0f) * 3.3f;
        LL_ADC_ClearFlag_EOS(ADC1);
    }
    return fadcvoltage;    
}