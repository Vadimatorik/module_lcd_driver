#pragma once

#include "mono_lcd_lib_cfg.h"
#include "mono_lcd_lib_font_struct.h"
#include "mono_lcd_lib_rle.h"

// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct ssd1306_cfg_t {
    const pin*      const cs;
    const pin*      const res;
    const pin*      const dc;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class ssd1306 {
public:
    constexpr ssd1306 ( const ssd1306_cfg_t* const cfg, uint8_t* const buf ): cfg(cfg), buf(buf) {}

    void    reinit          ( const spi_base* spi_obj ) const;
    void    reset           ( void ) const;
    void    update          ( void ) const; 				// Обновляем LCD из буффера, в котором рисовали.
    void    buf_reset       ( void ) const;                 // Очищаем буффер.

    void    fill_rect_to_buffer (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t& color ) const;	// Рисуем закрашенный прямоугольник.
    void    draw_line_to_buffer (  uint8_t x1, uint8_t y1 ,const  uint8_t& x2, const uint8_t& y2, const uint8_t& color ) const;	// Рисуем линию (или затеряем линией).
    void    print_char_to_buffer ( const font_t& font, const uint8_t& x, const uint8_t& y, uint8_t ch, const uint8_t& color, const uint8_t& rle ) const;
    void    print_string_to_buffer ( const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t color, uint8_t rle) const;	// Целую строку.
    void    print_string_number_to_buffer ( const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t number_char, uint8_t color, uint8_t rle) const;	// Рисуем заданное колличество символов на экране.
    void    draw_frame_to_buffer ( const uint8_t& x, uint8_t& y, const uint8_t& width, const uint8_t& height, const uint8_t& thickness, const uint8_t& color) const ;	// Рисуем рамку (или стираем рамкой).
    void    fill_rect_and_frame ( const uint8_t& x, uint8_t& y, const uint8_t& width, const uint8_t& height, const uint8_t& thickness, const uint8_t& color_rect, const uint8_t& color_line ) const;	// Рисуем прямоугольник с оформлением.

private:
    void    point_set_buffer ( const uint8_t& x, const uint8_t& y ) const;
    void    point_reset_buffer ( const uint8_t& x, const uint8_t& y ) const;
    void    set_pos_to_lcd ( const uint8_t& x, const uint8_t& y ) const;

    void    set_pixel_buffer ( const uint8_t& x, const uint8_t& y, const uint8_t& color ) const;

    mutable bool flag = false;  // Инициализирован ли LCD?
    mutable const spi_base* spi = nullptr;
    const ssd1306_cfg_t* const cfg;
    uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    mutable USER_OS_STATIC_MUTEX_BUFFER     mutex_buf = USER_OS_STATIC_MUTEX_BUFFER_INIT_VALUE;
    mutable USER_OS_STATIC_MUTEX            mutex = NULL;
};
