#include "key.h"
#include "main.h"

#define KEY_DEBOUNCE_MS 20U // 防彈跳延遲
#define KEY_LONG_PRESS_MS 1000U // 長按所需時間

tkeyKey tKey0;
tkeyKey tKey1;
tkeyKey tKey2;

static bool prvbkeyKey_Read(tkeyKey *key){
    return  LL_GPIO_IsInputPinSet(key->port, key->pin) == key->active_level; // key被按下時回傳true 否則回傳false
}

static void prvvSetEvent(tkeyKey *key, ekeyKeyEvent event){
    if(key->event_taken){ // 事件被拿走 才寫入新事件 (防止舊事件被覆蓋)
        key->pending_event = event;
        key->event_taken = false;
    }
}

void vkeyKey_Init(GPIO_TypeDef *port, uint16_t pin, uint16_t active_level, tkeyKey *key){
    key->port = port;
    key->pin = pin;
    key->active_level = active_level;
    key->state = prvbkeyKey_Read(key) ? KEYSTATE_PRESSED : KEYSTATE_IDLE;
    key->entry_tick = u32GetTick();
    key->event_taken = true;
    key->pending_event = KEY_EVENT_NONE;
}

void vkeyKey_Tick(tkeyKey *key){ // key state machine
    bool is_pressed = prvbkeyKey_Read(key); // ture : key有被按下 false : key沒被按下 
    uint32_t new_tick = u32GetTick(); // 獲取當前tick紀錄執行時間

    switch (key->state) 
    {
    case KEYSTATE_IDLE: // 空閒
        if(is_pressed){
            key->state = KEYSTATE_DEBOUNCE_PRESSED;
            key->entry_tick = new_tick;
        }        
        break;
    case KEYSTATE_DEBOUNCE_PRESSED: // 疑似被按下(防彈跳處理)
        if(!is_pressed){
            key->state = KEYSTATE_IDLE;            
        }
        else if(new_tick - key->entry_tick >= KEY_DEBOUNCE_MS){ // 超過時間則認定為按下
            key->state = KEYSTATE_PRESSED;
            key->entry_tick = new_tick; // 為了處理長按而記錄時間
            prvvSetEvent(key, KEY_EVENT_PRESS);
        }
        break;
    case KEYSTATE_PRESSED: // 按下
        if(!is_pressed){
            key->state = KEYSTATE_DEBOUNCE_RELEASED;
            key->entry_tick = new_tick;
        }
        else if(new_tick - key->entry_tick >= KEY_LONG_PRESS_MS){ // 超過時間則認定為長按
            key->entry_tick = new_tick; // 防止長按一直被觸發--確認為長按後 必須再經過一次KEY_LONG_PRESS_MS才會再發送下一次的長按事件
            prvvSetEvent(key, KEY_EVENT_LONG_PRESS);
        }        
        break;
    case KEYSTATE_DEBOUNCE_RELEASED:
        if(is_pressed){
            key->state = KEYSTATE_PRESSED;
        }
        else if(new_tick - key->entry_tick >= KEY_DEBOUNCE_MS){
            key->state = KEYSTATE_IDLE;
            prvvSetEvent(key, KEY_EVENT_RELEASE);
        }
        break;
    default:
        break;
    }
}

ekeyKeyEvent ekeyKey_GetEvent(tkeyKey *key){ // 若要不丟棄事件 要進行queue將事件放入排程
    ekeyKeyEvent get_event = key->pending_event;
    key->pending_event = KEY_EVENT_NONE; // 事件拿走必須清除待辦事件
    key->event_taken = true; // 將事件拿走後 設為true表示可以新增新事件
    return get_event;
}

bool bkeyKey_IsPressed(tkeyKey *key){
    return (key->state == KEYSTATE_PRESSED || key->state == KEYSTATE_DEBOUNCE_PRESSED);
}