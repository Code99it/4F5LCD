// lcd_driver.h

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bildschirmdimensionen (ggf. anpassen)
#define LCD_PX_WIDTH  320
#define LCD_PX_HEIGHT 480

/**
 * @brief Zeichnet ein einzelnes Pixel.
 *
 * @param x X-Position
 * @param y Y-Position
 * @param color RGB565-Farbwert
 */
void lcd_draw_pixel(int x, int y, uint16_t color);

/**
 * @brief Zeichnet ein Bitmap auf dem LCD.
 *
 * @param x0 Start-X-Koordinate
 * @param y0 Start-Y-Koordinate
 * @param x1 End-X-Koordinate
 * @param y1 End-Y-Koordinate
 * @param bitmap Zeiger auf Farbdaten (RGB565)
 */
void lcd_draw_bitmap(int x0, int y0, int x1, int y1, const uint16_t *bitmap);

/**
 * @brief Füllt den gesamten Bildschirm mit einer Farbe.
 *
 * @param color RGB565-Farbwert
 */
void lcd_fill_screen(uint16_t color);

#ifdef __cplusplus
}
#endif
