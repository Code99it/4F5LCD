#ifndef LCD_H
#define LCD_H

#define LCD_PIXEL_WIDTH  320
#define LCD_PIXEL_HEIGHT 480

#define FONT_LETTER_SPACING_PX 1

#define FB_WIDTH  80
#define FB_HEIGHT 29

#include "driver/gpio.h"
#include "stdbool.h"
#include "menu.h"

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

uint16_t scale_color_rgb565(uint16_t base_color, uint16_t brightness);

uint16_t lcd_draw_char(char c, uint16_t x, uint16_t y, uint8_t px);

uint16_t lcd_draw_colored_char(char c, uint16_t x, uint16_t y, uint8_t px, uint32_t hex_rgb);

uint16_t lcd_draw_string(const char *str, uint16_t x, uint16_t y, uint8_t px);

uint16_t lcd_draw_colored_string(const char *str, uint16_t x, uint16_t y, uint8_t px, uint32_t hex_rgb);

uint16_t lcd_draw_colored_string_centered(const char *str, uint16_t x_center, uint16_t y, uint8_t px, uint32_t hex_rgb);

void lcd_draw_horizontal_line(uint16_t y, uint16_t width, uint16_t thickness, uint16_t color);

void lcd_test_fill_area_fps(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t frames);

void lcd_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

uint16_t blend_rgb565(uint16_t fg, uint16_t bg, uint8_t alpha);

void lcd_draw_menu_select_point(
    uint16_t *fb,
    uint16_t fb_width,
    uint16_t fb_height,
    uint16_t x_offset,
    uint16_t y_offset,
    uint16_t ring_diameter,
    uint16_t ring_thickness,
    uint16_t center_diameter,
    uint32_t hex_ring_color,
    uint32_t hex_center_color,
    uint32_t hex_line_color,
    uint16_t line_length,
    uint16_t line_thickness,
    uint32_t hex_background_color
);

void lcd_fill_framebuffer(uint16_t *fb, uint16_t fb_width, uint16_t fb_height, uint32_t hex_color);

void lcd_blit_framebuffer(
    uint16_t *fb,
    uint16_t fb_width,
    uint16_t fb_height,
    uint16_t target_x,
    uint16_t target_y
);
#endif
