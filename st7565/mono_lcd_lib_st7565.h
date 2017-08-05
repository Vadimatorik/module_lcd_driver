#pragma once

#include "lcd_interface.h"
#include "mk_hardware_interfaces_pin.h"

/*
 * Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
 */
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct mono_lcd_lib_st7565_cfg_t {
    const pin_base*                 const a0;
    const pin_base*                 const res;
    const pin_base*                 const cs;
          spi_master_8bit_base*     const p_spi;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class mono_lcd_lib_st7565 : public mono_lcd_128x64_base {
public:
    constexpr mono_lcd_lib_st7565 ( const mono_lcd_lib_st7565_cfg_t* const cfg, uint8_t* const buf ): cfg(cfg), buf(buf) {}

    void    reinit          ( void ) const;
    void    reset           ( void ) const;
    void    set_contrast    ( uint8_t val) const;
    void    on              ( void ) const;
    void    off             ( void ) const;

    void    update          ( void ) const;
    void    clear           ( void ) const;


    void    buf_clear       ( void ) const;

private:
    void    com_out         ( uint8_t command ) const;
    void    data_out        ( uint8_t data ) const;

    mutable bool flag = false;  // Инициализирован ли LCD?
    const mono_lcd_lib_st7565_cfg_t* const cfg;
    uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    mutable USER_OS_STATIC_MUTEX_BUFFER     mutex_buf = USER_OS_STATIC_MUTEX_BUFFER_INIT_VALUE;
    mutable USER_OS_STATIC_MUTEX            mutex = NULL;
};
