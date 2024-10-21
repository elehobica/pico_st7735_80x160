#include "lcd.h"

#include "pico.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "lcd_private.h"
#include "oledfont.h"

// Macro definitions
#define LCD_WIDTH 160
#define LCD_HEIGHT 80

#define OLED_CS_Clr() gpio_put(_config.pin_cs, 0)
#define OLED_CS_Set() gpio_put(_config.pin_cs, 1)

#define OLED_RST_Clr() gpio_put(_config.pin_rst, 0)
#define OLED_RST_Set() gpio_put(_config.pin_rst, 1)

#define OLED_DC_Clr() gpio_put(_config.pin_dc, 0)
#define OLED_DC_Set() gpio_put(_config.pin_dc, 1)

#define DEFAULT_PWM_BLK_LEVEL 196

#define OLED_CMD  0 //write command
#define OLED_DATA 1 //write data

/******************************************************************************
   Pin selection
******************************************************************************/
static pico_st7735_80x160_config_t _config = {
    SPI_CLK_FREQ_DEFAULT,
#if SPI_INST_DEFAULT
    spi1,
    PIN_LCD_SPI1_CS_DEFAULT,
    PIN_LCD_SPI1_SCK_DEFAULT,
    PIN_LCD_SPI1_MOSI_DEFAULT,
#else
    spi0,
    PIN_LCD_SPI0_CS_DEFAULT,
    PIN_LCD_SPI0_SCK_DEFAULT,
    PIN_LCD_SPI0_MOSI_DEFAULT,
#endif
    PIN_LCD_DC_DEFAULT,
    PIN_LCD_RST_DEFAULT,
    PIN_LCD_BLK_DEFAULT,
    PWM_BLK_DEFAULT,
    INVERSION_DEFAULT,
    RGB_ORDER_DEFAULT,
    ROTATION_DEFAULT,
    H_OFS_DEFAULT,
    V_OFS_DEFAULT,
    X_MIRROR_DEFAULT
};

static u8 _pwm_blk_level = DEFAULT_PWM_BLK_LEVEL;
static u8 _rotation = 0;  //Set horizontal or vertical screen display. 0 or 1 is vertical screen, 2 or 3 is horizontal screen.
u16 BACK_COLOR;   //background color

/******************************************************************************
    Function description: display numbers
      Entry data: m base, n index
      Return value: None
******************************************************************************/
static u32 mypow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_config(void)
{
    gpio_init(_config.pin_sck);
    gpio_set_function(_config.pin_sck, GPIO_FUNC_SPI);

    gpio_init(_config.pin_mosi);
    gpio_set_function(_config.pin_mosi, GPIO_FUNC_SPI);

    gpio_init(_config.pin_cs);
    gpio_set_dir(_config.pin_cs, GPIO_OUT);

    spi_init(_config.spi_inst, _config.clk_freq);

    /* SPI parameter config */
    spi_set_format(
        _config.spi_inst,
        8, /* data_bits */
        SPI_CPOL_0, /* cpol */
        SPI_CPHA_0, /* cpha */
        SPI_MSB_FIRST /* order */
    );

    OLED_CS_Set();
}

static void set_blk_pwm()
{
    // BackLight PWM (125MHz / 65536 / 4 = 476.84 Hz)
    gpio_set_function(_config.pin_blk, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(_config.pin_blk);
    pwm_config pwm_cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&pwm_cfg, 4.f);
    pwm_init(slice_num, &pwm_cfg, true);
}

static void set_blk_gpio()
{
    gpio_init(_config.pin_blk);
    gpio_set_dir(_config.pin_blk, GPIO_OUT);
}

