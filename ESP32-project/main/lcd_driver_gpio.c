// lcd_driver.c
#include <math.h>
#include "lcd_driver.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_rom_sys.h"
#include "pins.h" 
#include "esp_random.h"
#include "font_PT_Sans_24px_white.h"
#include "gui_config.h"


// Funktion zum Umwandeln von R, G, B in RGB565
// --------------------------------------------
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) |  // obere 5 Bit von R
           ((g & 0xFC) << 3) |  // obere 6 Bit von G
           (b >> 3);            // obere 5 Bit von B
}

// Hex-String "#rrggbb" in R, G, B zerlegen und in RGB565 umwandeln
// ----------------------------------------------------------------
uint16_t hex_to_rgb565(const char *hex) {
    if (hex[0] == '#') hex++;  // optionales '#' überspringen

    uint8_t r = strtol((char[]){hex[0], hex[1], 0}, NULL, 16);
    uint8_t g = strtol((char[]){hex[2], hex[3], 0}, NULL, 16);
    uint8_t b = strtol((char[]){hex[4], hex[5], 0}, NULL, 16);

    return rgb888_to_rgb565(r, g, b);
}

void lcd_write_bus(uint8_t data)
{
    gpio_set_level(LCD_DATA_0, (data >> 0) & 1);
    gpio_set_level(LCD_DATA_1, (data >> 1) & 1);
    gpio_set_level(LCD_DATA_2, (data >> 2) & 1);
    gpio_set_level(LCD_DATA_3, (data >> 3) & 1);
    gpio_set_level(LCD_DATA_4, (data >> 4) & 1);
    gpio_set_level(LCD_DATA_5, (data >> 5) & 1);
    gpio_set_level(LCD_DATA_6, (data >> 6) & 1);
    gpio_set_level(LCD_DATA_7, (data >> 7) & 1);
}

void lcd_write_command(uint8_t cmd)
{
    gpio_set_level(LCD_RS, 0);
    gpio_set_level(LCD_CS, 0);
    lcd_write_bus(cmd);
    gpio_set_level(LCD_WR, 0);
    gpio_set_level(LCD_WR, 1);
    gpio_set_level(LCD_CS, 1);
}

void lcd_write_data(uint8_t data)
{
    gpio_set_level(LCD_RS, 1);
    gpio_set_level(LCD_CS, 0);
    lcd_write_bus(data);
    gpio_set_level(LCD_WR, 0);
    gpio_set_level(LCD_WR, 1);
    gpio_set_level(LCD_CS, 1);
}

void lcd_write_data16(uint16_t data)
{
    lcd_write_data(data >> 8);
    lcd_write_data(data & 0xFF);
}

void lcd_reset(void)
{
    gpio_set_level(LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

void lcd_init(void)
{
    lcd_reset();

    lcd_write_command(0x11); // Sleep out
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_write_command(0x3A); // Interface Pixel Format
    lcd_write_data(0x55);    // 16 Bit/Pixel

    lcd_write_command(0x36); // Memory Access Control
    lcd_write_data(0x88);    // MX, BGR

    lcd_write_command(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(20));
}

void lcd_fill_screen(uint16_t color)
{
    lcd_fill_rect(0, 0, LCD_PX_WIDTH, LCD_PX_HEIGHT, color);
}

void lcd_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    lcd_write_command(0x2A);
    lcd_write_data(x0 >> 8);
    lcd_write_data(x0 & 0xFF);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);

    lcd_write_command(0x2B);
    lcd_write_data(y0 >> 8);
    lcd_write_data(y0 & 0xFF);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);

    lcd_write_command(0x2C);

    uint32_t pixel_count = (x1 - x0 + 1) * (y1 - y0 + 1);
    for (uint32_t i = 0; i < pixel_count; i++) {
        lcd_write_data16(color);
    }
}

void lcd_draw_image(int x0, int y0, int img_width, int img_height, const uint16_t *image_data)
{
    int draw_width  = (img_width  > (320 - x0)) ? (320 - x0) : img_width;
    int draw_height = (img_height > (480 - y0)) ? (480 - y0) : img_height;

    for (int y = 0; y < draw_height; y++) {
        lcd_write_command(0x2A);
        lcd_write_data((x0 >> 8) & 0xFF);
        lcd_write_data(x0 & 0xFF);
        lcd_write_data(((x0 + draw_width - 1) >> 8) & 0xFF);
        lcd_write_data((x0 + draw_width - 1) & 0xFF);

        lcd_write_command(0x2B);
        lcd_write_data(((y0 + y) >> 8) & 0xFF);
        lcd_write_data((y0 + y) & 0xFF);
        lcd_write_data(((y0 + y) >> 8) & 0xFF);
        lcd_write_data((y0 + y) & 0xFF);

        lcd_write_command(0x2C);

        const uint16_t *row = &image_data[y * img_width];
        for (int x = 0; x < draw_width; x++) {
            lcd_write_data16(row[x]);
        }
    }
}

