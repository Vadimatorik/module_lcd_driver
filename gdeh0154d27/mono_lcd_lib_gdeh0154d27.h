#pragma once

#include "project_config.h"

#ifdef MODULE_MONO_LCD_GDEH0154D27_ENABLED

#include "lcd_interface.h"
#include "mc_hardware_interfaces_pin.h"
#include "mc_hardware_interfaces_spi_master_8bit.h"
#include "user_os.h"

namespace MonoLcd {

#define		checkResultAndBreak(baseResultVariable)										\
				if ( baseResultVariable != McHardwareInterfaces::BaseResult::ok ) {		\
					break;																\
				}

//
// Парметры SPI MOTOROLA, CPHA = 1, CPOL = 1.
//
// Структура конфигурации ssd1306 в 8-ми битнос SPI режиме.
struct GDEH0154D27Cfg {
	McHardwareInterfaces::Pin*					const dc;
	McHardwareInterfaces::Pin*					const res;
	McHardwareInterfaces::Pin*					const cs;
	McHardwareInterfaces::Pin*					const busy;
	McHardwareInterfaces::SpiMaster8Bit*		const s;
};

/*
 * Любой из методов класса долен быть вызван только
 * внутри потока пользовательской операционной системы.
 */
class GDEH0154D27 : public MonoLcd::Base {
public:

	GDEH0154D27 (	const GDEH0154D27Cfg*	const cfg,
					uint8_t*				const userBuf );

	McHardwareInterfaces::BaseResult	reset			(	void	);
	McHardwareInterfaces::BaseResult	on				(	void 	);
	McHardwareInterfaces::BaseResult	off				(	void	);
	McHardwareInterfaces::BaseResult	update			(	void	);
	McHardwareInterfaces::BaseResult	lcdClear			(	void	);
	void								bufClear		( void );

private:
	McHardwareInterfaces::BaseResult 	write ( const uint8_t* const data, uint8_t dataSize );
	McHardwareInterfaces::BaseResult 	writeCmd( uint8_t command );
	McHardwareInterfaces::BaseResult 	writeCmdP1( uint8_t command, uint8_t para );
	void ReadBusy ( void );
	McHardwareInterfaces::BaseResult 	sendBuffer ( void );

	McHardwareInterfaces::BaseResult setRamArea( uint8_t Xstart,uint8_t Xend,uint8_t Ystart,uint8_t Ystart1,uint8_t Yend,uint8_t Yend1);
	McHardwareInterfaces::BaseResult setRamPointer(uint8_t addrX,uint8_t addrY,uint8_t addrY1);
	McHardwareInterfaces::BaseResult writeDispRam( uint8_t XSize, uint8_t  YSize,		uint8_t  *Dispbuff);

	McHardwareInterfaces::BaseResult 	fullUpdate(void);
private:
	bool								flagInitLcd			= false;		// Инициализирован ли LCD?
	const GDEH0154D27Cfg*				const cfg;
	uint8_t*							const userBuf;		// Сам буффер менять можно, но указетль на него - нет.
	// Для предотвращения попытки использовать LCD из разных потоков одновременно.
	USER_OS_STATIC_MUTEX_BUFFER			mb;
	USER_OS_STATIC_MUTEX				m					= nullptr;

private:
const uint8_t	xDot 	=	200;
const uint8_t	yDot =  200;

		const uint8_t	GDOControl[4]={0x01,(200-1)%256,(200-1)/256,0x00}; //for 1.54inch
		const uint8_t	softstart[4]={0x0c,0xd7,0xd6,0x9d};
		const uint8_t	Rambypass[2] = {0x21,0x8f};		// Display update
		const uint8_t	MAsequency[2] = {0x22,0xf0};		// clock
		const uint8_t	GDVol[2] = {0x03,0x00};	// Gate voltage +15V/-15V
		const uint8_t	SDVol[2] = {0x04,0x0a};	// Source voltage +15V/-15V
		const uint8_t	VCOMVol[2] = {0x2c,0x9b};	// VCOM 7c
		const uint8_t	BOOSTERFB[2] = {0xf0,0x1f};	// Source voltage +15V/-15V
		const uint8_t	DummyLine[2] = {0x3a,0x1a};	// 4 dummy line per gate
		const uint8_t	Gatetime[2] = {0x3b,0x08};	// 2us per line
		const uint8_t	BorderWavefrom[2] = {0x3c,0x33};	// Border
		const uint8_t	RamDataEntryMode[2] = {0x11,0x01};	// Ram data entry mode

		const uint8_t LUTDefault_full[31] = {
			0x32,	// command
			//C221 25C Full update waveform
		  0x50 		 ,0xAA,			0x55,			0xAA,			0x11,			0x00,
			0x00,			0x00,			0x00,			0x00,
			0x00,			0x00,			0x00,			0x00,
			0x00,			0x00,			0x00,			0x00,
			0x00,			0x00,			0xFF,			0xFF,
			0x1F,			0x00,			0x00,			0x00,
			0x00,			0x00,			0x00,			0x00			};
};

}

#endif