/******************************************************************************
    Function description: LCD serial data writing function
      Entry data: dat serial data to be written
      Return value: None
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    OLED_CS_Clr();

    uint8_t *buff = (uint8_t *) &dat;
    spi_write_blocking(_config.spi_inst, buff, 1);

    OLED_CS_Set();
}


/******************************************************************************
    Function description: LCD writes data
      Entry data: data written by dat
      Return value: None
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    OLED_DC_Set();//write data
    LCD_Writ_Bus(dat);
}


/******************************************************************************
    Function description: LCD writes data
      Entry data: data written by dat
      Return value: None
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    OLED_DC_Set();//write data
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}


/******************************************************************************
    Function description: LCD write command
      Entry data: command written by dat
      Return value: None
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    OLED_DC_Clr();//write command
    LCD_Writ_Bus(dat);
}


/******************************************************************************
    Function description: Set the start and end addresses
      Entry data: x1,x2 sets the start and end addresses of the column
                y1,y2 set the start and end addresses of the line
      Return value: None
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(_rotation==0)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+_config.v_ofs);
        LCD_WR_DATA(x2+_config.v_ofs);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+_config.h_ofs);
        LCD_WR_DATA(y2+_config.h_ofs);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(_rotation==1)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+_config.v_ofs);
        LCD_WR_DATA(x2+_config.v_ofs);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+_config.h_ofs);
        LCD_WR_DATA(y2+_config.h_ofs);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(_rotation==2)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+_config.h_ofs);
        LCD_WR_DATA(x2+_config.h_ofs);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+_config.v_ofs);
        LCD_WR_DATA(y2+_config.v_ofs);
        LCD_WR_REG(0x2c);//储存器写
    }
    else
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WR_DATA(x1+_config.h_ofs);
        LCD_WR_DATA(x2+_config.h_ofs);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WR_DATA(y1+_config.v_ofs);
        LCD_WR_DATA(y2+_config.v_ofs);
        LCD_WR_REG(0x2c);//储存器写
    }
}

/******************************************************************************
* Set configuration
*
* @param[in] config the pointer of pico_st7735_80x160_config_t to configure with
******************************************************************************/
void LCD_Config(pico_st7735_80x160_config_t* config)
{
    _config = *config;
}

/******************************************************************************
    Function description: LCD initialization function
      Entry data: None
      Return value: None
******************************************************************************/
void LCD_Init(void)
{
    gpio_init(_config.pin_dc);
    gpio_set_dir(_config.pin_dc, GPIO_OUT);

    gpio_init(_config.pin_rst);
    gpio_set_dir(_config.pin_rst, GPIO_OUT);

    if (_config.pwm_blk)
    {
        set_blk_pwm();
    }
    else
    {
        set_blk_gpio();
    }

    spi_config();

    OLED_RST_Clr();
    sleep_ms(200);
    OLED_RST_Set();
    sleep_ms(20);
    OLED_BLK_Set();

    LCD_WR_REG(0x11);   // turn off sleep mode
    sleep_ms(100);

    if (_config.inversion)
    {
        LCD_WR_REG(0x21);   // display inversion mode
    }

    LCD_WR_REG(0xB1);   // Set the frame frequency of the full colors normal mode
                        // Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
                        // fosc = 850kHz
    LCD_WR_DATA8(0x05); // RTNA
    LCD_WR_DATA8(0x3A); // FPA
    LCD_WR_DATA8(0x3A); // BPA

    LCD_WR_REG(0xB2);   // Set the frame frequency of the Idle mode
                        // Frame rate=fosc/((RTNB x 2 + 40) x (LINE + FPB + BPB +2))
                        // fosc = 850kHz
    LCD_WR_DATA8(0x05); // RTNB
    LCD_WR_DATA8(0x3A); // FPB
    LCD_WR_DATA8(0x3A); // BPB

    LCD_WR_REG(0xB3);   // Set the frame frequency of the Partial mode/ full colors
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);

    LCD_WR_REG(0xB4);
    LCD_WR_DATA8(0x03);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x62);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x04);

    LCD_WR_REG(0xC1);
    LCD_WR_DATA8(0xC0);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x00);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0x6A);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0xEE);

    LCD_WR_REG(0xC5);  /*VCOM*/
    LCD_WR_DATA8(0x0E);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0x10);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0A);
    LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x27);
    LCD_WR_DATA8(0x37);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x10);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0x10);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x0F);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0A);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x26);
    LCD_WR_DATA8(0x36);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x10);

    LCD_WR_REG(0x3A);   // define the format of RGB picture data
    LCD_WR_DATA8(0x05); // 16-bit/pixel

    LCD_SetRotation(_rotation);
    /*
    // Memory data access control (MADCTL)
    LCD_WR_REG(0x36);
    if(_rotation==0)LCD_WR_DATA8(0x00 | (_config.rgb_order<<3));
    else if(_rotation==1)LCD_WR_DATA8(0xC0 | (_config.rgb_order<<3));
    else if(_rotation==2)LCD_WR_DATA8(0x70 | (_config.rgb_order<<3));
    else LCD_WR_DATA8(0xA0 | (_config.rgb_order<<3));
    */

    LCD_WR_REG(0x29);   // Display On
}

