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
	McHardwareInterfaces::BaseResult	comOut								(	uint8_t				command	);
	McHardwareInterfaces::BaseResult	dataOut								(	uint8_t				data	);

	McHardwareInterfaces::BaseResult	sendCommandDriverOutputControl		(	uint16_t			a,
																				uint8_t				b		);
	McHardwareInterfaces::BaseResult	sendCommandSetDummyLinePeriod		(	uint8_t				dummyLinePeriod	);
	McHardwareInterfaces::BaseResult	sendCommandSetGateLineWidth			(	uint8_t				gateLineWidth	);

	McHardwareInterfaces::BaseResult	sendCommandDataEntryModeSetting		(	uint8_t				dataEntryModeSetting	);

	McHardwareInterfaces::BaseResult	sendCommandSetRAMxAddress			(	void	);
	McHardwareInterfaces::BaseResult	sendCommandSetRAMyAddress			(	void	);

	McHardwareInterfaces::BaseResult	sendCommandWriteVcomRegister		(	uint8_t				vcom	);
	McHardwareInterfaces::BaseResult	sendCommandWriteLutRegister			(	const uint8_t		lutRegister[ 30 ]	);

	McHardwareInterfaces::BaseResult	sendCommandSetRamXAddressCounter	(	uint8_t				ramXAddressCounter	);
	McHardwareInterfaces::BaseResult	sendCommandSetRamYAddressCounter	(	uint16_t			ramYAddressCounter	);

	McHardwareInterfaces::BaseResult	sendBuffer							(	void	);

	McHardwareInterfaces::BaseResult	sendCommandDisplayUpdateControl2	(	uint8_t				arg	);

	McHardwareInterfaces::BaseResult	sendCommandMasterActivation			(	void	);

private:
	bool								flagInitLcd			= false;		// Инициализирован ли LCD?
	const GDEH0154D27Cfg*				const cfg;
	uint8_t*							const userBuf;		// Сам буффер менять можно, но указетль на него - нет.
	// Для предотвращения попытки использовать LCD из разных потоков одновременно.
	USER_OS_STATIC_MUTEX_BUFFER			mb;
	USER_OS_STATIC_MUTEX				m					= nullptr;
};

}

#endif
