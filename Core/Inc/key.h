#ifndef KEY_H
#define KEY_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define ACTIVE_LOW 0U
#define ACTIVE_HIGH !ACTIVE_LOW

typedef enum{
    KEYSTATE_IDLE = 0,
    KEYSTATE_DEBOUNCE_PRESSED,
    KEYSTATE_PRESSED,
    KEYSTATE_DEBOUNCE_RELEASED
} ekeyKeyState;

typedef enum{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_PRESS,
    KEY_EVENT_RELEASE,
    KEY_EVENT_LONG_PRESS    
} ekeyKeyEvent;

typedef struct{
    /* key port 硬體設定*/
    GPIO_TypeDef *port;
    uint16_t pin;
    uint16_t active_level;

    uint32_t entry_tick; // 紀錄防彈跳時間
    ekeyKeyEvent pending_event; // 存放待處理事件(如果有兩事件 會保留舊事件 新事件丟棄)
    ekeyKeyState state;  // 狀態機(也可宣告成 uint32_t 如果狀態機不想寫在.h檔的話)
    bool event_taken;  // 事件是否被拿走了
} tkeyKey;

void vkeyKey_Init(GPIO_TypeDef *port, uint16_t pin, uint16_t active_level, tkeyKey *key);
void vkeyKey_Tick(tkeyKey *key);
ekeyKeyEvent ekeyKey_GetEvent(tkeyKey *key);
bool bkeyKey_IsPressed(tkeyKey *key);

#endif