/******************************************************************************
      Function description：LCD clear screen function
      Entry data: None
      Return value: None
******************************************************************************/
void LCD_Clear(u16 Color)
{
    u16 i,j;
    LCD_Address_Set(0,0,LCD_W()-1,LCD_H()-1);
    for(i=0;i<LCD_W();i++)
    {
        for (j=0;j<LCD_H();j++)
        {
            LCD_WR_DATA(Color);
        }
    }
}

void LCD_SetRotation(u8 rot)
{
    _rotation = rot ^ _config.rotation;
    // Memory data access control (MADCTL)
    // D7: MY   Row Address Order
    // D6: M    Column Address Order
    // D5: MV   Row/Column Exchange
    // D4: ML   Vertical Refresh Order
    // D3: RGB  RGB-BGR Order
    // D2: MH   Horizontal Refresh Order
    LCD_WR_REG(0x36);
    if(_rotation==0)
        LCD_WR_DATA8((0x00 ^ (_config.x_mirror<<7)) | (_config.rgb_order<<3));
    else if(_rotation==1)
        LCD_WR_DATA8((0xC0 ^ (_config.x_mirror<<7)) | (_config.rgb_order<<3));
    else if(_rotation==2)
        LCD_WR_DATA8((0x70 ^ (_config.x_mirror<<6)) | (_config.rgb_order<<3));
    else
        LCD_WR_DATA8((0xA0 ^ (_config.x_mirror<<6)) | (_config.rgb_order<<3));
}

u16 LCD_W()
{
    if (_rotation == 0 || _rotation == 1) {
        return LCD_HEIGHT;
    } else {
        return LCD_WIDTH;
    }
}

u16 LCD_H()
{
    if (_rotation == 0 || _rotation == 1) {
        return LCD_WIDTH;
    } else {
        return LCD_HEIGHT;
    }
}

#if 0
/******************************************************************************
    Function description: LCD displays Chinese characters
      Entry data: x,y starting coordinates
                index Chinese character serial number
                size font size
      Return value: None
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)
{
    u8 i,j;
    u8 *temp,size1;
    if(size==16){temp=Hzk16;}//选择字号
    if(size==32){temp=Hzk32;}
  LCD_Address_Set(x,y,x+size-1,y+size-1); //设置一个汉字的区域
  size1=size*size/8;//一个汉字所占的字节
    temp+=index*size1;//写入的起始位置
    for(j=0;j<size1;j++)
    {
        for(i=0;i<8;i++)
        {
            if((*temp&(1<<i))!=0)//从数据的低位开始读
            {
                LCD_WR_DATA(color);//点亮
            }
            else
            {
                LCD_WR_DATA(BACK_COLOR);//不点亮
            }
        }
        temp++;
     }
}
#endif


/******************************************************************************
    Function description: LCD displays Chinese characters
      Entry data: x,y starting coordinates
      Return value: None
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
    LCD_Address_Set(x,y,x,y);//Set cursor position
    LCD_WR_DATA(color);
}


/******************************************************************************
    Function description: Draw a large dot on LCD
      Entry data: x,y starting coordinates
      Return value: None
******************************************************************************/
void LCD_DrawPoint_big(u16 x,u16 y,u16 color)
{
    LCD_Fill(x-1,y-1,x+1,y+1,color);
}


