#pragma once

#include "stdint.h"
#include "stdlib.h"

// Класс для расшифровки RLE-сжатых шрифтов.
// Созщдается в стеке каждый раз, когда требуется расшифровать RLE/
class mono_lcd_lib_rle {
public:
    mono_lcd_lib_rle ( const uint8_t* p ): p(p) {}
    uint8_t get_byte ( void );

private:
    const uint8_t*    p;                // Указываем 0-й байт символа, с которого начнется расшифровка.
    uint8_t     rle_loop = 0;			// Колличество повторний.
    bool        flag_rle = false;
};
