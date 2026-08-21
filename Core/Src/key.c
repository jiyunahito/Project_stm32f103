#include "key.h"

#define KEY_ACTIVE_LEVEL 0 // active-low
#define KEY_DEBOUNCE_MS 20U // 防彈跳延遲
#define KEY_LONG_PRESS_MS 1000U // 長按所需時間

typedef enum{
    KEYSTATE_IDLE = 0,
} KeyState;

static bool prvbkeyKey_Read(void){
    return  LL_GPIO_IsInputPinSet(KEY2_GPIO_Port, KEY2_Pin) == KEY_ACTIVE_LEVEL;
}

void vkeyKey_Init(void){
    
}

void vkeyKey_Tick(void){

}

ekeyKeyEvent ekeyKey_GetEvent(void){

}

bool bkeyKey_IsPressed(void){

}