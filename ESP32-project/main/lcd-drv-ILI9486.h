// lcd-drv-ILI9486.h

#ifndef LCD_DRV_ILI9486_H
#define LCD_DRV_ILI9486_H

#include <stdint.h>

// Öffentliche Funktionen
void lcd_init();
void lcd_read_id();

// Interne Helfer (nur verwenden, wenn nötig)
void lcd_write_command(uint8_t cmd);
void lcd_write_data(uint8_t data);
void lcd_reset();
void lcd_set_data_output();
void lcd_set_data_input();
uint8_t lcd_read_bus();
void lcd_write_bus(uint8_t data);
void lcd_run_diagnose();
void lcd_run_full_register_scan();
void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color_rgb565);
void lcd_test_fill_red();
void lcd_write_rgb565(uint16_t color);

#endif // LCD_DRV_ILI9486_H
