/*

@startuml

namespace MonoLcd {

MonoLcd.ST7565		--o			mc_interfaces.Pin
MonoLcd.ST7565		--o			mc_interfaces.SpiMaster8Bit
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
	{method}+	mc_interfaces::res	reset			( void )
	{method}+	mc_interfaces::res	setContrast	( uint8_t		val )
	{method}+	mc_interfaces::res	on			( void )
	{method}+	mc_interfaces::res	off			( void )
	{method}+	mc_interfaces::res	update		( void )
	{method}+	mc_interfaces::res	lcdClear		( void )
	{method}+	void							bufClear		( void )
	__Private methods__
	{method}-	mc_interfaces::res	comOut		( uint8_t		command );
	{method}-	mc_interfaces::res	dataOut		( uint8_t		data );
}

}

@enduml

*/

#pragma once

#include "project_config.h"

#ifdef MODULE_MONO_LCD_ST7565_ENABLED

#include "lcd_interface.h"
#include "mc_pin.h"
#include "mc_spi_master_8bit.h"
#include "user_os.h"

namespace MonoLcd {

#define        checkResultAndBreak(baseResultVariable)                                        \
                if ( baseResultVariable != mc_interfaces::res::err_ok ) {        \
                    break;                                                                \
                }

enum class ST7565_MODE {
    STANDARD = 0,
    IVERT_X = 1,
    IVERT_Y = 2,
    IVERT_X_AMD_Y = 3
};

//
// Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
//
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct ST7565Cfg {
    mc_interfaces::Pin *const a0;
    mc_interfaces::Pin *const res;
    mc_interfaces::Pin *const cs;
    mc_interfaces::SpiMaster8Bit *const s;
    const ST7565_MODE mode;
};

#define ST7565_WIDTH 128
#define ST7565_HEIGHT 64

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class ST7565 : public MonoLcd::Base {
public:
    
    ST7565 (const ST7565Cfg *const cfg,
            uint8_t *const userBuf);
    
    ~ST7565(){}
    
    mc_interfaces::res reset (void);
    
    mc_interfaces::res set_contrast (uint8_t val);
    
    mc_interfaces::res on (void);
    
    mc_interfaces::res off (void);
    
    mc_interfaces::res update (void);
    
    mc_interfaces::res lcd_clear (void);
    
    void buf_clear (void);

private:
    mc_interfaces::res comOut (uint8_t command);
    
    mc_interfaces::res dataOut (uint8_t data);

private:
    bool flagInitLcd = false;        // Инициализирован ли LCD?
    const ST7565Cfg *const cfg;
    uint8_t *const userBuf;        // Сам буффер менять можно, но указетль на него - нет.
    // Системный буфер для преобразования.
    // Дисплей едресуется вертикальными столбцами по 8 бит в высоту и 1 ширину. И так 8 строк.
    uint8_t lcdImage[128] = {0};
    // Для предотвращения попытки использовать LCD из разных потоков одновременно.
    USER_OS_STATIC_MUTEX_BUFFER mb;
    USER_OS_STATIC_MUTEX m = nullptr;
};

}

#endif
