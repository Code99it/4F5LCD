#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

/**
 * @brief Wandelt eine 24-Bit-Hexfarbe (0xRRGGBB) in RGB565 (16 Bit) um.
 * 
 * @param hex_color 24-Bit-Farbwert (z. B. 0xFFAA33)
 * @return RGB565-Farbwert (z. B. 0bRRRRRGGGGGGBBBBB)
 */
uint16_t hex_to_rgb565(uint32_t hex_color);

#endif
