#include "lcd.h"
#include "gpio.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_ll.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h"
#include "esp_timer.h"
#include "font.h"

#define GPIO_RS (1 << LCD_RS)
#define GPIO_WR (1 << LCD_WR)
#define GPIO_CS (1 << LCD_CS)

static const gpio_num_t lcd_data_pins[8] = {
    LCD_DATA_0, LCD_DATA_1, LCD_DATA_2, LCD_DATA_3,
    LCD_DATA_4, LCD_DATA_5, LCD_DATA_6, LCD_DATA_7
};

void lcd_gpio_write(gpio_num_t pin, bool level) {
    if (level)
        GPIO.out_w1ts = (1 << pin);
    else
        GPIO.out_w1tc = (1 << pin);
}

void lcd_init(void) {
    // Datenleitungen konfigurieren
    for (int i = 0; i < 8; i++) {
        gpio_reset_pin(lcd_data_pins[i]);
        gpio_set_direction(lcd_data_pins[i], GPIO_MODE_OUTPUT);
    }

    // Steuerleitungen konfigurieren
    gpio_reset_pin(LCD_RS);
    gpio_set_direction(LCD_RS, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LCD_WR);
    gpio_set_direction(LCD_WR, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LCD_CS);
    gpio_set_direction(LCD_CS, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LCD_RST);
    gpio_set_direction(LCD_RST, GPIO_MODE_OUTPUT);

    // Optional: RD-Leitung, wenn verwendet
#if LCD_RD >= 0
    gpio_reset_pin(LCD_RD);
    gpio_set_direction(LCD_RD, GPIO_MODE_OUTPUT);
#endif

    // Initialzustände
    lcd_gpio_write(LCD_CS, LOW);
    lcd_gpio_write(LCD_WR, HIGH);
    lcd_gpio_write(LCD_RS, HIGH);
#if LCD_RD >= 0
    lcd_gpio_write(LCD_RD, HIGH);
#endif

    // Reset-Sequenz
    lcd_gpio_write(LCD_RST, LOW);
    vTaskDelay(pdMS_TO_TICKS(20));
    lcd_gpio_write(LCD_RST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(150));
}

void lcd_driver_init(void) {
    vTaskDelay(pdMS_TO_TICKS(20));  // Power-on delay

    lcd_write_command(0x11); // Sleep OUT
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_write_command(0x3A); // Interface Pixel Format
    lcd_write_data(0x55);    // 16-bit/pixel (RGB565)

    lcd_write_command(0x36); // Memory Access Control (Rotation + BGR)
    lcd_write_data(0x48);    // MX=1, MY=0, MV=0, ML=0, BGR=1

    // Optional: Set Gamma Curve (can be omitted for basic test)
    lcd_write_command(0xF2);
    lcd_write_data(0x08);

    // Display ON
    lcd_write_command(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(20));
}

void lcd_write_bus(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        lcd_gpio_write(lcd_data_pins[i], (value >> i) & 0x01);
    }
}

static void lcd_write_bus_fast(uint8_t value) {
    // Erst alle relevanten Bits auf 0 setzen (Clear)
    GPIO.out_w1tc = ((1 << 1) | (1 << 3) | (1 << 4) | (1 << 5) |
                     (1 << 6) | (1 << 7) | (1 << 15) | (1 << 16));

    // Dann neue Bits setzen
    uint32_t mapped = 0;
    mapped |= ((value >> 0) & 1) << 1;
    mapped |= ((value >> 1) & 1) << 3;
    mapped |= ((value >> 2) & 1) << 4;
    mapped |= ((value >> 3) & 1) << 5;
    mapped |= ((value >> 4) & 1) << 6;
    mapped |= ((value >> 5) & 1) << 7;
    mapped |= ((value >> 6) & 1) << 15;
    mapped |= ((value >> 7) & 1) << 16;

    GPIO.out_w1ts = mapped;
}

void lcd_write_command(uint8_t cmd) {
    GPIO.out_w1tc = GPIO_RS;     // RS = 0
    GPIO.out_w1tc = GPIO_WR;     // WR = 0
    lcd_write_bus_fast(cmd);
    GPIO.out_w1ts = GPIO_WR;     // WR = 1
}

