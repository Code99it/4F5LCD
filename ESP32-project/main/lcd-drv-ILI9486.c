// lcd-drv-ILI9486.c

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_rom_sys.h"  // Für esp_rom_delay_us
#include "lcd-drv-ILI9486.h"
#include "gpio.h"

#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#define GPIO_RST GPIO_NUM_14
#define GPIO_CS  GPIO_NUM_15
#define GPIO_RS  GPIO_NUM_16  // Manchmal auch DC genannt
#define GPIO_WR  GPIO_NUM_17
#define GPIO_RD  GPIO_NUM_18

#endif

// Hilfsmakros zur Steuerung der LCD-Kontrollleitungen
#define LCD_RS_COMMAND() gpio_set_level(GPIO_RS, 0)
#define LCD_RS_DATA()    gpio_set_level(GPIO_RS, 1)
#define LCD_WR_LOW()     gpio_set_level(GPIO_WR, 0)
#define LCD_WR_HIGH()    gpio_set_level(GPIO_WR, 1)
#define LCD_RD_LOW()     gpio_set_level(GPIO_RD, 0)
#define LCD_RD_HIGH()    gpio_set_level(GPIO_RD, 1)
#define LCD_CS_LOW()     gpio_set_level(GPIO_CS, 0)
#define LCD_CS_HIGH()    gpio_set_level(GPIO_CS, 1)
#define LCD_RST_LOW()    gpio_set_level(GPIO_RST, 0)
#define LCD_RST_HIGH()   gpio_set_level(GPIO_RST, 1)
#define LCD_WR_STROBE()  do { LCD_WR_LOW(); esp_rom_delay_us(1); LCD_WR_HIGH(); } while (0)

// GPIOs für Datenleitungen D0–D7: GPIO35–GPIO42
static const int data_pins[8] = {
    GPIO_NUM_35, GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38,
    GPIO_NUM_39, GPIO_NUM_40, GPIO_NUM_41, GPIO_NUM_42
};

void lcd_set_data_output() {
    for (int i = 0; i < 8; i++) {
        gpio_set_direction(data_pins[i], GPIO_MODE_OUTPUT);
    }
}

void lcd_set_data_input() {
    for (int i = 0; i < 8; i++) {
        gpio_set_direction(data_pins[i], GPIO_MODE_INPUT);
    }
}

void lcd_write_bus(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        gpio_set_level(data_pins[i], (data >> i) & 0x01);
    }
}

uint8_t lcd_read_bus() {
    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        value |= gpio_get_level(data_pins[i]) << i;
    }
    return value;
}

void lcd_write_command(uint8_t cmd) {
    LCD_RS_COMMAND();
    LCD_CS_LOW();
    lcd_write_bus(cmd);
    LCD_WR_STROBE();
    LCD_CS_HIGH();
}

void lcd_write_data(uint8_t data) {
    LCD_RS_DATA();
    LCD_CS_LOW();
    lcd_write_bus(data);
    LCD_WR_STROBE();
    LCD_CS_HIGH();
}

void lcd_reset() {
    LCD_RST_LOW();
    esp_rom_delay_us(10000);
    LCD_RST_HIGH();
    esp_rom_delay_us(10000);
}

void lcd_init() {
    printf("Initializing LCD driver ILI9486\n");

    gpio_set_direction(GPIO_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RS, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_WR, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RD, GPIO_MODE_OUTPUT);

    lcd_set_data_output();
    lcd_reset();

    lcd_write_command(0x11); // Sleep out
    esp_rom_delay_us(120000);

    lcd_write_command(0x29); // Display on
    esp_rom_delay_us(2000);
}

void lcd_read_id() {
    uint8_t dummy, id[3];

    // Vorbereitung
    lcd_set_data_output();
    LCD_RS_COMMAND();
    LCD_CS_LOW();
    lcd_write_bus(0x04);   // Read Display ID
    LCD_WR_STROBE();

    // Umschalten auf Lesemodus
    lcd_set_data_input();
    LCD_RS_DATA();

    // Dummy-Read
    LCD_RD_LOW();
    esp_rom_delay_us(2);
    dummy = lcd_read_bus();
    LCD_RD_HIGH();
    esp_rom_delay_us(2);

    // Echte ID-Bytes lesen
    for (int i = 0; i < 3; i++) {
        LCD_RD_LOW();
        esp_rom_delay_us(2);
        id[i] = lcd_read_bus();
        LCD_RD_HIGH();
        esp_rom_delay_us(2);
    }

    LCD_CS_HIGH();
    lcd_set_data_output();

    printf("LCD ID: %02X %02X %02X\n", id[0], id[1], id[2]);
}

