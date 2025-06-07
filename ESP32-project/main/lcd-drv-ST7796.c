#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void lcd_write_bus(uint8_t data) {
    gpio_set_level(LCD_DATA_0, (data >> 0) & 1);
    gpio_set_level(LCD_DATA_1, (data >> 1) & 1);
    gpio_set_level(LCD_DATA_2, (data >> 2) & 1);
    gpio_set_level(LCD_DATA_3, (data >> 3) & 1);
    gpio_set_level(LCD_DATA_4, (data >> 4) & 1);
    gpio_set_level(LCD_DATA_5, (data >> 5) & 1);
    gpio_set_level(LCD_DATA_6, (data >> 6) & 1);
    gpio_set_level(LCD_DATA_7, (data >> 7) & 1);
}

void lcd_write_command(uint8_t cmd) {
    gpio_set_level(LCD_RS, LOW); // Command mode
    gpio_set_level(LCD_CS, LOW);
    lcd_write_bus(cmd);
    gpio_set_level(LCD_WR, LOW);
    gpio_set_level(LCD_WR, HIGH);
    gpio_set_level(LCD_CS, HIGH);
}

void lcd_write_data(uint8_t data) {
    gpio_set_level(LCD_RS, HIGH); // Data mode
    gpio_set_level(LCD_CS, LOW);
    lcd_write_bus(data);
    gpio_set_level(LCD_WR, LOW);
    gpio_set_level(LCD_WR, HIGH);
    gpio_set_level(LCD_CS, HIGH);
}

void lcd_write_color(uint16_t color) {
    lcd_write_data(color >> 8);      // High Byte
    lcd_write_data(color & 0xFF);    // Low Byte
}

void lcd_reset() {
    gpio_set_level(LCD_RST, LOW);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(LCD_RST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(120));
}

void lcd_init() {
    lcd_reset();

    lcd_write_command(0x01); // SWRESET
    vTaskDelay(pdMS_TO_TICKS(150));

    lcd_write_command(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_write_command(0x3A); // Interface Pixel Format
    lcd_write_data(0x55);    // 16bit/pixel

    lcd_write_command(0x36); // MADCTL
    lcd_write_data(0x48);    // MX, BGR

    lcd_write_command(0xB4); // Display Inversion Control
    lcd_write_data(0x01);    // 2-dot inversion

    lcd_write_command(0xB7); // Entry Mode
    lcd_write_data(0x07);

    lcd_write_command(0xC0); // Power Control 1
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);

    lcd_write_command(0xC1); // Power Control 2
    lcd_write_data(0x41);

    lcd_write_command(0xC5); // VCOM Control
    lcd_write_data(0x00);

    lcd_write_command(0xE0); // Positive Gamma Control
    lcd_write_data(0x0F); lcd_write_data(0x1F); lcd_write_data(0x1C);
    lcd_write_data(0x0C); lcd_write_data(0x0F); lcd_write_data(0x08);
    lcd_write_data(0x48); lcd_write_data(0x98); lcd_write_data(0x37);
    lcd_write_data(0x0A); lcd_write_data(0x13); lcd_write_data(0x04);
    lcd_write_data(0x11); lcd_write_data(0x0D); lcd_write_data(0x00);

    lcd_write_command(0xE1); // Negative Gamma Control
    lcd_write_data(0x0F); lcd_write_data(0x32); lcd_write_data(0x2E);
    lcd_write_data(0x0B); lcd_write_data(0x0D); lcd_write_data(0x05);
    lcd_write_data(0x47); lcd_write_data(0x75); lcd_write_data(0x37);
    lcd_write_data(0x06); lcd_write_data(0x10); lcd_write_data(0x03);
    lcd_write_data(0x24); lcd_write_data(0x20); lcd_write_data(0x00);

    lcd_write_command(0x21); // Inversion ON
    lcd_write_command(0x29); // Display ON
}

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color) {
    uint16_t x1 = x0 + w - 1;
    uint16_t y1 = y0 + h - 1;

    // Set column address
    lcd_write_command(0x2A);
    lcd_write_data(x0 >> 8); lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8); lcd_write_data(x1 & 0xFF);

    // Set row address
    lcd_write_command(0x2B);
    lcd_write_data(y0 >> 8); lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8); lcd_write_data(y1 & 0xFF);

    // Write to RAM
    lcd_write_command(0x2C);

    for (uint32_t i = 0; i < w * h; i++) {
        lcd_write_color(color);
    }
}