void lcd_write_data(uint8_t data) {
    GPIO.out_w1ts = GPIO_RS;     // RS = 1
    GPIO.out_w1tc = GPIO_WR;     // WR = 0
    lcd_write_bus_fast(data);
    GPIO.out_w1ts = GPIO_WR;     // WR = 1
}

/**
 * @brief Wandelt eine 24-Bit-Hexfarbe (0xRRGGBB) in RGB565 (16 Bit) um.
 * 
 * @param hex_color 24-Bit-Farbwert (z. B. 0xFFAA33)
 * @return RGB565-Farbwert (z. B. 0bRRRRRGGGGGGBBBBB)
 */
uint16_t hex_to_rgb565(uint32_t hex_color) {
    uint8_t r = (hex_color >> 16) & 0xFF;
    uint8_t g = (hex_color >> 8) & 0xFF;
    uint8_t b = hex_color & 0xFF;

    uint16_t r5 = (r * 31) / 255;
    uint16_t g6 = (g * 63) / 255;
    uint16_t b5 = (b * 31) / 255;

    return (r5 << 11) | (g6 << 5) | b5;
}

void lcd_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set (X)
    lcd_write_command(0x2A);
    lcd_write_data(x0 >> 8);
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);

    // Page address set (Y)
    lcd_write_command(0x2B);
    lcd_write_data(y0 >> 8);
    lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);

    // Memory Write
    lcd_write_command(0x2C);
}

void lcd_fill_color(uint16_t color) {
    const uint16_t width = 320;
    const uint16_t height = 480;
    const uint32_t total_pixels = width * height;

    lcd_set_address_window(0, 0, width - 1, height - 1);

    uint8_t high = color >> 8;
    uint8_t low = color & 0xFF;

    for (uint32_t i = 0; i < total_pixels; i++) {
        lcd_write_data(high);
        lcd_write_data(low);
    }
}

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    // Bereich setzen
    lcd_set_address_window(x0, y0, x1, y1);

    uint32_t total_pixels = (x1 - x0 + 1) * (y1 - y0 + 1);
    uint8_t high = color >> 8;
    uint8_t low = color & 0xFF;

    for (uint32_t i = 0; i < total_pixels; i++) {
        lcd_write_data(high);
        lcd_write_data(low);
    }
}

void lcd_fill_rainbow(void) {
    const uint16_t band_height = LCD_PIXEL_HEIGHT / 10;

    lcd_fill_rect(0, 0 * band_height, LCD_PIXEL_WIDTH - 1, 1 * band_height - 1, hex_to_rgb565(0xFF0000)); // Rot
    vTaskDelay(1);
    lcd_fill_rect(0, 1 * band_height, LCD_PIXEL_WIDTH - 1, 2 * band_height - 1, hex_to_rgb565(0xFF7F00)); // Orange
    vTaskDelay(1);
    lcd_fill_rect(0, 2 * band_height, LCD_PIXEL_WIDTH - 1, 3 * band_height - 1, hex_to_rgb565(0xFFFF00)); // Gelb
    vTaskDelay(1);
    lcd_fill_rect(0, 3 * band_height, LCD_PIXEL_WIDTH - 1, 4 * band_height - 1, hex_to_rgb565(0x00FF00)); // Grün
    vTaskDelay(1);
    lcd_fill_rect(0, 4 * band_height, LCD_PIXEL_WIDTH - 1, 5 * band_height - 1, hex_to_rgb565(0x00FFFF)); // Türkis
    vTaskDelay(1);
    lcd_fill_rect(0, 5 * band_height, LCD_PIXEL_WIDTH - 1, 6 * band_height - 1, hex_to_rgb565(0x0000FF)); // Blau
    vTaskDelay(1);
    lcd_fill_rect(0, 6 * band_height, LCD_PIXEL_WIDTH - 1, 7 * band_height - 1, hex_to_rgb565(0x4B0082)); // Indigo
    vTaskDelay(1);
    lcd_fill_rect(0, 7 * band_height, LCD_PIXEL_WIDTH - 1, 8 * band_height - 1, hex_to_rgb565(0x8F00FF)); // Violett
    vTaskDelay(1);
    lcd_fill_rect(0, 8 * band_height, LCD_PIXEL_WIDTH - 1, 9 * band_height - 1, hex_to_rgb565(0xFF69B4)); // Rosa
    vTaskDelay(1);
    lcd_fill_rect(0, 9 * band_height, LCD_PIXEL_WIDTH - 1, LCD_PIXEL_HEIGHT - 1, hex_to_rgb565(0xFFFFFF)); // Weiß
    vTaskDelay(1);
}

