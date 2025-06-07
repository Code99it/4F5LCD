#ifndef COLOR_HELPERS_H
#define COLOR_HELPERS_H

#include <stdint.h>
#include <string.h>
#include <stddef.h>

uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
uint16_t hex_to_rgb565(const char *hex);
uint16_t blend_color(uint16_t fg, uint16_t bg, float alpha);

#endif