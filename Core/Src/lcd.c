#include "lcd.h"

tlcdLCD_Info tlcdLCDInfo;

void vlcdLCD_Init(void){
    vlcdLCD_Write_Reg(0xD3);
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = (tlcdLCDInfo.id << 8) | ulcdLCD_Read_Data();

    if(tlcdLCDInfo.id == 0x9341){
        return;
        // tlcdLCDInfo.width = 240;
        // tlcdLCDInfo.height = 320;

        // vlcdLCD_Write_Reg(0xCF);
        // vlcdLCD_Write_Data(0x00); vlcdLCD_Write_Data(0xC1); vlcdLCD_Write_Data(0x30);

        // vlcdLCD_Write_Reg(0xED);
        // vlcdLCD_Write_Data(0x64); vlcdLCD_Write_Data(0x03); vlcdLCD_Write_Data(0x12); vlcdLCD_Write_Data(0x81);

        // vlcdLCD_Write_Reg(0x28);
        // vlcdLCD_Write_Reg(0x11);
        // vlcdLCD_Write_Reg(0x29);

        // vlcdLCD_Clear(WHITE);
    }
    
    vlcdLCD_Write_Reg(0x04);
    (void)ulcdLCD_Read_Data();
    (void)ulcdLCD_Read_Data();
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    tlcdLCDInfo.id = (tlcdLCDInfo.id << 8) | ulcdLCD_Read_Data();
    if(tlcdLCDInfo.id == 0x8552){
        tlcdLCDInfo.height = 240;
        tlcdLCDInfo.width = 320;
        /* 背光開啟 */
        LCD_BL_ON();
        /* software reset */
        vlcdLCD_Write_Reg(0x01);
        /* sleep mode out */
        vlcdLCD_Write_Reg(0x11);
        /* pixel format */
        vlcdLCD_Write(0x3A, 0x05);
        /* memory access control */
        vlcdLCD_Write(0x36, 0x00);

        // 6. 門極與垂直掃描時序配置 (ST7789V 特有參數)
        vlcdLCD_Write_Reg(0xB2); // Porch Setting
        vlcdLCD_Write_Data(0x0C);
        vlcdLCD_Write_Data(0x0C);
        vlcdLCD_Write_Data(0x00);
        vlcdLCD_Write_Data(0x33);
        vlcdLCD_Write_Data(0x33);

        vlcdLCD_Write_Reg(0xB7); // Gate Control
        vlcdLCD_Write_Data(0x35);

        vlcdLCD_Write_Reg(0xBB); // VCOMS Setting
        vlcdLCD_Write_Data(0x19);

        vlcdLCD_Write_Reg(0xC0); // LCM Control
        vlcdLCD_Write_Data(0x2C);

        vlcdLCD_Write_Reg(0xC2); // VDV and VRH Command Enable
        vlcdLCD_Write_Data(0x01);

        vlcdLCD_Write_Reg(0xC3); // VRH Set
        vlcdLCD_Write_Data(0x12);

        vlcdLCD_Write_Reg(0xC4); // VDV Set
        vlcdLCD_Write_Data(0x20);

        vlcdLCD_Write_Reg(0xC6); // Frame Rate Control (60Hz)
        vlcdLCD_Write_Data(0x0F);

        vlcdLCD_Write_Reg(0xD0); // Power Control 1
        vlcdLCD_Write_Data(0xA4);
        vlcdLCD_Write_Data(0xA1);

        // 7. Positive Gamma Correction
        vlcdLCD_Write_Reg(0xE0);
        vlcdLCD_Write_Data(0xD0); vlcdLCD_Write_Data(0x04); vlcdLCD_Write_Data(0x0D); vlcdLCD_Write_Data(0x11);
        vlcdLCD_Write_Data(0x13); vlcdLCD_Write_Data(0x2B); vlcdLCD_Write_Data(0x3F); vlcdLCD_Write_Data(0x54);
        vlcdLCD_Write_Data(0x4C); vlcdLCD_Write_Data(0x18); vlcdLCD_Write_Data(0x0D); vlcdLCD_Write_Data(0x0B);
        vlcdLCD_Write_Data(0x1F); vlcdLCD_Write_Data(0x23);

        // 8. Negative Gamma Correction
        vlcdLCD_Write_Reg(0xE1);
        vlcdLCD_Write_Data(0xD0); vlcdLCD_Write_Data(0x04); vlcdLCD_Write_Data(0x0C); vlcdLCD_Write_Data(0x11);
        vlcdLCD_Write_Data(0x13); vlcdLCD_Write_Data(0x2C); vlcdLCD_Write_Data(0x3F); vlcdLCD_Write_Data(0x44);
        vlcdLCD_Write_Data(0x51); vlcdLCD_Write_Data(0x2F); vlcdLCD_Write_Data(0x1F); vlcdLCD_Write_Data(0x1F);
        vlcdLCD_Write_Data(0x20); vlcdLCD_Write_Data(0x23);

        // 9. 開啟顯示 (Display ON)
        vlcdLCD_Write_Reg(0x29);        
        LL_mDelay(1);
        // 10. 預設全螢幕清為白色
        vlcdLCD_Clear(WHITE);
        return;
    }

    vlcdLCD_Write_Reg(0x00);
    tlcdLCDInfo.id = ulcdLCD_Read_Data();
    if(tlcdLCDInfo.id == 0x9325){
        return;
    }
}

inline void vlcdLCD_Write_Reg(uint16_t regval){
    LCD->lcd_reg = regval;
}

inline void vlcdLCD_Write_Data(uint16_t data){
    LCD->lcd_ram = data;
}

inline uint16_t ulcdLCD_Read_Data(void){
    return LCD->lcd_ram;
}

inline void vlcdLCD_Write(uint16_t lcd_reg, uint16_t value){
    LCD->lcd_reg = lcd_reg;
    LCD->lcd_ram = value;
}

void vlcdLCD_Set_Cursor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end){
    vlcdLCD_Write_Reg(0x2A);
    vlcdLCD_Write_Data(x_start >> 8); vlcdLCD_Write_Data(x_start & 0xFF);
    vlcdLCD_Write_Data(x_end >> 8); vlcdLCD_Write_Data(x_end & 0xFF);
    vlcdLCD_Write_Reg(0x2B);
    vlcdLCD_Write_Data(y_start >> 8); vlcdLCD_Write_Data(y_start & 0xFF);
    vlcdLCD_Write_Data(y_end >> 8); vlcdLCD_Write_Data(y_end & 0xFF);
    vlcdLCD_Write_Reg(0x2C);
}

void vlcdLCD_Clear(uint16_t color){
    uint32_t index = 0;
    uint32_t total_point = tlcdLCDInfo.height * tlcdLCDInfo.width;

    vlcdLCD_Set_Cursor(0, tlcdLCDInfo.width - 1, 0, tlcdLCDInfo.height - 1);
    for(index = 0; index < total_point; index++){
        vlcdLCD_Write_Data(color);
    }
}