#pragma once

#include "lcd_interface.h"
#include "mc_hardware_interfaces_pin.h"
#include "mc_hardware_interfaces_spi.h"
#include "user_os.h"

//
// Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
//
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct mono_lcd_lib_ssd1306_cfg_t {
	const pin_base*					const cs;
	const pin_base*					const res;
	const pin_base*					const dc;
			spi_master_8bit_base*	const p_spi;
};

//
// Любой из методов класса долен быть вызван только
// внутри потока пользовательской операционной системы.
//
class mono_lcd_lib_ssd1306 : public mono_lcd_128x64_base {
public:
	mono_lcd_lib_ssd1306 ( const mono_lcd_lib_ssd1306_cfg_t* const cfg, uint8_t* const buf );

	BASE_RESULT	reset			( void ) const;
	BASE_RESULT	update			( void ) const; 				// Обновляем LCD из буффера, в котором рисовали.
	BASE_RESULT	on				( void ) const;
	BASE_RESULT	off				( void ) const;

	void	buf_clear			( void ) const;				 // Очищаем буффер.

private:
	BASE_RESULT set_pos_to_lcd ( const uint8_t& x, const uint8_t& y ) const;

	mutable bool flag = false;	// Инициализирован ли LCD?
	const mono_lcd_lib_ssd1306_cfg_t* const cfg;
	uint8_t* const buf; // Сам буффер менять можно, но указетль на него - нет.
	// Для предотвращения попытки использовать LCD из разных потоков одновременно.
	mutable USER_OS_STATIC_MUTEX_BUFFER	 mutex_buf;
	mutable USER_OS_STATIC_MUTEX			mutex = nullptr;
};
