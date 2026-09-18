#include "lcd.h"

tlcdLCD_Info tlcdLCDInfo;

void vlcdLCD_Init(void){
    vlcdLCD_Write_Reg(0xD3);
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = (tlcdLCDInfo.id << 8) | ulcdLCD_Read_Data();

    if(tlcdLCDInfo.id == 0x9341){
        tlcdLCDInfo.width = 240;
        tlcdLCDInfo.height = 320;

        vlcdLCD_Write_Reg(0xCF);
        vlcdLCD_Write_Data(0x00); vlcdLCD_Write_Data(0xC1); vlcdLCD_Write_Data(0x30);

        vlcdLCD_Write_Reg(0xED);
        vlcdLCD_Write_Data(0x64); vlcdLCD_Write_Data(0x03); vlcdLCD_Write_Data(0x12); vlcdLCD_Write_Data(0x81);

        vlcdLCD_Write_Reg(0x28);
        vlcdLCD_Write_Reg(0x11);
        vlcdLCD_Write_Reg(0x29);
    }
    vlcdLCD_Clear(WHITE);
}

void vlcdLCD_Write_Reg(uint16_t regval){
    LCD->lcd_reg = regval;
}

void vlcdLCD_Write_Data(uint16_t data){
    LCD->lcd_ram = data;
}

uint16_t ulcdLCD_Read_Data(void){
    return LCD->lcd_ram;
}

void vlcdLCD_Write(uint16_t lcd_reg, uint16_t value){
    LCD->lcd_reg = lcd_reg;
    LCD->lcd_ram = value;
}

void vlcdLCD_Set_Cursor(uint16_t xpos, uint16_t ypos){
    vlcdLCD_Write_Reg(0x2A);
    vlcdLCD_Write_Data(xpos >> 8); vlcdLCD_Write_Data(xpos & 0xFF);
    vlcdLCD_Write_Reg(0x2B);
    vlcdLCD_Write_Data(ypos >> 8); vlcdLCD_Write_Data(ypos & 0xFF);
    vlcdLCD_Write_Reg(0x2C);
}

void vlcdLCD_Clear(uint16_t color){
    uint32_t index = 0;
    uint32_t total_point = tlcdLCDInfo.height * tlcdLCDInfo.width;

    vlcdLCD_Set_Cursor(0, 0);
    for(index = 0; index < total_point; index++){
        vlcdLCD_Write_Data(color);
    }
}