// Pixel zeichnen
void lcd_write_pixel(int x, int y, uint16_t color)
{
    lcd_write_command(0x2A);
    lcd_write_data(x >> 8);
    lcd_write_data(x & 0xFF);
    lcd_write_data(x >> 8);
    lcd_write_data(x & 0xFF);

    lcd_write_command(0x2B);
    lcd_write_data(y >> 8);
    lcd_write_data(y & 0xFF);
    lcd_write_data(y >> 8);
    lcd_write_data(y & 0xFF);

    lcd_write_command(0x2C);
    lcd_write_data16(color);
}

//Farbmischfunktion
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

// Einen Kreisring zeichnen
void draw_circle_ring(int cx, int cy, float radius, float stroke_width, uint16_t color) {
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

                // Hintergrundfarbe schwarz (#000000)
                uint16_t blended = blend_color(color, 0x0000, alpha);
                lcd_write_pixel(cx + x, cy + y, blended);
            }
        }
    }
}

#include <math.h>  // für sinf, cosf, atan2f, M_PI

/**
 * Zeichnet einen anti-aliased Kreisbogen mit definierter Strichbreite und Farbe.
 *
 * @param cx               X-Koordinate des Kreismittelpunkts
 * @param cy               Y-Koordinate des Kreismittelpunkts
 * @param radius           Radius des Bogens (zur Mitte der Linienbreite)
 * @param stroke_width     Breite der Linie (in Pixel)
 * @param start_angle_deg  Startwinkel in Grad (0° = rechts, 90° = oben, 180° = links, 270° = unten)
 * @param end_angle_deg    Endwinkel in Grad (0–360, Richtung: mathematisch positiv, gegen den Uhrzeigersinn)
 * @param color            Farbe im RGB565-Format (z. B. aus hex_to_rgb565())
 */
void draw_arc(int cx, int cy, float radius, float stroke_width,
              float start_angle_deg, float end_angle_deg, uint16_t color)
{
    float r_outer = radius + stroke_width / 2.0f;
    float r_inner = radius - stroke_width / 2.0f;

    // Grad → Bogenmaß
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

            // Gültiger Winkelbereich?
            bool inside_angle;
            if (start_rad < end_rad)
                inside_angle = (angle >= start_rad && angle <= end_rad);
            else
                inside_angle = (angle >= start_rad || angle <= end_rad);

            if (!inside_angle)
                continue;

            // Anti-Aliasing
            float alpha = 1.0f;
            if (dist < r_inner)
                alpha = dist - (r_inner - 1.0f);
            else if (dist > r_outer)
                alpha = (r_outer + 1.0f) - dist;

            if (alpha < 0.0f) alpha = 0.0f;
            if (alpha > 1.0f) alpha = 1.0f;

            uint16_t blended = blend_color(color, 0x0000 /* Hintergrundfarbe */, alpha);
            lcd_write_pixel(cx + x, cy + y, blended);
        }
    }
}


// Footer mit schwarz füllen und quasi löschen
void lcd_flush_footer() {
    lcd_fill_rect(
    0,
    LCD_PX_HEIGHT - GUI_FOOTER_HEIGHT + 1,
    LCD_PX_WIDTH,
    LCD_PX_HEIGHT,
    hex_to_rgb565("#000000")
    );
}

// Header mit schwarz füllen und quasi löschen
void lcd_flush_header() {
    lcd_fill_rect(
    0,
    0,
    LCD_PX_WIDTH - 76, // remove 50 after dev process
    GUI_HEADER_HEIGHT - 1,
    hex_to_rgb565("#000000")
    );
}

// Einfache UTF-8-Dekodierung für 1- und 2-Byte-Zeichen
static uint16_t utf8_next(const char **p) {
    const unsigned char *s = (const unsigned char *)*p;
    uint16_t code;

    if (s[0] < 0x80) {
        code = s[0];
        *p += 1;
    } else if ((s[0] & 0xE0) == 0xC0) {
        code = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        *p += 2;
    } else {
        // ungültig oder 3+ Byte UTF-8 – wird ignoriert
        code = '?';
        *p += 1;
    }

    return code;
}



