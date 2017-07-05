#pragma once

#include <stdint.h>
#include "mono_lcd_lib_font_struct.h"
#include "mono_lcd_lib_rle.h"

/*
 * Бибилиотека для рисования простых фигур,
 * символов и строк в массиве LCD.
 */
class simple_drawing_lib {
public:
    constexpr simple_drawing_lib ( uint8_t* const buf ): buf(buf) {}

    void    point_set ( const uint8_t& x, const uint8_t& y ) const;
    void    point_reset ( const uint8_t& x, const uint8_t& y ) const;
    void    set_pixel ( const uint8_t& x, const uint8_t& y, const uint8_t& color ) const;

    void    fill_rect (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t& color ) const;	// Рисуем закрашенный прямоугольник.
    void    draw_line (  uint8_t x1, uint8_t y1 ,const  uint8_t& x2, const uint8_t& y2, const uint8_t& color ) const;	// Рисуем линию (или затеряем линией).
    void    print_char ( const font_t& font, const uint8_t& x, const uint8_t& y, uint8_t ch, const uint8_t& color, const uint8_t& rle ) const;
    void    print_string ( const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t color, uint8_t rle) const;	// Целую строку.
    void    print_string_number ( const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t number_char, uint8_t color, uint8_t rle) const;	// Рисуем заданное колличество символов на экране.
    void    draw_frame ( const uint8_t& x, uint8_t& y, const uint8_t& width, const uint8_t& height, const uint8_t& thickness, const uint8_t& color) const ;	// Рисуем рамку (или стираем рамкой).
    void    fill_rect_and_frame ( const uint8_t& x, uint8_t& y,
                                  const uint8_t& width, const uint8_t& height,
                                  const uint8_t& thickness, const uint8_t& color_rect, const uint8_t& color_line ) const;	// Рисуем прямоугольник с оформлением.

    void    clear ( void ) const;
private:
    uint8_t* const buf;

};
