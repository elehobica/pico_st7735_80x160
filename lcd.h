#pragma once

#include "hardware/spi.h"
#include "hardware/clocks.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef char  i8;
typedef short i16;
typedef long  i32;

#ifndef SPI_CLK_FREQ_DEFAULT
#define SPI_CLK_FREQ_DEFAULT        (50 * MHZ)
#endif

#ifndef SPI_INST_DEFAULT
#define SPI_INST_DEFAULT            1   // 0: spi0, 1: spi1
#endif

// pin assignment
#define PIN_LCD_SPI1_CS_DEFAULT     13  //  9, 13
#define PIN_LCD_SPI1_SCK_DEFAULT    10  // 10, 14
#define PIN_LCD_SPI1_MOSI_DEFAULT   11  // 11, 15
#define PIN_LCD_DC_DEFAULT          14  // any GPIO
#define PIN_LCD_RST_DEFAULT         15  // any GPIO
#define PIN_LCD_BLK_DEFAULT         9   // any GPIO

#define PIN_LCD_SPI0_CS_DEFAULT     5   //  1,  5, 17
#define PIN_LCD_SPI0_SCK_DEFAULT    2   //  2,  6, 18
#define PIN_LCD_SPI0_MOSI_DEFAULT   3   //  3,  7, 19

#define SPI_INST_WAVESHARE          1   // 0: spi0, 1: spi1
#define PIN_LCD_SPI1_CS_WAVESHARE   9   //  9, 13
#define PIN_LCD_SPI1_SCK_WAVESHARE  10  // 10, 14
#define PIN_LCD_SPI1_MOSI_WAVESHARE 11  // 11, 15
#define PIN_LCD_DC_WAVESHARE        8   // any GPIO
#define PIN_LCD_RST_WAVESHARE       12  // any GPIO
#define PIN_LCD_BLK_WAVESHARE       25  // any GPIO

#define PWM_BLK_DEFAULT             true
#define INVERSION_DEFAULT           true  // false: non-color-inversion, true: color-inversion
#define RGB_ORDER_DEFAULT           1   // 0: RGB, 1: BGR
#define ROTATION_DEFAULT            2

#define H_OFS_DEFAULT                +1  // 0, +1
#define V_OFS_DEFAULT               +26  // +24, +26

#define X_MIRROR_DEFAULT            0    // 0: non-x-mirror, 1: x-mirror

typedef struct _pico_st7735_80x160_config_t {
    uint       clk_freq;
    spi_inst_t *spi_inst;  // spi0 or spi1
    uint       pin_cs;
    uint       pin_sck;
    uint       pin_mosi;
    uint       pin_dc;
    uint       pin_rst;
    uint       pin_blk;
    bool       pwm_blk;
    bool       inversion;
    u8         rgb_order;
    u8         rotation;
    i16        h_ofs;
    i16        v_ofs;
    u8         x_mirror;
} pico_st7735_80x160_config_t;

//-----------------OLED端口定义----------------
#ifdef __cplusplus
extern "C" {
#endif

extern  u16 BACK_COLOR;   //背景色

void LCD_Config(pico_st7735_80x160_config_t* config);
void LCD_Init(void);
void LCD_Clear(u16 Color);
void LCD_SetRotation(u8 rot);
u16 LCD_W();
u16 LCD_H();
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_DrawPoint_big(u16 x,u16 y,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color);
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color);
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);
void LCD_ShowPicture(u16 x1, u16 y1, u16 x2, u16 y2, u8* image);
void OLED_BLK_Clr();
void OLED_BLK_Set();
void OLED_BLK_Set_PWM(u8 level);
u8 OLED_BLK_Get_PWM(void);


//画笔颜色
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
//GUI颜色

#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN       0X841F //浅绿色
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define DARKGRAY         0x4208
#define DARKGREEN        0x03E0

#define DARKRED          0x7800

#ifdef __cplusplus
}
#endif
