#pragma once

#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

void LCD_ShowPartialChar2(i16 x,i16 y,u16 x_min,u16 x_max,u16 y_min,u16 y_max,u8 num,u8 mode,u16 color);
void LCD_ShowPartialChar(i16 x,i16 y,u16 x_min,u16 x_max,u16 y_min,u16 y_max,u8 num,u8 mode,u16 color);
void LCD_ShowIcon(u16 x,u16 y,u8 *index,u8 mode,u16 color);
u16 LCD_ShowStringLn(i16 x,i16 y,u16 x_min,u16 x_max,const u8 *p,u8 mode,u16 color);
void LCD_Scroll_ShowString(u16 x, u16 y, u16 x_min, u16 x_max, u8 *p, u8 mode, u16 color, u16 *sft_val, u32 tick);
void LCD_ShowDimPicture(u16 x1, u16 y1, u16 x2, u16 y2, u8 dim);
void LCD_ShowDimPictureOfs(u16 x1, u16 y1, u16 x2, u16 y2, u8 dim, u16 ofs_x, u16 ofs_y);
void LCD_FillBackground(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 mode,u16 color);

#ifdef __cplusplus
}
#endif
