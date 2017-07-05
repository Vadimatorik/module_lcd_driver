#pragma once

#include "lcd_interface.h"

/*
 * Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
 */
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct mono_lcd_lib_ssd1306_cfg_t {
    const pin*      const cs;
    const pin*      const res;
    const pin*      const dc;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class mono_lcd_lib_ssd1306 : public mono_lcd_128x64_base {
public:
    constexpr mono_lcd_lib_ssd1306 ( const mono_lcd_lib_ssd1306_cfg_t* const cfg, uint8_t* const buf ): cfg(cfg), buf(buf) {}

    void    reinit          ( const spi_base* spi_obj ) const;
    void    reset           ( void ) const;
    void    update          ( void ) const; 				// Обновляем LCD из буффера, в котором рисовали.
    void    buf_clear       ( void ) const;                 // Очищаем буффер.
    void    on ( void ) const;
    void    off ( void ) const;

private:
    void    set_pos_to_lcd ( const uint8_t& x, const uint8_t& y ) const;

    void    set_pixel_buffer ( const uint8_t& x, const uint8_t& y, const uint8_t& color ) const;

    mutable bool flag = false;  // Инициализирован ли LCD?
    mutable const spi_base* spi = nullptr;
    const mono_lcd_lib_ssd1306_cfg_t* const cfg;
    uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    mutable USER_OS_STATIC_MUTEX_BUFFER     mutex_buf = USER_OS_STATIC_MUTEX_BUFFER_INIT_VALUE;
    mutable USER_OS_STATIC_MUTEX            mutex = NULL;
};
