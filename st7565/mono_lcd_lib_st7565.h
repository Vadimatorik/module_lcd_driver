#pragma once

#include "mono_lcd_lib_cfg.h"

/*
 * Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
 */
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct mono_lcd_lib_st7565_cfg_t {
    const pin*      const a0;
    const pin*      const res;
    const pin*      const cs;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class mono_lcd_lib_st7565 {
public:
    constexpr mono_lcd_lib_st7565 ( const mono_lcd_lib_st7565_cfg_t* const cfg, uint8_t* const buf ): cfg(cfg), buf(buf) {}

    void    reinit          ( const spi_base* spi_obj ) const;

private:

    mutable bool flag = false;  // Инициализирован ли LCD?
    mutable const spi_base* spi = nullptr;
    const mono_lcd_lib_st7565_cfg_t* const cfg;
    uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    mutable USER_OS_STATIC_MUTEX_BUFFER     mutex_buf = USER_OS_STATIC_MUTEX_BUFFER_INIT_VALUE;
    mutable USER_OS_STATIC_MUTEX            mutex = NULL;
};
