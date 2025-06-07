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

#include <stdint.h>
#include "driver/gpio.h"

// LCD Data wires (should be in a continous row)
#define LCD_DATA_0 GPIO_NUM_35
#define LCD_DATA_1 GPIO_NUM_36
#define LCD_DATA_2 GPIO_NUM_37
#define LCD_DATA_3 GPIO_NUM_38
#define LCD_DATA_4 GPIO_NUM_39
#define LCD_DATA_5 GPIO_NUM_40
#define LCD_DATA_6 GPIO_NUM_41
#define LCD_DATA_7 GPIO_NUM_42

// LCD Command wires
#define LCD_RD   GPIO_NUM_14   
#define LCD_WR   GPIO_NUM_15
#define LCD_RS   GPIO_NUM_16   
#define LCD_CS   GPIO_NUM_17  
#define LCD_RST  GPIO_NUM_18

#define HIGH true
#define LOW false

#endif