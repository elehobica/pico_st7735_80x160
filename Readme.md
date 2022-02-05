![ST7735 Test](doc/pico_st7735_80x160_breadboard.jpg)
![waveshare_rp2040_lcd_096](doc/waveshare_rp2040-lcd-096.jpg)

# Raspberry Pi Pico ST7735 80x160 LCD Test

## Overview
This project is LCD display demo for ST7735 80x160.

This project supports:
* ST7735 80x160 LCD Demo
* Backlight Control by PWM

## Supported Board
* Raspberry Pi Pico
* ST7735S 80x160 LCD
* Waveshare RP2040-LCD-0.96 Board (ST7735S 80x160 LCD build-in)

## Pin Assignment
### ST7735S 80x160 LCD (Raspberry Pi Pico Board)

| Pico Pin # | Pin Name | Function | ST7735S 80x160 Board |
----|----|----|----
|12 | GP9 | GPIO | BLK |
|14 | GP10 | SPI1_SCK | SCL |
|15 | GP11 | SPI1_TX | SDA |
|17 | GP13 | SPI1_CSn | CS |
|19 | GP14 | GPIO | DC |
|20 | GP15 | GPIO | RES |
| 18 | GND | GND | GND |
| 36 | 3V3(OUT) | 3.3V | VCC |

![ST7735S_schematic](doc/pico_st7735_80x160_schematic.png)

### ST7735S 80x160 LCD (Waveshare RP2040-LCD-0.96 Board)

Uncomment #define BOARD_WAVESHARE_RP2040_LCD_096 in lcd.h

| Pin # | Pin Name | Function | Signal Name |
----|----|----|----
|11 | GP8 | GPIO | DC |
|12 | GP9 | SPI1_CSn | CS |
|14 | GP10 | SPI1_SCK | SCL |
|15 | GP11 | SPI1_TX | DIN |
|16 | GP12 | GPIO | RST |
|-- | GP25 | GPIO | BL |

### Serial (CP2102 module)
| Pico Pin # | Pin Name | Function | CP2102 module |
----|----|----|----
|  1 | GP0 | UART0_TX | RXD |
|  2 | GP1 | UART0_RX | TXD |
|  3 | GND | GND | GND |

## How to build
* See ["Getting started with Raspberry Pi Pico"](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
* Build is confirmed only in Developer Command Prompt for VS 2019 and Visual Studio Code on Windows enviroment
* Put "pico-sdk" and "pico-examples" on the same level with this project folder.
```
> git clone -b master https://github.com/raspberrypi/pico-sdk.git
> cd pico-sdk
> git submodule update -i
> cd ..
> git clone -b master https://github.com/raspberrypi/pico-examples.git
> 
> git clone -b main https://github.com/elehobica/pico_st7735_80x160.git
```
* Lanuch "Developer Command Prompt for VS 2019"
```
> cd pico_st7735_80x160
> mkdir build
> cd build
> cmake -G "NMake Makefiles" ..
> nmake
```
* Put "pico_st7735_80x160.uf2" on RPI-RP2 drive

## Backlight Control from Serial Terminal
When scrolling text display appears at demo final, program accepts Serial Terminal control.
* '+' to increase backlight level
* '-' to decrease backlight level
* 'r' to repeat demo

## Application Example
* [RPi_Pico_WAV_Player](https://github.com/elehobica/RPi_Pico_WAV_Player)
