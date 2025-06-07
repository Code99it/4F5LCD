#ifndef LCD_DRV_ST7796_H
#define LCD_DRV_ST7796_H

#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"  

// Initialisierung der GPIOs (einmalig beim Start)
void lcd_gpio_init(void);

// Display-Hardreset über RST-Pin
void lcd_reset(void);

// Display initialisieren (Power-On-Sequenz)
void lcd_init(void);

// Schreibe ein 8-Bit Kommando auf den Bus
void lcd_write_command(uint8_t cmd);

// Schreibe ein 8-Bit Datenwert auf den Bus
void lcd_write_data(uint8_t data);

// Schreibe einen 16-Bit RGB565 Farbwert (High-Byte zuerst)
void lcd_write_color(uint16_t color);

// Schreibe nacheinander zwei Bytes auf den Bus (z. B. für X-/Y-Position)
void lcd_write_data16(uint16_t data);

void lcd_write_color(uint16_t color);

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);

#endif 
