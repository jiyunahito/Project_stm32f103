#include "key.h"



static bool prvbkeyKey_Read(void){
    return  LL_GPIO_IsInputPinSet(KEY2_GPIO_Port, KEY2_Pin) == 0;
}

void vkeyKey_Init(void){
    
}

void vkeyKey_Tick(void){

}

ekeyKeyEvent ekeyKey_GetEvent(void){

}

bool bkeyKey_IsPressed(void){

}