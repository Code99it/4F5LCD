// lcd_driver_i80.h
#ifndef LCD_DRIVER_I80_H
#define LCD_DRIVER_I80_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"

#define LCD_PX_WIDTH  320
#define LCD_PX_HEIGHT 480

// Initialisierung und Zugriff
void lcd_i80_init(void);
esp_lcd_panel_handle_t lcd_i80_get_handle(void);

// Zeichnen und Füllen
void lcd_i80_draw_pixel(int x, int y, uint16_t color);
void lcd_i80_draw_bitmap(int x0, int y0, int x1, int y1, const uint16_t *bitmap);
void lcd_i80_fill_screen(uint16_t color);
void lcd_i80_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

// Textausgabe
int lcd_i80_measure_string(const char *text);
int lcd_i80_write_string_colored(int x, int y, const char *text, uint16_t text_color);
int lcd_i80_write_string(int x, int y, const char *text);

// UI-Elemente löschen
void lcd_i80_flush_footer(void);
void lcd_i80_flush_header(void);

// Grafikelemente
void draw_i80_arc(int cx, int cy, float radius, float stroke_width,
                  float start_angle_deg, float end_angle_deg, uint16_t color);
void draw_i80_circle_ring(int cx, int cy, float radius, float stroke_width, uint16_t color);

// Hilfsfunktionen
uint16_t hex_to_rgb565(const char *hex);
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
uint16_t blend_color(uint16_t fg, uint16_t bg, float alpha);

#endif // LCD_DRIVER_I80_H
