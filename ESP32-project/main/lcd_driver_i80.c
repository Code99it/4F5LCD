// lcd_driver_i80.c
// --------------------------------------------------------------------------------------------------
// Diese Datei enthält alle Funktionen zur Ansteuerung eines LCD-Displays im I80-Modus mit ESP-IDF.
// Sie nutzt die ESP-IDF-eigene esp_lcd-API, um ein paralleles 8-Bit-Display (z. B. ILI9486) effizient
// zu betreiben. Zusätzlich sind Funktionen zur Darstellung von Texten mit Bitmap-Fonts enthalten.
// --------------------------------------------------------------------------------------------------

#include "lcd_driver_i80.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pins.h"
#include "font_PT_Sans_24px_white.h"
#include <string.h>
#include <math.h>
#include "esp_heap_caps.h"
#include "gui_config.h"
#include <stdlib.h>

static const char *TAG = "lcd_i80";
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_i80_bus_handle_t i80_bus = NULL;
static bool i80_initialized = false;

esp_lcd_panel_handle_t lcd_i80_get_handle(void) {
    return panel_handle;
}

// Funktion zum Umwandeln von R, G, B in RGB565
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Hex-String "#rrggbb" in R, G, B zerlegen und in RGB565 umwandeln
uint16_t hex_to_rgb565(const char *hex) {
    if (hex[0] == '#') hex++;
    uint8_t r = strtol((char[]){hex[0], hex[1], 0}, NULL, 16);
    uint8_t g = strtol((char[]){hex[2], hex[3], 0}, NULL, 16);
    uint8_t b = strtol((char[]){hex[4], hex[5], 0}, NULL, 16);
    return rgb888_to_rgb565(r, g, b);
}

// Farbmischfunktion
uint16_t blend_color(uint16_t fg, uint16_t bg, float alpha) {
    uint8_t rf = (fg >> 11) & 0x1F;
    uint8_t gf = (fg >> 5) & 0x3F;
    uint8_t bf = fg & 0x1F;
    uint8_t rb = (bg >> 11) & 0x1F;
    uint8_t gb = (bg >> 5) & 0x3F;
    uint8_t bb = bg & 0x1F;
    uint8_t r = rf * alpha + rb * (1 - alpha);
    uint8_t g = gf * alpha + gb * (1 - alpha);
    uint8_t b = bf * alpha + bb * (1 - alpha);
    return (r << 11) | (g << 5) | b;
}

void lcd_i80_draw_pixel(int x, int y, uint16_t color) {
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &color);
}

void lcd_i80_init(void) {
    if (i80_initialized) {
        ESP_LOGI(TAG, "Bereits initialisiert – überspringe");
        return;
    }

    esp_lcd_panel_io_handle_t io_handle = NULL;

    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = LCD_RS,
        .wr_gpio_num = LCD_WR,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            LCD_DATA_0, LCD_DATA_1, LCD_DATA_2, LCD_DATA_3,
            LCD_DATA_4, LCD_DATA_5, LCD_DATA_6, LCD_DATA_7
        },
        .bus_width = 8,
        .max_transfer_bytes = LCD_PX_WIDTH * LCD_PX_HEIGHT * 2,
    };
    esp_err_t err = esp_lcd_new_i80_bus(&bus_config, &i80_bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i80-Bus konnte nicht erstellt werden: %s", esp_err_to_name(err));
        return;
    }

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = LCD_CS,
        .pclk_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    i80_initialized = true;
}

/**
 * @brief Füllt den kompletten Bildschirm mit einer Farbe.
 * 
 * @param color Die 16-Bit-Farbe im RGB565-Format.
 * @return void
 */