void lcd_test_fill_fps(void)
{
    const int test_frames = 10;
    int64_t start_time = esp_timer_get_time();  // in µs

    for (int i = 0; i < test_frames; i++) {
        // Wechselfarbe (optional, damit Display nicht cached)
        uint16_t color = (i % 2 == 0) ? hex_to_rgb565(0x0000FF) : hex_to_rgb565(0x00FF00);
        lcd_fill_color(color);
    }

    int64_t end_time = esp_timer_get_time();  // in µs
    int64_t elapsed_us = end_time - start_time;

    float elapsed_s = elapsed_us / 1000000.0f;
    float fps = test_frames / elapsed_s;

    printf("Elapsed time: %.2f s for %d frames\n", elapsed_s, test_frames);
    printf("FPS: %.2f\n", fps);
}


/**
 * Zeichnet ein einzelnes Zeichen anhand des ASCII-Codes.
 *
 * @param c Das Zeichen (z.B. 'Q')
 * @param x Startposition X
 * @param y Startposition Y
 *
 * @return neue Cursor-X-Position (x + Zeichenbreite)
 */
uint16_t lcd_draw_char(char c, uint16_t x, uint16_t y, uint8_t px) {
    const FontGlyph *glyph;

    switch (px) {
        case 29:
            glyph = &outfit29_glyphs[(uint8_t)c];
            break;
        case 36:
            glyph = &outfit36_glyphs[(uint8_t)c];
            break;
        default:
            return x; // unbekannte Größe – Zeichen überspringen
    }

    if (!glyph->data) {
        return x; // unbekanntes Zeichen – überspringen
    }

    lcd_set_address_window(x, y, x + glyph->width - 1, y + glyph->height - 1);

    for (uint32_t i = 0; i < glyph->width * glyph->height; i++) {
        uint16_t brightness = glyph->data[i]; // z. B. 0x0000–0xFFFF
        uint16_t modulated = scale_color_rgb565(hex_to_rgb565(0x00AAFF), brightness);
        lcd_write_data(modulated >> 8);
        lcd_write_data(modulated & 0xFF);
    }

    return x + glyph->width;
}

uint16_t scale_color_rgb565(uint16_t base_color, uint16_t brightness) {
    uint8_t r = (base_color >> 11) & 0x1F;
    uint8_t g = (base_color >> 5) & 0x3F;
    uint8_t b = base_color & 0x1F;

    r = (r * brightness) >> 16;
    g = (g * brightness) >> 16;
    b = (b * brightness) >> 16;

    return (r << 11) | (g << 5) | b;
}

/**
 * Zeichnet ein Zeichen in Farbe mit Anti-Aliasing.
 *
 * @param c        Das Zeichen (z. B. 'Q')
 * @param x        Startposition X
 * @param y        Startposition Y
 * @param px       Schriftgröße (z. B. 29 oder 36)
 * @param hex_rgb  Wunschfarbe im Format 0xRRGGBB
 *
 * @return neue Cursor-X-Position (x + Zeichenbreite)
 */
uint16_t lcd_draw_colored_char(char c, uint16_t x, uint16_t y, uint8_t px, uint32_t hex_rgb) {
    const FontGlyph *glyph;
    switch (px) {
        case 24:
            glyph = &outfit24_glyphs[(uint8_t)c];
            break;
        case 29:
            glyph = &outfit29_glyphs[(uint8_t)c];
            break;
        case 36:
            glyph = &outfit36_glyphs[(uint8_t)c];
            break;
        default:
            return x;
    }

    if (!glyph || !glyph->data)
        return x;

    uint16_t base_color = hex_to_rgb565(hex_rgb);
    lcd_set_address_window(x, y, x + glyph->width - 1, y + glyph->height - 1);

    for (uint32_t i = 0; i < glyph->width * glyph->height; i++) {
        uint16_t brightness = glyph->data[i];  // Originale Graustufe
        uint16_t modulated = scale_color_rgb565(base_color, brightness);
        lcd_write_data(modulated >> 8);
        lcd_write_data(modulated & 0xFF);
    }

    return x + glyph->width + FONT_LETTER_SPACING_PX;
}

