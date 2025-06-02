#ifndef LCD_H
#define LCD_H

#define LCD_PIXEL_WIDTH  320
#define LCD_PIXEL_HEIGHT 480

#include "driver/gpio.h"
#include "stdbool.h"

/**
 * @brief Initialisiert alle GPIOs für das LCD im 8-Bit-Parallelbetrieb.
 */
void lcd_init(void);

/**
 * @brief Schreibt ein Byte an den LCD-Datenbus (D0–D7).
 * @param data Das zu schreibende Byte.
 */
void lcd_write_data(uint8_t data);

/**
 * @brief Schreibt ein Steuerkommando an das LCD.
 * @param cmd Das Kommando-Byte.
 */
void lcd_write_command(uint8_t cmd);

/**
 * @brief Setzt einen GPIO auf HIGH oder LOW.
 */
void lcd_gpio_write(gpio_num_t pin, bool level);

/**
 * @brief Füllt den gesamten Bildschirm mit einer Farbe (RGB565).
 * @param color RGB565-Farbwert (0bRRRRRGGGGGGBBBBB)
 */
void lcd_fill_color(uint16_t color);

void lcd_driver_init(void);

/**
 * @brief Füllt ein Rechteck auf dem Display mit einer Farbe.
 * 
 * @param x0 Linke obere X-Koordinate
 * @param y0 Linke obere Y-Koordinate
 * @param x1 Rechte untere X-Koordinate
 * @param y1 Rechte untere Y-Koordinate
 * @param color RGB565-Farbwert (z. B. 0xF800 für Rot)
 */
void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void lcd_fill_rainbow(void);

void lcd_test_fill_fps(void);

#endif
