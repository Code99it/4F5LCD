// lcd_driver.h
#ifndef LCD_H
#define LCD_H

// LCD Pixelbreite und -höhe
// -------------------------
#define LCD_PX_WIDTH 320
#define LCD_PX_HEIGHT 480

#include <stdint.h>

// --------------------------------------------
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
uint16_t hex_to_rgb565(const char *hex);

void lcd_write_bus(uint8_t data);
void lcd_write_command(uint8_t cmd);
void lcd_write_data(uint8_t data);
void lcd_write_data16(uint16_t data);
void lcd_reset(void);
void lcd_init(void);
void lcd_fill_screen(uint16_t color);
void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcd_draw_image(int x0, int y0, int img_width, int img_height, const uint16_t *image_data);
int lcd_measure_string(const char *text);
void lcd_flush_footer();
void lcd_flush_header();
int lcd_write_string(int x, int y, const char *text);
int lcd_write_string_colored(int x, int y, const char *text, uint16_t text_color);
void draw_circle_ring(int cx, int cy, float radius, float stroke_width, uint16_t color);
void draw_arc(int cx, int cy, float radius, float stroke_width, float start_angle_deg, float end_angle_deg, uint16_t color);

#endif 