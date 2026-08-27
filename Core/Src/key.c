#include "key.h"
#include "main.h"

#define KEY_DEBOUNCE_MS 20U // 防彈跳延遲
#define KEY_LONG_PRESS_MS 1000U // 長按所需時間


static bool prvbkeyKey_Read(tkeyKey *key){
    return  LL_GPIO_IsInputPinSet(key->port, key->pin) == key->active_level;
}

static void prvvSetEvent(tkeyKey *key, ekeyKeyEvent event){
    if(key->event_taken){
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

void vkeyKey_Tick(tkeyKey *key){
    bool is_pressed = prvbkeyKey_Read(key);
    uint32_t new_tick = u32GetTick();

    switch (key->state)
    {
    case KEYSTATE_IDLE:
        if(is_pressed){
            key->state = KEYSTATE_DEBOUNCE_PRESSED;
            key->entry_tick = new_tick;
        }        
        break;
    case KEYSTATE_DEBOUNCE_PRESSED:
        if(!is_pressed){
            key->state = KEYSTATE_IDLE;            
        }
        else if(new_tick - key->entry_tick >= KEY_DEBOUNCE_MS){
            key->state = KEYSTATE_PRESSED;
            key->entry_tick = new_tick;
            prvvSetEvent(key, KEY_EVENT_PRESS);
        }
        break;
    case KEYSTATE_PRESSED:    
        if(!is_pressed){
            key->state = KEYSTATE_DEBOUNCE_RELEASED;
            key->entry_tick = new_tick;
        }
        else if(new_tick - key->entry_tick >= KEY_LONG_PRESS_MS){
            key->entry_tick = new_tick;
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

ekeyKeyEvent ekeyKey_GetEvent(tkeyKey *key){
    ekeyKeyEvent get_event = key->pending_event;
    key->pending_event = KEY_EVENT_NONE; // 事件拿走必須清除待辦事件
    key->event_taken = true; // 將事件拿走後 設為true表示可以新增新事件
    return get_event;
}

bool bkeyKey_IsPressed(tkeyKey *key){
    return (key->state == KEYSTATE_PRESSED || key->state == KEYSTATE_DEBOUNCE_PRESSED);
}