void lcd_i80_fill_screen(uint16_t color)
{
    // Speicher im PSRAM für das gesamte Display reservieren
    uint16_t *fill_buffer = heap_caps_malloc(LCD_PX_WIDTH * LCD_PX_HEIGHT * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (!fill_buffer) {
        ESP_LOGE(TAG, "Kein Speicher für fill_buffer");
        return;
    }

    // Füllfarbe in alle Pixel schreiben
    for (int i = 0; i < LCD_PX_WIDTH * LCD_PX_HEIGHT; i++) {
        fill_buffer[i] = color;
    }

    // Bitmap auf Display zeichnen
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_PX_WIDTH, LCD_PX_HEIGHT, fill_buffer);
    heap_caps_free(fill_buffer);
}

/**
 * @brief Zeichnet ein Bitmap an eine bestimmte Position auf dem Bildschirm.
 * 
 * @param x0 Start-X-Koordinate (links oben)
 * @param y0 Start-Y-Koordinate (links oben)
 * @param x1 End-X-Koordinate (rechts unten)
 * @param y1 End-Y-Koordinate (rechts unten)
 * @param bitmap Zeiger auf das Farbarray im RGB565-Format
 * @return void
 */
void lcd_i80_draw_bitmap(int x0, int y0, int x1, int y1, const uint16_t *bitmap)
{
    esp_lcd_panel_draw_bitmap(panel_handle, x0, y0, x1, y1, bitmap);
}

/**
 * @brief Berechnet die Breite eines Strings in Pixeln (basierend auf Fontbreite + Zeichenabstand).
 * 
 * @param text Der darzustellende String
 * @return Breite in Pixel
 */
int lcd_i80_measure_string(const char *text)
{
    if (!text) return 0;
    int len = strlen(text);
    return len * 16 + (len - 1) * GUI_FONT_LETTER_SPACING;
}

/**
 * @brief Zeichnet einen String farbig auf das Display mit variabler Zeichenbreite.
 * 
 * @param x Start-X-Position in Pixeln
 * @param y Start-Y-Position in Pixeln
 * @param text Textinhalt als Null-terminierter String
 * @param text_color Textfarbe im RGB565-Format
 * @return Breite des gezeichneten Textes in Pixeln
 */
int lcd_i80_write_string_colored(int x, int y, const char *text, uint16_t text_color)
{
    int cursor = x;
    while (*text) {
        const uint16_t *bitmap = font_PT_Sans_24px_white_get_char(*text);
        int char_width = font_PT_Sans_24px_white_get_char_width(*text);
        if (bitmap && char_width > 0) {
            uint16_t colored[char_width * GUI_FONT_1_HEIGHT];
            for (int i = 0; i < char_width * GUI_FONT_1_HEIGHT; i++) {
                colored[i] = (bitmap[i] > 0) ? text_color : 0x0000;
            }
            lcd_i80_draw_bitmap(cursor, y, cursor + char_width, y + GUI_FONT_1_HEIGHT, colored);
            cursor += char_width + GUI_FONT_LETTER_SPACING;
        }
        text++;
    }
    return cursor - x;
}

/**
 * @brief Zeichnet einen String in weißer Farbe auf das Display.
 * 
 * @param x Start-X-Position in Pixeln
 * @param y Start-Y-Position in Pixeln
 * @param text Textinhalt als Null-terminierter String
 * @return Breite des gezeichneten Textes in Pixeln
 */
int lcd_i80_write_string(int x, int y, const char *text)
{
    return lcd_i80_write_string_colored(x, y, text, 0xFFFF);
}

// Füllt den Footer-Bereich mit Schwarz, um ihn zu löschen
void lcd_i80_flush_footer(void)
{
    uint16_t black = 0x0000; // Schwarz im RGB565-Format
    int y_start = LCD_PX_HEIGHT - GUI_FOOTER_HEIGHT + 1;
    int y_end = LCD_PX_HEIGHT;

    // Rechteckzeile für Zeile-weise füllen vorbereiten
    uint16_t line[LCD_PX_WIDTH];
    for (int i = 0; i < LCD_PX_WIDTH; i++) {
        line[i] = black;
    }

    for (int y = y_start; y <= y_end; y++) {
        lcd_i80_draw_bitmap(0, y, LCD_PX_WIDTH, y + 1, line);
    }
}

