#pragma once

#ifdef MODULE_MONO_LCD_ST7565_ENABLED

#include "lcd_interface.h"
#include "mc_hardware_interfaces_pin.h"
#include "mc_hardware_interfaces_spi.h"
#include "user_os.h"

enum class ST7565_MODE {
	STANDARD		= 0,
	IVERT_X			= 1,
	IVERT_Y			= 2,
	IVERT_X_AMD_Y	= 3
};

//
// Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
//
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct ST7565Cfg {
	PinBase*					const a0;
	PinBase*					const res;
	PinBase*					const cs;
	SpiMaster8BitBase*			const s;
	const ST7565_MODE			mode;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class ST7565 : public monoLcd128x64Base {
public:

	ST7565 ( const ST7565Cfg* const cfg, uint8_t* const userBuf );

	BaseResult	reset				( void );
	BaseResult	setContrast			( uint8_t val );
	BaseResult	on					( void );
	BaseResult	off					( void );

	BaseResult	update				( void );
	BaseResult	clear				( void );


	void		bufClear			( void );

private:
	BaseResult com_out				( uint8_t command );
	BaseResult data_out			( uint8_t data );

	bool flag = false;			// Инициализирован ли LCD?
	const ST7565Cfg* const cfg;
	uint8_t*	const userBuf;		 // Сам буффер менять можно, но указетль на него - нет.
	// Системный буфер для преобразования.
	// Дисплей едресуется вертикальными столбцами по 8 бит в высоту и 1 ширину. И так 8 строк.
	uint8_t	 system_buf[ 128 ] = { 0 };
	// Для предотвращения попытки использовать LCD из разных потоков одновременно.
	USER_OS_STATIC_MUTEX_BUFFER			mutex_buf;
	USER_OS_STATIC_MUTEX				mutex = NULL;
};

#endif