// Misst die breite eines auszugebenden Strings in Pixeln
int lcd_measure_string(const char *text) {
    int width = 0;
    int char_count = 0;

    const char *p = text;
    while (*p) {
        uint16_t ch = utf8_next(&p);

        const FontChar *found = NULL;
        for (int i = 0; i < font_std_len; i++) {
            if (font_std[i].codepoint == ch) {
                found = &font_std[i];
                break;
            }
        }

        if (found) {
            width += found->width;
            char_count++;
        }
    }

    if (char_count > 0) {
        width += (char_count - 1) * GUI_FONT_LETTER_SPACING;
        width += GUI_GAP_KEY_VALUE;
    }

    return width;
}

int lcd_write_string(int x, int y, const char *text) {
    int cursor_x = x;

    const char *p = text;
    while (*p) {
        uint16_t ch = utf8_next(&p);

        const FontChar *glyph = NULL;
        for (int i = 0; i < font_std_len; i++) {
            if (font_std[i].codepoint == ch) {
                glyph = &font_std[i];
                break;
            }
        }

        if (!glyph) continue;

        for (int row = 0; row < glyph->height; row++) {
            for (int col = 0; col < glyph->width; col++) {
                int draw_x = cursor_x + col;
                int draw_y = y + row;

                if (draw_x >= 0 && draw_x < LCD_PX_WIDTH && draw_y >= 0 && draw_y < LCD_PX_HEIGHT) {
                    uint16_t color = ((const uint16_t *)glyph->bitmap)[row * glyph->width + col];

                    // Transparente Pixel (optional: wenn 0x0000 z.B. als „nicht zeichnen“ interpretiert wird)
                    //& if (color == 0x0000) continue;

                    lcd_write_command(0x2A);
                    lcd_write_data(draw_x >> 8);
                    lcd_write_data(draw_x & 0xFF);
                    lcd_write_data(draw_x >> 8);
                    lcd_write_data(draw_x & 0xFF);

                    lcd_write_command(0x2B);
                    lcd_write_data(draw_y >> 8);
                    lcd_write_data(draw_y & 0xFF);
                    lcd_write_data(draw_y >> 8);
                    lcd_write_data(draw_y & 0xFF);

                    lcd_write_command(0x2C);
                    lcd_write_data16(color);
                }
            }
        }

        cursor_x += glyph->width + GUI_FONT_LETTER_SPACING;
    }

    return cursor_x;
}

int lcd_write_string_colored(int x, int y, const char *text, uint16_t text_color) {
    int cursor_x = x;

    const char *p = text;
    while (*p) {
        uint16_t ch = utf8_next(&p);

        const FontChar *glyph = NULL;
        for (int i = 0; i < font_std_len; i++) {
            if (font_std[i].codepoint == ch) {
                glyph = &font_std[i];
                break;
            }
        }

        if (!glyph) continue;

        for (int row = 0; row < glyph->height; row++) {
            for (int col = 0; col < glyph->width; col++) {
                int draw_x = cursor_x + col;
                int draw_y = y + row;

                if (draw_x >= 0 && draw_x < LCD_PX_WIDTH && draw_y >= 0 && draw_y < LCD_PX_HEIGHT) {
                    uint16_t pix = ((const uint16_t *)glyph->bitmap)[row * glyph->width + col];

                    if (pix == 0x0000) continue; // Transparent

                    // Helligkeit aus Pixel ableiten (Graustufen aus 0x0000 - 0xFFFF)
                    float brightness = pix / 65535.0f;  // 0.0 bis 1.0

                    uint16_t blended = blend_color(text_color, 0x0000, brightness);

                    lcd_write_command(0x2A);
                    lcd_write_data(draw_x >> 8);
                    lcd_write_data(draw_x & 0xFF);
                    lcd_write_data(draw_x >> 8);
                    lcd_write_data(draw_x & 0xFF);

                    lcd_write_command(0x2B);
                    lcd_write_data(draw_y >> 8);
                    lcd_write_data(draw_y & 0xFF);
                    lcd_write_data(draw_y >> 8);
                    lcd_write_data(draw_y & 0xFF);

                    lcd_write_command(0x2C);
                    lcd_write_data16(blended);
                }
            }
        }

        cursor_x += glyph->width + GUI_FONT_LETTER_SPACING;
    }

    return cursor_x;
}




