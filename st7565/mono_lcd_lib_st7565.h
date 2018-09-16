/*

@startuml

namespace MonoLcd {

MonoLcd.ST7565		--o			McHardwareInterfaces.Pin
MonoLcd.ST7565		--o			McHardwareInterfaces.SpiMaster8Bit
MonoLcd.ST7565		..|>		MonoLcd.Base

class ST7565 {
	{field}-	bool								flagInitLcd
	{field}-	const ST7565Cfg*					const cfg
	{field}-	uint8_t*							const userBuf
	{field}-	uint8_t							lcdImage[ 128 ]
	{field}-	USER_OS_STATIC_MUTEX_BUFFER			mb
	{field}-	USER_OS_STATIC_MUTEX				m
	__Constructor__
	{method}+	ST7565	( const ST7565Cfg*	const cfg,\n\t\t  uint8_t*			const userBuf )
	__Public methods__
	{method}+	McHardwareInterfaces::BaseResult	reset			( void )
	{method}+	McHardwareInterfaces::BaseResult	setContrast	( uint8_t		val )
	{method}+	McHardwareInterfaces::BaseResult	on			( void )
	{method}+	McHardwareInterfaces::BaseResult	off			( void )
	{method}+	McHardwareInterfaces::BaseResult	update		( void )
	{method}+	McHardwareInterfaces::BaseResult	lcdClear		( void )
	{method}+	void							bufClear		( void )
	__Private methods__
	{method}-	McHardwareInterfaces::BaseResult	comOut		( uint8_t		command );
	{method}-	McHardwareInterfaces::BaseResult	dataOut		( uint8_t		data );
}

}

@enduml

*/

#pragma once

#include "project_config.h"

#ifdef MODULE_MONO_LCD_ST7565_ENABLED

#include "lcd_interface.h"
#include "mc_hardware_interfaces_pin.h"
#include "mc_hardware_interfaces_spi_master_8bit.h"
#include "user_os.h"

namespace MonoLcd {

#define		checkResultAndBreak(baseResultVariable)										\
				if ( baseResultVariable != McHardwareInterfaces::BaseResult::ok ) {		\
					break;																\
				}

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
	McHardwareInterfaces::Pin*					const a0;
	McHardwareInterfaces::Pin*					const res;
	McHardwareInterfaces::Pin*					const cs;
	McHardwareInterfaces::SpiMaster8Bit*		const s;
	const ST7565_MODE							mode;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class ST7565 : public MonoLcd::Base {
public:

	ST7565 (	const ST7565Cfg*	const cfg,
				uint8_t*			const userBuf );

	McHardwareInterfaces::BaseResult	reset			(	void	);
	McHardwareInterfaces::BaseResult	setContrast		(	uint8_t		val	);
	McHardwareInterfaces::BaseResult	on				(	void 	);
	McHardwareInterfaces::BaseResult	off				(	void	);
	McHardwareInterfaces::BaseResult	update			(	void	);
	McHardwareInterfaces::BaseResult	lcdClear			(	void	);
	void								bufClear		( void );

private:
	McHardwareInterfaces::BaseResult	comOut			(	uint8_t		command	);
	McHardwareInterfaces::BaseResult	dataOut			(	uint8_t		data	);

private:
	bool								flagInitLcd			= false;		// Инициализирован ли LCD?
	const ST7565Cfg*					const cfg;
	uint8_t*							const userBuf;		// Сам буффер менять можно, но указетль на него - нет.
	// Системный буфер для преобразования.
	// Дисплей едресуется вертикальными столбцами по 8 бит в высоту и 1 ширину. И так 8 строк.
	uint8_t								lcdImage[ 128 ]	= { 0 };
	// Для предотвращения попытки использовать LCD из разных потоков одновременно.
	USER_OS_STATIC_MUTEX_BUFFER			mb;
	USER_OS_STATIC_MUTEX				m					= nullptr;
};

}

#endif
