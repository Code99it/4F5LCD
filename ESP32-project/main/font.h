#pragma once
#include <stdint.h>

typedef struct {
    const uint16_t *data;
    uint8_t width;
    uint8_t height;
} FontGlyph;

extern const FontGlyph outfit24_glyphs[128];
extern const FontGlyph outfit29_glyphs[128];
extern const FontGlyph outfit36_glyphs[128];
