# Raspberry Pi Pico ST7735 80x160 LCD Library Demo test

## Pin Assignment
In addition to original connection

### Serial (CP2102 module)
| Pico Pin # | Pin Name | Function | CP2102 module |
----|----|----|----
|  1 | GP0 | UART0_TX | RXD |
|  2 | GP1 | UART0_RX | TXD |
|  3 | GND | GND | GND |

## Serial interface usage
### Opening
Select LCD Config type
* '0': LCD config 0
* '1': LCD config 1
* '2': LCD config 2
* '3': Waveshare RP2040-LCD-0.96

### During demo running
* '+' or '=': Increase back light
* '-': Decrease back light