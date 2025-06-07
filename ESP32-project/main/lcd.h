#include "lcd.h"
#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "font.h"
#include "color.h"
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "esp32s3/rom/gpio.h"

#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialisiert alle GPIOs und das LCD (ILI9486) im 8-Bit-Parallelmodus.
 */
void lcd_init(void);

/**
 * @brief Füllt ein Rechteck auf dem Display mit einer Farbe.
 * 
 * @param x Start-X-Position
 * @param y Start-Y-Position
 * @param w Breite in Pixeln
 * @param h Höhe in Pixeln
 * @param color RGB565-Farbwert (z. B. 0xF800 für Rot)
 */
void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Gibt ein einfaches Testmuster aus (zum Debuggen)
 */
void lcd_test_pattern(void);

/**
 * @brief Manuelle GPIO-Initialisierung für das Display (falls separat benötigt)
 */
void lcd_gpio_init(void);

/**
 * @brief Schreibt ein einzelnes Datenbyte auf den Bus (für Low-Level-Funktionen)
 * @param data Das 8-Bit-Datenbyte
 */
void lcd_write_bus(uint8_t data);

/**
 * @brief Schreibt einen LCD-Befehl
 * @param cmd 8-Bit-Befehlscode
 */
void lcd_write_command(uint8_t cmd);

/**
 * @brief Schreibt LCD-Daten
 * @param data 8-Bit-Datenwert
 */
void lcd_write_data(uint8_t data);

#endif // LCD_H
