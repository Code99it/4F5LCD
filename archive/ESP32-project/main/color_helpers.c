#include <stdint.h>
#include <string.h>
#include <stddef.h>

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