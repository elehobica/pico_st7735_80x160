#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "lcd_extra.h"

// 0 ~ 2: Raspberry Pi Pico
// 3:     Waveshare RP2040-LCD-0.96
#define LCD_CFG_CASE 0

static void error_blink(uint led, int count)
{
    while (true) {
        for (int i = 0; i < count; i++) {
            gpio_put(led, 1);
            sleep_ms(250);
            gpio_put(led, 0);
            sleep_ms(250);
        }
        gpio_put(led, 0);
        sleep_ms(500);
    }
}

void testtexts()
{
    LCD_Clear(BLACK);
    LCD_ShowString(24,  0*16, (u8 *) "Hello World", WHITE);
    LCD_ShowString(24,  1*16, (u8 *) "Hello World", GRAY);
    LCD_ShowString(24,  2*16, (u8 *) "Hello World", RED);
    LCD_ShowString(24,  3*16, (u8 *) "Hello World", GREEN);
    LCD_ShowString(24,  4*16, (u8 *) "Hello World", BLUE);
}

void testoverlaytexts()
{
    LCD_ShowStringLn(0, 0*16, 0*8, 17*8, "Raspberry Pi Pico", 1, BLACK);
    LCD_ShowStringLn(0, 2*16, 0*8, 17*8, "Raspberry Pi Pico", 1, LIGHTBLUE);
    LCD_ShowStringLn(0, 4*16, 0*8, 17*8, "Raspberry Pi Pico", 1, GRAYBLUE);
}

void testscrolltexts(int i, int bl_val, u8 rotation)
{
    char str[16];
    static uint16_t sft0 = 0;
    static uint16_t sft1 = 0;

    LCD_Scroll_ShowString(0, 0*16, 0*16, LCD_W()-1, "Raspberry Pi Pico is a tiny, fast, and versatile board built using RP2040.", 0, GRED, &sft0, i);
    LCD_Scroll_ShowString(0, 2*16, 0*16, LCD_W()-1, "RP2040 features a dual-core Arm Cortex-M0+ processor with 264KB internal RAM and support for up to 16MB of off-chip Flash.", 0, GBLUE, &sft1, i);
    sprintf(str, "Rot: %d", (int) rotation);
    LCD_ShowString(0,  3*16, (u8 *) str, WHITE);
    sprintf(str, "BL: %3d", bl_val);
    LCD_ShowString(0,  4*16, (u8 *) str, WHITE);

}

void testpoints(uint16_t color1, uint16_t color2)
{
    LCD_Clear(BLACK);
    for (int16_t y=4; y < LCD_H(); y+=8) {
        for (int16_t x=4; x < LCD_W(); x+=8) {
            if (x == 4 || y == 4 || x >= LCD_W()-8 || y >= LCD_H()-8) {
                LCD_DrawPoint_big(x, y, color2);
            } else {
                LCD_DrawPoint(x, y, color1);
            }
        }
    }
}

