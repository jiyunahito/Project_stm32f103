#ifndef LCD_H
#define LCD_H

#include "main.h"

#define LCD_BL_ON() LL_GPIO_SetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)
#define LCD_BL_OFF() LL_GPIO_ResetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)

#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define BRED           0xF81F
#define GRED           0xFFE0
#define GBLUE          0x07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x07FF
#define YELLOW         0xFFE0
#define BROWN          0xBC40
#define GRAY           0x8430

#define X_AXIS 0
#define Y_AXIS 1

/* FSMC 位址映射 (Bank1 NE4 & A10)*/
/* 16bit下 A10對應 HADDR[11], 因此RS = 1時 偏移量要為 0x0000_0800*/
#define LCD_BASE ((uint32_t)(0x6C000000 | 0x000007FE))
#define LCD ((LCD_TypeDef *) LCD_BASE)

typedef struct
{
    volatile uint16_t lcd_reg; // RS = 0 (寫指令) 地址為 0x6C00_07FE
    volatile uint16_t lcd_ram; // RS = 1 (寫資料) 地址為 0x6C00_0800
} LCD_TypeDef;

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint8_t dir;
} tlcdLCD_Info;

extern tlcdLCD_Info tlcdLCDInfo;

void vlcdLCD_Init(void);
void vlcdLCD_Write_Reg(uint16_t regval);
void vlcdLCD_Write_Data(uint16_t data);
uint16_t ulcdLCD_Read_Data(void);
void vlcdLCD_Write(uint16_t lcd_reg, uint16_t value);

void vlcdLCD_Set_Cursor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end);
void vlcdLCD_Clear(uint16_t color);
void vlcdLCD_Draw_Rectangle(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t color);
void vlcdLCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);
void vlcdLCD_Draw_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

#endif