#ifndef LCD_H
#define LCD_H

#include "main.h"

#define LCD_BASE ((uint32_t)(0x6C000000 | 0x000007FE))
#define LCD ((LCD_TypeDef *) LCD_BASE)

#define LCD_BL_ON() LL_GPIO_SetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)
#define LCD_BL_OFF() LL_GPIO_ResetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin)

#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define RED            0xF800
#define GREEN          0x07E0

typedef struct
{
    volatile uint16_t lcd_reg;
    volatile uint16_t lcd_ram;
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

#endif