/******************************************************************************
    Function description: Fill the specified area with color
      Entry data: xsta, ysta starting coordinates
                xend,yend end coordinates
      Return value: None
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
    u16 i,j;
    LCD_Address_Set(xsta,ysta,xend,yend);      //Set cursor position
    for(i=ysta;i<=yend;i++)
    {
        for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//Set cursor position
    }
}


/******************************************************************************
    Function description: line drawing
      Entry data: x1, y1 starting coordinates
                x2,y2 end coordinates
      Return value: None
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //Calculate coordinate increment
    delta_y=y2-y1;
    uRow=x1;//Coordinates of starting point of drawing line
    uCol=y1;
    if(delta_x>0)incx=1; //Set single step direction
    else if (delta_x==0)incx=0;//vertical line
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//horizontal line
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x; //Select base incremental axis
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);//painting point
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


/******************************************************************************
    Function description: Draw a rectangle
      Entry data: x1, y1 starting coordinates
                x2,y2 end coordinates
      Return value: None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
    Function description: draw a circle
      Entrance data: x0, y0 circle center coordinates
                r radius
      Return value: None
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
    int a,b;
    // int di;
    a=0;b=r;
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3
        LCD_DrawPoint(x0+b,y0-a,color);             //0
        LCD_DrawPoint(x0-a,y0+b,color);             //1
        LCD_DrawPoint(x0-a,y0-b,color);             //2
        LCD_DrawPoint(x0+b,y0+a,color);             //4
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//Determine whether the point to be drawn is too far away
        {
            b--;
        }
    }
}


/******************************************************************************
    Function description: display characters
      Entry data: x,y starting point coordinates
                num characters to display
                mode 1 superposition mode 0 non-superposition mode
      Return value: None
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color)
{
    u8 temp;
    u8 pos,t;
      u16 x0=x;
    if(x>LCD_W()-8||y>LCD_H()-16)return;        //settings window
    num=num-' ';//
    LCD_Address_Set(x,y,x+8-1,y+16-1);      //Set cursor position
    if(!mode) //non-superposition method
    {
        for(pos=0;pos<16;pos++)
        {
            temp=asc2_1608[(u16)num*16+pos];         //Call 1608 font
            for(t=0;t<8;t++)
            {
                if(temp&0x01)LCD_WR_DATA(color);
                else LCD_WR_DATA(BACK_COLOR);
                temp>>=1;
                x++;
            }
            x=x0;
            y++;
        }
    }else//Overlay method
    {
        for(pos=0;pos<16;pos++)
        {
            temp=asc2_1608[(u16)num*16+pos];         //Call 1608 font
            for(t=0;t<8;t++)
            {
                if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//draw a dot
                temp>>=1;
            }
        }
    }
}


/******************************************************************************
    Function description: display string
      Entry data: x,y starting point coordinates
                *p string starting address
      Return value: None
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color)
{
    while(*p!='\0')
    {
        if(x>LCD_W()-8){x=0;y+=16;}
        if(y>LCD_H()-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }
}


/******************************************************************************
    Function description: display numbers
      Entry data: x,y starting point coordinates
                num number to display
                len The number of digits to be displayed
      Return value: None
******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
{
    u8 t,temp;
    u8 enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+8*t,y,' ',0,color);
                continue;
            }else enshow=1;

        }
        LCD_ShowChar(x+8*t,y,temp+48,0,color);
    }
}


/******************************************************************************
    Function description: display decimals
      Entry data: x,y starting point coordinates
                num decimal to display
                len The number of digits to be displayed
      Return value: None
******************************************************************************/
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
{
    u8 t,temp;
    // u8 enshow=0;
    u16 num1;
    num1=num*100;
    for(t=0;t<len;t++)
    {
        temp=(num1/mypow(10,len-t-1))%10;
        if(t==(len-2))
        {
            LCD_ShowChar(x+8*(len-2),y,'.',0,color);
            t++;
            len+=1;
        }
        LCD_ShowChar(x+8*t,y,temp+48,0,color);
    }
}


/******************************************************************************
    Function description: display 40x40 picture
      Entry data: x,y starting point coordinates
      Return value: None
******************************************************************************/
void LCD_ShowPicture(u16 x1, u16 y1, u16 x2, u16 y2, u8 *image)
{
    int i;
    int j;
    LCD_Address_Set(x1,y1,x2,y2);
    for(i=0;i<(x2-x1+1)*(y2-y1+1)*2;i++)
    {
        LCD_WR_DATA8(image[i]);
    }
}

void OLED_BLK_Clr()
{
    if (_config.pwm_blk)
        pwm_set_gpio_level(_config.pin_blk, 0);
    else
        gpio_put(_config.pin_blk, 0);
}

void OLED_BLK_Set()
{
    if (_config.pwm_blk)
        // Square bl_val to make brightness appear more linear
        pwm_set_gpio_level(_config.pin_blk, (uint16_t) _pwm_blk_level * _pwm_blk_level);
    else
        gpio_put(_config.pin_blk, 1);
}

void OLED_BLK_Set_PWM(u8 level)
{
    if (_config.pwm_blk)
    {
        if (level == 0) {
            if (_pwm_blk_level != 0) {
                set_blk_gpio();
                gpio_put(_config.pin_blk, false);
            }
            _pwm_blk_level = level;
        } else {
            if (_pwm_blk_level == 0) {
                set_blk_pwm();
            }
            _pwm_blk_level = level;
            // Square bl_val to make brightness appear more linear
            pwm_set_gpio_level(_config.pin_blk, (uint16_t) _pwm_blk_level * _pwm_blk_level);
        }
    }
}

u8 OLED_BLK_Get_PWM(void)
{
    return _pwm_blk_level;
}
