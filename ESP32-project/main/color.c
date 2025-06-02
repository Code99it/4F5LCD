#include "color.h"

uint16_t hex_to_rgb565(uint32_t hex_color) {
    uint8_t r = (hex_color >> 16) & 0xFF;
    uint8_t g = (hex_color >> 8) & 0xFF;
    uint8_t b = hex_color & 0xFF;

    uint16_t r5 = (r * 31) / 255;
    uint16_t g6 = (g * 63) / 255;
    uint16_t b5 = (b * 31) / 255;

    return (r5 << 11) | (g6 << 5) | b5;
}
