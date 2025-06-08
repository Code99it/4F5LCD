#include "gpio.h"
#include "Code99-API-LCD-ILI9486.h"
#include "Code99-DRV-LCD-ILI9486-8-wire-parallel.h"

void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {

    lcd_ili9486_column_address_set(
        (x1 >> 8) & 0xFF, x1 & 0xFF,
        (x2 >> 8) & 0xFF, x2 & 0xFF
    );

    lcd_ili9486_page_address_set(
        (y1 >> 8) & 0xFF, y1 & 0xFF,
        (y2 >> 8) & 0xFF, y2 & 0xFF
    );

    uint32_t total_pixels = (x2 - x1 + 1) * (y2 - y1 + 1);
    printf("LCD drawing %ld pixels from %d,%d to %d,%d in color %#06x\n", total_pixels, x1, y1, x2, y2, color);
    lcd_ili9486_memory_write(color, total_pixels);
}

void lcd_draw_test_pattern() {
	printf("LCD drawing test pattern\n");
    lcd_ili9486_column_address_set(0, 0, 0, 10);
    lcd_ili9486_page_address_set(0, 0, 0, 10);
    lcd_ili9486_memory_write(0xF800, 121); 
}