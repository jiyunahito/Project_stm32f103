#ifndef KEY_H
#define KEY_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_PRESS,
    KEY_EVENT_RELEASE,
    KET_EVENT_LONG_PRESS    
} ekeyKeyEvent;

void vkeyKey_Init(void);
void vkeyKey_Tick(void);
ekeyKeyEvent ekeyKey_GetEvent(void);
bool bkeyKey_IsPressed(void);

#endif