void lcd_run_diagnose() {
    struct {
        uint8_t cmd;
        const char* name;
    } diagnostics[] = {
        {0x04, "Read Display ID"},
        {0x09, "Get Status"},
        {0x0A, "Display Status"},
        {0x0D, "Read Power Mode"},
        {0x0E, "Read MADCTL"},
        {0x0F, "Read Pixel Format"},
        {0x45, "Read Write Display Brightness"},
        {0xDA, "Manufacturer ID"},
        {0xDB, "Driver Version"},
        {0xDC, "Module ID"},
    };

    uint8_t dummy, value;

    printf("\nDiagnose Start:\n");

    for (size_t i = 0; i < sizeof(diagnostics)/sizeof(diagnostics[0]); ++i) {
        LCD_RS_COMMAND();
        LCD_CS_LOW();
        lcd_write_bus(diagnostics[i].cmd);
        LCD_WR_STROBE();

        lcd_set_data_input();
        LCD_RS_DATA();

        // Dummy read
        LCD_RD_LOW();
        esp_rom_delay_us(2);
        dummy = lcd_read_bus();
        LCD_RD_HIGH();
        esp_rom_delay_us(2);

        // Echtwert lesen
        LCD_RD_LOW();
        esp_rom_delay_us(2);
        value = lcd_read_bus();
        LCD_RD_HIGH();
        esp_rom_delay_us(2);

        LCD_CS_HIGH();
        lcd_set_data_output();

        printf("%s (0x%02X): %02X\n", diagnostics[i].name, diagnostics[i].cmd, value);
    }
}

void lcd_run_full_register_scan() {
    uint8_t dummy, value;

    printf("\nVollständiger Register-Scan:\n");

    for (uint16_t cmd = 0x00; cmd <= 0xFF; cmd++) {
        // Befehl senden
        LCD_RS_COMMAND();
        LCD_CS_LOW();
        lcd_write_bus(cmd);
        LCD_WR_STROBE();

        lcd_set_data_input();
        LCD_RS_DATA();

        // Dummy read
        LCD_RD_LOW();
        esp_rom_delay_us(2);
        dummy = lcd_read_bus();
        LCD_RD_HIGH();
        esp_rom_delay_us(2);

        // Wert lesen
        LCD_RD_LOW();
        esp_rom_delay_us(2);
        value = lcd_read_bus();
        LCD_RD_HIGH();
        esp_rom_delay_us(2);

        LCD_CS_HIGH();
        lcd_set_data_output();

        // Formatierte Ausgabe: 8 Werte pro Zeile
        printf("0x%02X: %02X  ", cmd, value);
        if ((cmd & 0x07) == 0x07) {
            printf("\n");
        }
    }

    printf("\nScan abgeschlossen.\n\n");
}

void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color_rgb565) {
    uint32_t total_pixels = w * h;

    // Set column address (X)
    lcd_write_command(0x2A);
    lcd_write_data(x >> 8);
    lcd_write_data(x & 0xFF);
    lcd_write_data((x + w - 1) >> 8);
    lcd_write_data((x + w - 1) & 0xFF);

    // Set row address (Y)
    lcd_write_command(0x2B);
    lcd_write_data(y >> 8);
    lcd_write_data(y & 0xFF);
    lcd_write_data((y + h - 1) >> 8);
    lcd_write_data((y + h - 1) & 0xFF);

    // Write memory (start pixel stream)
    lcd_write_command(0x2C);

    // Write pixel data (same color for all)
    for (uint32_t i = 0; i < total_pixels; i++) {
        lcd_write_data(color_rgb565 >> 8);
        lcd_write_data(color_rgb565 & 0xFF);
    }
}

void lcd_write_rgb565(uint16_t color)
{
    lcd_write_data(color >> 8);     // High byte
    lcd_write_data(color & 0xFF);   // Low byte
}

void lcd_test_fill_red() {
    lcd_write_command(0x2A); // Column Address Set
    lcd_write_data(0x00); lcd_write_data(0x00);
    lcd_write_data(0x01); lcd_write_data(0x3F); // 319

    lcd_write_command(0x2B); // Page Address Set
    lcd_write_data(0x00); lcd_write_data(0x00);
    lcd_write_data(0x01); lcd_write_data(0xDF); // 479

    lcd_write_command(0x2C); // Memory Write

    for (int i = 0; i < 320 * 480; i++) {
        lcd_write_data(0xF8); // High byte (Red)
        lcd_write_data(0x00); // Low byte (Rest)
    }
}

