#include "mono_lcd_lib_gdeh0154d27.h"

#ifdef MODULE_MONO_LCD_GDEH0154D27_ENABLED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace MonoLcd {

GDEH0154D27::GDEH0154D27 ( const GDEH0154D27Cfg* const cfg, uint8_t* const buf ) :
	cfg( cfg ), userBuf( buf ) {
    this->m     = USER_OS_STATIC_MUTEX_CREATE( &this->mb );
}

McHardwareInterfaces::BaseResult GDEH0154D27::comOut ( uint8_t command ) {
	while( this->cfg->busy->read() == true ) {}
	cfg->dc->reset();
    cfg->cs->reset();
	McHardwareInterfaces::BaseResult r = this->cfg->s->tx( &command, 1, 10 );
    cfg->cs->set();
	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::dataOut ( uint8_t data ) {
	while( this->cfg->busy->read() == true ) {}
	cfg->dc->set();
    cfg->cs->reset();
	McHardwareInterfaces::BaseResult r = this->cfg->s->tx( &data, 1, 10 );
    cfg->cs->set();
	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandDriverOutputControl ( uint16_t yPixels ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x01 );
		checkResultAndBreak( r );
		r = this->dataOut( static_cast< uint8_t >( yPixels - 1 ) );
		checkResultAndBreak( r );
		r = this->dataOut( static_cast< uint8_t >( ( ( yPixels - 1 ) >> 8 ) & 0x01 ) );
		checkResultAndBreak( r );
		r = this->dataOut( 0 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetDummyLinePeriod ( uint8_t dummyLinePeriod  ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x3A );
		checkResultAndBreak( r );
		r = this->dataOut( dummyLinePeriod & 0b1111111 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetGateLineWidth ( uint8_t gateLineWidth ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x3B );
		checkResultAndBreak( r );
		r = this->dataOut( gateLineWidth & 0b1111 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandDataEntryModeSetting ( uint8_t dataEntryModeSetting ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x11 );
		checkResultAndBreak( r );
		r = this->dataOut( dataEntryModeSetting & 0b111 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetRAMxAddress ( void ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x44 );
		checkResultAndBreak( r );
		r = this->dataOut( 0x00 );
		checkResultAndBreak( r );
		r = this->dataOut( 0X18 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetRAMyAddress ( void ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x45 );
		checkResultAndBreak( r );
		r = this->dataOut( 0xc7 );
		checkResultAndBreak( r );
		r = this->dataOut( 0x00 );
		checkResultAndBreak( r );
		r = this->dataOut( 0x00 );
		checkResultAndBreak( r );
		r = this->dataOut( 0x00 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandWriteVcomRegister ( uint8_t vcom ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x2C );
		checkResultAndBreak( r );
		r = this->dataOut( vcom );
	} while ( false );

	return r;
}


McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandWriteLutRegister ( const uint8_t lutRegister[ 30 ] ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x32 );
		checkResultAndBreak( r );

		cfg->dc->set();
		cfg->cs->reset();
		r = this->cfg->s->tx( lutRegister, 30, 20 );
		cfg->cs->set();
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetRamXAddressCounter ( uint8_t ramXAddressCounter ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x4E );
		checkResultAndBreak( r );
		r = this->dataOut( ramXAddressCounter & 0b11111 );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandSetRamYAddressCounter ( uint16_t ramYAddressCounter ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x4F );
		checkResultAndBreak( r );
		r = this->dataOut( static_cast< uint8_t >( ramYAddressCounter ) );
		checkResultAndBreak( r );
		r = this->dataOut( static_cast< uint8_t >( ( ramYAddressCounter >> 8 ) & 0x01 ) );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendBuffer ( void ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x24 );
		checkResultAndBreak( r );

		cfg->dc->set();
		cfg->cs->reset();
		r = this->cfg->s->tx( this->userBuf, 5000, 100 );
		cfg->cs->set();

		r = this->comOut( 0xff );
				checkResultAndBreak( r );
	} while( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandDisplayUpdateControl2 ( uint8_t arg ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x22 );
		checkResultAndBreak( r );
		r = this->dataOut( arg );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandMasterActivation ( void ) {
	McHardwareInterfaces::BaseResult r;

	r = this->comOut( 0x20 );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::sendCommandBoosterSoftstart( uint8_t a, uint8_t b, uint8_t c ) {
	McHardwareInterfaces::BaseResult r;

	do {
		r = this->comOut( 0x0C );
		checkResultAndBreak( r );
		r = this->dataOut( a | ( 1 << 7 ) );
		checkResultAndBreak( r );
		r = this->dataOut( b | ( 1 << 7 ) );
		checkResultAndBreak( r );
		r = this->dataOut( c | ( 1 << 7 ) );
		checkResultAndBreak( r );
	} while ( false );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::reset ( void ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult r;

	do {
		cfg->cs->set();
		cfg->res->reset();
		USER_OS_DELAY_MS(10);
		cfg->res->set();
		USER_OS_DELAY_MS(10);

		r = this->sendCommandBoosterSoftstart( 0xd7, 0xd6, 0x9d );
																														checkResultAndBreak( r );

		r = this->sendCommandDriverOutputControl( 200 );
		checkResultAndBreak( r );

		r = this->sendCommandSetDummyLinePeriod( 0x1a );
				checkResultAndBreak( r );

				r = this->sendCommandSetGateLineWidth( 0x08 );
				checkResultAndBreak( r );

				r = sendCommandDataEntryModeSetting( 0x01 );
										checkResultAndBreak( r );

					r = sendCommandSetRAMxAddress();
						checkResultAndBreak( r );
					r = sendCommandSetRAMyAddress();
					checkResultAndBreak( r );




											r = sendCommandWriteVcomRegister( 0x10 );
													checkResultAndBreak( r );






													static uint8_t LUT[ 30 ] = {	0x50, 0xAA, 0x55, 0xAA, 0x11,
																							0x00, 0x00, 0x00, 0x00, 0x00,
																							0x00, 0x00, 0x00, 0x00, 0x00,
																							0x00, 0x00, 0x00, 0x00, 0x00,
																							0xFF, 0xFF, 0x1F, 0x00, 0x00,
																							0x00, 0x00, 0x00, 0x00, 0x00 };
															r = sendCommandWriteLutRegister( LUT );




															r = sendCommandSetRamXAddressCounter( 0x00 );
																								checkResultAndBreak( r );
																								r = sendCommandSetRamYAddressCounter( 0x00 );
																								checkResultAndBreak( r );


																								r = this->sendBuffer();

																								r = sendCommandDisplayUpdateControl2( 0xc0 );
																										checkResultAndBreak( r );



																												r = sendCommandDisplayUpdateControl2( 0xC4 );
																														checkResultAndBreak( r );
																										r = sendCommandMasterActivation();
					while( this->cfg->busy->read() == true ) {
					//	r = sendCommandMasterActivation();
				//		checkResultAndBreak( r );
					/*	cfg->dc->reset();
						    cfg->cs->reset();
						    uint8_t c = 0x20;
							r = this->cfg->s->tx( &c, 1, 10 );
						    cfg->cs->set();
						checkResultAndBreak( r );*/
									}







/*









*/

	} while ( false );

	USER_OS_GIVE_MUTEX( this->m );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::on ( void ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult r;
	r = McHardwareInterfaces::BaseResult::errNotData;
	//r = this->comOut(CMD_DISPLAY_ON);

	USER_OS_GIVE_MUTEX( this->m );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::off ( void ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult r;
	r = McHardwareInterfaces::BaseResult::errNotData;
	//r = this->comOut(CMD_DISPLAY_OFF);

	USER_OS_GIVE_MUTEX( this->m );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::update ( void ) {
	McHardwareInterfaces::BaseResult r;

	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	r = this->sendBuffer();

    USER_OS_GIVE_MUTEX( this->m );

	return r;
}

McHardwareInterfaces::BaseResult GDEH0154D27::lcdClear ( void ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult r;

	memset( this->userBuf, 0, 5000 );
	r = this->sendBuffer();

     USER_OS_GIVE_MUTEX( this->m );

	return r;
}

void GDEH0154D27::bufClear ( void ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	memset( this->userBuf, 0, 5000 );

	USER_OS_GIVE_MUTEX( this->m );
}

}

#endif