void testlines(uint16_t color)
{
    LCD_Clear(BLACK);
    for (int16_t x=0; x < LCD_W(); x+=6) {
        LCD_DrawLine(0, 0, x, LCD_H()-1, color);
        sleep_ms(0);
    }
    for (int16_t y=0; y < LCD_H(); y+=6) {
        LCD_DrawLine(0, 0, LCD_W()-1, y, color);
        sleep_ms(0);
    }

    LCD_Clear(BLACK);
    for (int16_t x=0; x < LCD_W(); x+=6) {
        LCD_DrawLine(LCD_W()-1, 0, x, LCD_H()-1, color);
        sleep_ms(0);
    }
    for (int16_t y=0; y < LCD_H(); y+=6) {
        LCD_DrawLine(LCD_W()-1, 0, 0, y, color);
        sleep_ms(0);
    }

    LCD_Clear(BLACK);
    for (int16_t x=0; x < LCD_W(); x+=6) {
        LCD_DrawLine(0, LCD_H()-1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y=0; y < LCD_H(); y+=6) {
        LCD_DrawLine(0, LCD_H()-1, LCD_W()-1, y, color);
        sleep_ms(0);
    }

    LCD_Clear(BLACK);
    for (int16_t x=0; x < LCD_W(); x+=6) {
        LCD_DrawLine(LCD_W()-1, LCD_H()-1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y=0; y < LCD_H(); y+=6) {
        LCD_DrawLine(LCD_W()-1, LCD_H()-1, 0, y, color);
        sleep_ms(0);
    }
}

void testdrawrects(uint16_t color)
{
    LCD_Clear(BLACK);
    for (int16_t x=0; x < LCD_W(); x+=6) {
        LCD_DrawRectangle(LCD_W()/2 -x/2, LCD_H()/2 -x*LCD_H()/2/LCD_W(), LCD_W()/2 -x/2 + x, LCD_H()/2 -x*LCD_H()/2/LCD_W() + x*LCD_H()/LCD_W(), color);
    }
}

void testfillrects(uint16_t color1, uint16_t color2)
{
    LCD_Clear(BLACK);
    for (int16_t x=LCD_W()-1; x > 8; x-=8) {
        LCD_Fill(LCD_W()/2 -x/2, LCD_H()/2 -x*LCD_H()/2/LCD_W(), LCD_W()/2 -x/2 + x, LCD_H()/2 -x*LCD_H()/2/LCD_W() + x*LCD_H()/LCD_W(), color1);
        LCD_DrawRectangle(LCD_W()/2 -x/2, LCD_H()/2 -x*LCD_H()/2/LCD_W(), LCD_W()/2 -x/2 + x, LCD_H()/2 -x*LCD_H()/2/LCD_W() + x*LCD_H()/LCD_W(), color2);
    }
}

void testdrawcircles(uint8_t radius, uint16_t color1, uint16_t color2)
{
    LCD_Clear(BLACK);
    for (int16_t x=radius; x < LCD_W(); x+=radius*2) {
        for (int16_t y=radius; y < LCD_H(); y+=radius*2) {
            Draw_Circle(x, y, radius, color1);
            for (int8_t r = radius-1; r > 0; r--) {
                Draw_Circle(x, y, r, color2);
            }
        }
    }
}

int main()
{
    const uint32_t TimeStay = 1000;
    u8 rotation = 0;

    stdio_init_all();

    // Initialise UART 0
    uart_init(uart0, 115200);
    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // LED
    const uint32_t LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

#if LCD_CFG_CASE == 0
    pico_st7735_80x160_config_t lcd_cfg = {
        SPI_CLK_FREQ_DEFAULT,
        spi1,
        PIN_LCD_SPI1_CS_DEFAULT,
        PIN_LCD_SPI1_SCK_DEFAULT,
        PIN_LCD_SPI1_MOSI_DEFAULT,
        PIN_LCD_DC_DEFAULT,
        PIN_LCD_RST_DEFAULT,
        PIN_LCD_BLK_DEFAULT,
        PWM_BLK_DEFAULT,
        INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
        RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
        ROTATION_DEFAULT,
        H_OFS_DEFAULT,
        V_OFS_DEFAULT,
        X_MIRROR_DEFAULT
    };
#elif LCD_CFG_CASE == 1
    pico_st7735_80x160_config_t lcd_cfg = {
        SPI_CLK_FREQ_DEFAULT,
        spi1,
        PIN_LCD_SPI1_CS_DEFAULT,
        PIN_LCD_SPI1_SCK_DEFAULT,
        PIN_LCD_SPI1_MOSI_DEFAULT,
        PIN_LCD_DC_DEFAULT,
        PIN_LCD_RST_DEFAULT,
        PIN_LCD_BLK_DEFAULT,
        PWM_BLK_DEFAULT,
        0,  //INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
        RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
        ROTATION_DEFAULT,
        0,  //H_OFS_DEFAULT,
        24,  //V_OFS_DEFAULT
        X_MIRROR_DEFAULT
    };
#elif LCD_CFG_CASE == 2
    pico_st7735_80x160_config_t lcd_cfg = {
        SPI_CLK_FREQ_DEFAULT,
        spi1,
        PIN_LCD_SPI1_CS_DEFAULT,
        PIN_LCD_SPI1_SCK_DEFAULT,
        PIN_LCD_SPI1_MOSI_DEFAULT,
        PIN_LCD_DC_DEFAULT,
        PIN_LCD_RST_DEFAULT,
        PIN_LCD_BLK_DEFAULT,
        PWM_BLK_DEFAULT,
        INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
        0,  //RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
        ROTATION_DEFAULT,
        H_OFS_DEFAULT,
        V_OFS_DEFAULT,
        1  //X_MIRROR_DEFAULT
    };
#elif LCD_CFG_CASE == 3
    pico_st7735_80x160_config_t lcd_cfg = {
        SPI_CLK_FREQ_DEFAULT,
        spi1,
        PIN_LCD_SPI1_CS_WAVESHARE,
        PIN_LCD_SPI1_SCK_WAVESHARE,
        PIN_LCD_SPI1_MOSI_WAVESHARE,
        PIN_LCD_DC_WAVESHARE,
        PIN_LCD_RST_WAVESHARE,
        PIN_LCD_BLK_WAVESHARE,
        PWM_BLK_DEFAULT,
        INVERSION_DEFAULT,  // 0: non-color-inversion, 1: color-inversion
        RGB_ORDER_DEFAULT,  // 0: RGB, 1: BGR
        ROTATION_DEFAULT,
        H_OFS_DEFAULT,
        V_OFS_DEFAULT,
        X_MIRROR_DEFAULT
    };
#endif

    // Discard any input from Serial
    while (uart_is_readable(uart0)) {
        uart_getc(uart0);
    }
    printf("\n");
    //printf("Type any character to start\n");
    //while (!uart_is_readable_within_us(uart0, 1000));

    printf("========================\n");
    printf("== pico_st7735_80x160 ==\n");
    printf("========================\n");
    printf("-: ncrease back light\n");
    printf("=: decrease back light\n");
    printf("r: repeat demo\n");

    LCD_Config(&lcd_cfg);
    LCD_Init();
    u8 bl_val = OLED_BLK_Get_PWM();

    while (1) {
        LCD_SetRotation(rotation);

        LCD_Clear(BLACK);
        BACK_COLOR=BLACK;

        testtexts();
        sleep_ms(TimeStay);

        testpoints(GREEN, RED);
        sleep_ms(TimeStay);

        testlines(YELLOW);
        sleep_ms(TimeStay);

        testdrawrects(GREEN);
        sleep_ms(TimeStay);

        testfillrects(YELLOW, MAGENTA);
        sleep_ms(TimeStay);

        testdrawcircles(10, DARKGRAY, BLUE);
        sleep_ms(TimeStay);

        testoverlaytexts();
        sleep_ms(TimeStay);

        LCD_Clear(BLACK);

        // Demo end (Text Scroll & Serial command)
        for (int i = 0; i < 512; i++) {
            int c = getchar_timeout_us(0);
            switch (c) {
                case '-':
                    if (bl_val) bl_val--;
                    OLED_BLK_Set_PWM(bl_val);
                    break;
                case '=':
                case '+':
                    if (bl_val < 255) bl_val++;
                    OLED_BLK_Set_PWM(bl_val);
                    break;
                default:
                    break;
            }
            //if (c == 'r') break; // repeat demo
            testscrolltexts(i, bl_val, rotation);
            sleep_ms(2);
        }
        rotation = (rotation + 1) & 0x3;
    }
    printf("\nDone\n");

    return 0;
}
