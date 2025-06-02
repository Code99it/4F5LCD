/**
 * @file gpio.h
 * 
 * @brief GPIO pin definitions for 8-bit parallel LCD interface (ESP32-S3)
 * 
 * @author Christian Wilhelm (cw1@code99.it)
 * @version 1.0.1
 * @date 2025-06-02
 *
 * This file defines the GPIO mappings for data and control signals
 * used to connect an 8-bit parallel LCD to the ESP32-S3. All selected
 * pins are safe to use — they are not reserved for flash, USB, or
 * bootstrapping, and are fully output-capable.
 * 
 */

#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"

// LCD Data wires (safe GPIOs for 8-bit parallel interface)
#define LCD_DATA_0 GPIO_NUM_1
#define LCD_DATA_1 GPIO_NUM_3
#define LCD_DATA_2 GPIO_NUM_4
#define LCD_DATA_3 GPIO_NUM_5
#define LCD_DATA_4 GPIO_NUM_6
#define LCD_DATA_5 GPIO_NUM_7
#define LCD_DATA_6 GPIO_NUM_15
#define LCD_DATA_7 GPIO_NUM_16

// LCD Command wires (safe GPIOs for control signals)
#define LCD_RD   GPIO_NUM_13   // Optional, set to -1 if unused
#define LCD_WR   GPIO_NUM_17
#define LCD_RS   GPIO_NUM_9    // Also called D/C or A0
#define LCD_CS   GPIO_NUM_10   // Can be tied to GND if not shared
#define LCD_RST  GPIO_NUM_11

#define HIGH true
#define LOW false

#endif