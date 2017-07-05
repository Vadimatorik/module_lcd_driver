#pragma once

#include "mono_lcd_lib_cfg.h"

#define BLACK					1
#define WHITE 					0

#define LCDWIDTH 				128
#define LCDHEIGHT 				64

#define CMD_DISPLAY_OFF   		0xAE
#define CMD_DISPLAY_ON    		0xAF

#define CMD_SET_DISP_START_LINE 0x40
#define CMD_SET_PAGE  			0xB0

#define CMD_SET_COLUMN_UPPER  	0x10
#define CMD_SET_COLUMN_LOWER  	0x00

#define CMD_SET_ADC_NORMAL  	0xA0
#define CMD_SET_ADC_REVERSE 	0xA1

#define CMD_SET_DISP_NORMAL 	0xA6
#define CMD_SET_DISP_REVERSE 	0xA7

#define CMD_SET_ALLPTS_NORMAL 	0xA4
#define CMD_SET_ALLPTS_ON  		0xA5
#define CMD_SET_BIAS_9 			0xA2
#define CMD_SET_BIAS_7 			0xA3

#define CMD_RMW  				0xE0
#define CMD_RMW_CLEAR			0xEE
#define CMD_INTERNAL_RESET  	0xE2
#define CMD_SET_COM_NORMAL  	0xC0
#define CMD_SET_COM_REVERSE  	0xC8
#define CMD_SET_POWER_CONTROL  	0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  	0x81
#define CMD_SET_VOLUME_SECOND  	0
#define CMD_SET_STATIC_OFF  	0xAC
#define CMD_SET_STATIC_ON  		0xAD
#define CMD_SET_STATIC_REG  	0x0
#define CMD_SET_BOOSTER_FIRST  	0xF8
#define CMD_SET_BOOSTER_234  	0
#define CMD_SET_BOOSTER_5  		1
#define CMD_SET_BOOSTER_6  		3
#define CMD_NOP  				0xE3
#define CMD_TEST  				0xF0

#define ST7565_STARTBYTES 		1

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
    void  reset ( uint8_t contrast ) const;
    void com_out ( uint8_t command ) const;
    void set_brightness ( uint8_t val) const;
private:

    mutable bool flag = false;  // Инициализирован ли LCD?
    mutable const spi_base* spi = nullptr;
    const mono_lcd_lib_st7565_cfg_t* const cfg;
    uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    mutable USER_OS_STATIC_MUTEX_BUFFER     mutex_buf = USER_OS_STATIC_MUTEX_BUFFER_INIT_VALUE;
    mutable USER_OS_STATIC_MUTEX            mutex = NULL;
};
