#include "key.h"
#include "main.h"

#define KEY_ACTIVE_LEVEL 0 // active-low
#define KEY_DEBOUNCE_MS 20U // 防彈跳延遲
#define KEY_LONG_PRESS_MS 1000U // 長按所需時間

typedef enum{
    KEYSTATE_IDLE = 0,
    KEYSTATE_DEBOUNCE_PRESSED,
    KEYSTATE_PRESSED,
    KEYSTATE_DEBOUNCE_RELEASED
} ekeyKeyState;

static ekeyKeyState t_state = KEYSTATE_IDLE;
static uint32_t t_entry_tick = 0;
static bool t_event_taken = true; // 是否可以處理新事件
static ekeyKeyEvent t_pending_event = KEY_EVENT_NONE;

static bool prvbkeyKey_Read(void){
    return  LL_GPIO_IsInputPinSet(KEY2_GPIO_Port, KEY2_Pin) == KEY_ACTIVE_LEVEL;
}

static void prvvSetEvent(ekeyKeyEvent event){
    if(t_event_taken){
        t_pending_event = event;
        t_event_taken = false;
    }
}

void vkeyKey_Init(void){
    t_state = prvbkeyKey_Read() ? KEYSTATE_PRESSED : KEYSTATE_IDLE;
    t_entry_tick = u32GetTick();
    t_event_taken = true;
    t_pending_event = KEY_EVENT_NONE;
}

void vkeyKey_Tick(void){
    bool is_pressed = prvbkeyKey_Read();
    uint32_t new_tick = u32GetTick();

    switch (t_state)
    {
    case KEYSTATE_IDLE:
        if(is_pressed){
            t_state = KEYSTATE_DEBOUNCE_PRESSED;
            t_entry_tick = new_tick;
        }        
        break;
    case KEYSTATE_DEBOUNCE_PRESSED:
        if(!is_pressed){
            t_state = KEYSTATE_IDLE;            
        }
        else if(new_tick - t_entry_tick >= KEY_DEBOUNCE_MS){
            t_state = KEYSTATE_PRESSED;
            t_entry_tick = new_tick;
            prvvSetEvent(KEY_EVENT_PRESS);
        }
        break;
    case KEYSTATE_PRESSED:    
        if(!is_pressed){
            t_state = KEYSTATE_DEBOUNCE_RELEASED;
            t_entry_tick = new_tick;
        }
        else if(new_tick - t_entry_tick >= KEY_LONG_PRESS_MS){
            t_entry_tick = new_tick;
            prvvSetEvent(KEY_EVENT_LONG_PRESS);
        }
        break;
    case KEYSTATE_DEBOUNCE_RELEASED:
        if(is_pressed){
            t_state = KEYSTATE_PRESSED;
        }
        else if(new_tick - t_entry_tick >= KEY_DEBOUNCE_MS){
            t_state = KEYSTATE_IDLE;
            prvvSetEvent(KEY_EVENT_RELEASE);
        }
        break;
    default:
        break;
    }
}

ekeyKeyEvent ekeyKey_GetEvent(void){
    t_event_taken = true; // 將事件拿走後 設為true表示可以新增新事件
    return t_pending_event;
}

bool bkeyKey_IsPressed(void){
    return (t_state == KEYSTATE_PRESSED || t_state == KEYSTATE_DEBOUNCE_PRESSED);
}