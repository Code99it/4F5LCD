// lcd_driver.c (GPIO-based)

#include "lcd_driver.h"
#include "lcd_init.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pins.h"
#include "font_PT_Sans_24px_white.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "esp_heap_caps.h"
#include "gui_config.h"
#include "color_helpers.h"

static const char *TAG = "lcd_driver";

void lcd_send_command(uint8_t cmd)
{
    gpio_set_level(LCD_RS, 0);
    lcd_write_byte(cmd);
}

void lcd_send_data(uint8_t data)
{
    gpio_set_level(LCD_RS, 1);
    lcd_write_byte(data);
}

static void lcd_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    lcd_send_command(0x2A);
    lcd_send_data(x0 >> 8);
    lcd_send_data(x0 & 0xFF);
    lcd_send_data(x1 >> 8);
    lcd_send_data(x1 & 0xFF);

    lcd_send_command(0x2B);
    lcd_send_data(y0 >> 8);
    lcd_send_data(y0 & 0xFF);
    lcd_send_data(y1 >> 8);
    lcd_send_data(y1 & 0xFF);

    lcd_send_command(0x2C);
}

void lcd_draw_pixel(int x, int y, uint16_t color)
{
    lcd_set_address_window(x, y, x, y);
    lcd_send_data(color >> 8);
    lcd_send_data(color & 0xFF);
}

void lcd_draw_bitmap(int x0, int y0, int x1, int y1, const uint16_t *bitmap)
{
    int width = x1 - x0 + 1;
    int height = y1 - y0 + 1;
    int size = width * height;

    lcd_set_address_window(x0, y0, x1, y1);
    gpio_set_level(LCD_RS, 1);

    for (int i = 0; i < size; i++) {
        lcd_write_byte(bitmap[i] >> 8);
        lcd_write_byte(bitmap[i] & 0xFF);
    }
}

void lcd_fill_screen(uint16_t color)
{
    uint16_t *buffer = heap_caps_malloc(LCD_PX_WIDTH * LCD_PX_HEIGHT * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (!buffer) {
        ESP_LOGE(TAG, "Kein Speicher für fill_buffer");
        return;
    }

    for (int i = 0; i < LCD_PX_WIDTH * LCD_PX_HEIGHT; i++) {
        buffer[i] = color;
    }

    lcd_draw_bitmap(0, 0, LCD_PX_WIDTH - 1, LCD_PX_HEIGHT - 1, buffer);
    heap_caps_free(buffer);
}
