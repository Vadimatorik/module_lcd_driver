#pragma once

#include <stdint.h>

struct image_t {
    const uint8_t*    const p_array;
    const uint8_t     x;
    const uint8_t     size;
    const uint8_t     b;
};

struct char_t {
    uint32_t          code;
    const image_t*    const image;
};

struct font_t {
    int               length;
    const char_t*     const chars;
};