// Füllt den Header-Bereich mit Schwarz, um ihn zu löschen
void lcd_i80_flush_header(void)
{
    uint16_t black = 0x0000; // Schwarz im RGB565-Format
    int y_start = 0;
    int y_end = GUI_HEADER_HEIGHT - 1;
    int x_end = LCD_PX_WIDTH - 76; // Hinweis: -76 statt -50 nach dev phase

    // Rechteckzeile vorbereiten
    uint16_t line[x_end];
    for (int i = 0; i < x_end; i++) {
        line[i] = black;
    }

    for (int y = y_start; y <= y_end; y++) {
        lcd_i80_draw_bitmap(0, y, x_end, y + 1, line);
    }
}

void draw_i80_arc(int cx, int cy, float radius, float stroke_width,
                  float start_angle_deg, float end_angle_deg, uint16_t color)
{
    float r_outer = radius + stroke_width / 2.0f;
    float r_inner = radius - stroke_width / 2.0f;

    float start_rad = start_angle_deg * M_PI / 180.0f;
    float end_rad   = end_angle_deg   * M_PI / 180.0f;

    for (int y = -r_outer - 1; y <= r_outer + 1; y++) {
        for (int x = -r_outer - 1; x <= r_outer + 1; x++) {
            float fx = (float)x;
            float fy = (float)y;
            float dist = sqrtf(fx * fx + fy * fy);

            // Innerhalb des Ringbereichs?
            if (dist < r_inner - 1.0f || dist > r_outer + 1.0f)
                continue;

            // Winkelberechnung in 0–2π
            float angle = atan2f(fy, fx);
            if (angle < 0) angle += 2.0f * M_PI;

            bool inside_angle;
            if (start_rad < end_rad)
                inside_angle = (angle >= start_rad && angle <= end_rad);
            else
                inside_angle = (angle >= start_rad || angle <= end_rad);

            if (!inside_angle)
                continue;

            // Anti-Aliasing: Alphamischung mit Hintergrund
            float alpha = 1.0f;
            if (dist < r_inner)
                alpha = dist - (r_inner - 1.0f);
            else if (dist > r_outer)
                alpha = (r_outer + 1.0f) - dist;

            if (alpha < 0.0f) alpha = 0.0f;
            if (alpha > 1.0f) alpha = 1.0f;

            uint16_t blended = blend_color(color, 0x0000, alpha);
            lcd_i80_draw_pixel(cx + x, cy + y, blended);
        }
    }
}

void draw_i80_circle_ring(int cx, int cy, float radius, float stroke_width, uint16_t color)
{
    float r_outer = radius + stroke_width / 2.0f;
    float r_inner = radius - stroke_width / 2.0f;

    for (int y = -r_outer - 1; y <= r_outer + 1; y++) {
        for (int x = -r_outer - 1; x <= r_outer + 1; x++) {
            float dist = sqrtf(x * x + y * y);

            if (dist >= r_inner - 1.0f && dist <= r_outer + 1.0f) {
                float alpha = 1.0f;

                if (dist < r_inner)
                    alpha = dist - (r_inner - 1.0f);
                else if (dist > r_outer)
                    alpha = (r_outer + 1.0f) - dist;

                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;

                uint16_t blended = blend_color(color, 0x0000 /* schwarz */, alpha);
                lcd_i80_draw_pixel(cx + x, cy + y, blended);
            }
        }
    }
}


void lcd_i80_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    if (!panel_handle) return;

    uint16_t width = x1 - x0 + 1;
    uint16_t height = y1 - y0 + 1;
    uint32_t pixel_count = width * height;

    // Speicher für das Rechteck erzeugen
    uint16_t *buffer = malloc(pixel_count * sizeof(uint16_t));
    if (!buffer) return;

    // Alles mit der Farbe füllen
    for (uint32_t i = 0; i < pixel_count; i++) {
        buffer[i] = color;
    }

    // In einem Rutsch zeichnen
    esp_lcd_panel_draw_bitmap(panel_handle, x0, y0, x1 + 1, y1 + 1, buffer);

    free(buffer);
}