uint16_t lcd_draw_string(const char *str, uint16_t x, uint16_t y, uint8_t px) {
    while (*str) {
        x = lcd_draw_char(*str, x, y, px);
        str++;
    }
    return x; // neue Cursor-Position nach dem String
}

uint16_t lcd_draw_colored_string(const char *str, uint16_t x, uint16_t y, uint8_t px, uint32_t hex_rgb) {
    while (*str) {
        x = lcd_draw_colored_char(*str, x, y, px, hex_rgb);
        str++;
    }
    return x;  // Neue Cursor-Position nach dem String
}

/**
 * Zeichnet einen zentrierten, farbigen String mit Anti-Aliasing.
 *
 * @param str      Der String (z. B. "Hallo Welt")
 * @param x_center Die horizontale Mitte, um die zentriert wird
 * @param y        Y-Koordinate des Textes
 * @param px       Schriftgröße (29 oder 36)
 * @param hex_rgb  Farbe in 0xRRGGBB
 */
uint16_t lcd_draw_colored_string_centered(const char *str, uint16_t x_center, uint16_t y, uint8_t px, uint32_t hex_rgb) {
    const FontGlyph *glyph;
    uint16_t total_width = 0;
    const char *ptr = str;

    while (*ptr) {
        switch (px) {
            case 29:
                glyph = &outfit29_glyphs[(uint8_t)*ptr];
                break;
            case 36:
                glyph = &outfit36_glyphs[(uint8_t)*ptr];
                break;
            case 24:
                glyph = &outfit24_glyphs[(uint8_t)*ptr];
                break;
            default:
                glyph = NULL;
                break;
        }

        if (glyph && glyph->data) {
            total_width += glyph->width + FONT_LETTER_SPACING_PX;
        }

        ptr++;
    }

    if (total_width > 0) {
        total_width -= FONT_LETTER_SPACING_PX;  // Letztes Spacing entfernen
    }

    int16_t start_x = x_center - total_width / 2;
    if (start_x < 0) start_x = 0;

    return lcd_draw_colored_string(str, (uint16_t)start_x, y, px, hex_rgb);
}


/**
 * Zeichnet eine horizontale Linie auf dem Display.
 *
 * @param y         Y-Koordinate (Oberkante der Linie)
 * @param width     Breite in Pixel (0 = volle Displaybreite)
 * @param thickness Dicke der Linie (Höhe in Pixeln)
 * @param color     RGB565-Farbe
 */
void lcd_draw_horizontal_line(uint16_t y, uint16_t width, uint16_t thickness, uint16_t color) {
    if (width == 0) {
        width = LCD_PIXEL_WIDTH;  // z. B. 320
    }

    uint16_t x0 = 0;
    uint16_t x1 = x0 + width - 1;
    uint16_t y0 = y;
    uint16_t y1 = y + thickness - 1;

    lcd_set_address_window(x0, y0, x1, y1);

    uint8_t high = color >> 8;
    uint8_t low = color & 0xFF;
    uint32_t total_pixels = width * thickness;

    for (uint32_t i = 0; i < total_pixels; i++) {
        lcd_write_data(high);
        lcd_write_data(low);
    }
}

/**
 * Misst die FPS beim Füllen eines bestimmten Bildschirmbereichs.
 *
 * @param x0 Linke X-Koordinate
 * @param y0 Obere Y-Koordinate
 * @param x1 Rechte X-Koordinate
 * @param y1 Untere Y-Koordinate
 * @param frames Anzahl Testdurchläufe
 */
void lcd_test_fill_area_fps(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t frames) {
    int64_t start_time = esp_timer_get_time();  // µs

    for (int i = 0; i < frames; i++) {
        uint32_t color = (i % 2 == 0) ? 0x0000FF : 0x00FF00;
        lcd_fill_rect(x0, y0, x1, y1, hex_to_rgb565(color));
    }

    int64_t end_time = esp_timer_get_time();
    int64_t elapsed_us = end_time - start_time;

    float elapsed_s = elapsed_us / 1000000.0f;
    float fps = frames / elapsed_s;

    printf("Filled area (%u,%u)-(%u,%u) for %u frames\n", x0, y0, x1, y1, frames);
    printf("Elapsed time: %.2f s, FPS: %.2f\n", elapsed_s, fps);
}


