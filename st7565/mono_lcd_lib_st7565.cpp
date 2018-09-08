#include "mono_lcd_lib_st7565.h"

#ifdef MODULE_MONO_LCD_ST7565_ENABLED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mono_lcd_lib_st7565_cmd.h"

namespace MonoLcd {

ST7565::ST7565 ( const ST7565Cfg* const cfg, uint8_t* const buf ) :
	cfg( cfg ), userBuf( buf ) {
    this->m     = USER_OS_STATIC_MUTEX_CREATE( &this->mb );
}

McHardwareInterfaces::BaseResult ST7565::comOut ( uint8_t command ) {
    cfg->a0->reset();
    cfg->cs->reset();
	McHardwareInterfaces::BaseResult r = this->cfg->s->tx( &command, 1, 100 );
    cfg->cs->set();
	return r;
}

McHardwareInterfaces::BaseResult ST7565::dataOut ( uint8_t data ) {
    cfg->a0->set();
    cfg->cs->reset();
	McHardwareInterfaces::BaseResult r = this->cfg->s->tx( &data, 1, 100 );
    cfg->cs->set();
	return r;
}



McHardwareInterfaces::BaseResult ST7565::setContrast ( uint8_t	val ) {
	McHardwareInterfaces::BaseResult r = this->comOut(CMD_SET_VOLUME_FIRST);
	checkResult( r );
	r = this->comOut(CMD_SET_VOLUME_SECOND | (val & 0x3f));
	return r;
}

McHardwareInterfaces::BaseResult ST7565::reset ( void ) {
	cfg->cs->set();
	cfg->res->reset();
	USER_OS_DELAY_MS(5);
	cfg->res->set();
    USER_OS_DELAY_MS(5);

	McHardwareInterfaces::BaseResult r;

    // LCD bias select
	r = this->comOut(CMD_SET_BIAS_9);
	checkResult( r );

	// ADC select
	r = this->comOut(CMD_SET_ADC_REVERSE);
	checkResult( r );

	// SHL select
	r = this->comOut(CMD_SET_COM_NORMAL);
	checkResult( r );

	// Initial display line
	r = this->comOut(CMD_SET_DISP_START_LINE);
	checkResult( r );

	// turn on voltage converter (VC=1, VR=0, VF=0)
	r = this->comOut(CMD_SET_POWER_CONTROL | 0x4);
	checkResult( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	r = this->comOut(CMD_SET_POWER_CONTROL | 0x6);
	checkResult( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	r = this->comOut(CMD_SET_POWER_CONTROL | 0x7);
	checkResult( r );
	USER_OS_DELAY_MS(1);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	r = this->comOut(CMD_SET_RESISTOR_RATIO | 0x6);
	checkResult( r );

	r = this->comOut(CMD_SET_ALLPTS_NORMAL);
	return r;
}

McHardwareInterfaces::BaseResult ST7565::on ( void ) {
	McHardwareInterfaces::BaseResult r;
	r = this->comOut(CMD_DISPLAY_ON);
	return r;
}

McHardwareInterfaces::BaseResult ST7565::off ( void ) {
	McHardwareInterfaces::BaseResult r;
	r = this->comOut(CMD_DISPLAY_OFF);
	return r;
}

McHardwareInterfaces::BaseResult ST7565::update ( void ) {
	McHardwareInterfaces::BaseResult r;
    for ( int page_l = 0; page_l < 8; page_l++ ) {
		r = this->comOut( CMD_SET_PAGE | page_l);
		checkResult( r );
		r = this->comOut( CMD_SET_COLUMN_UPPER );
		checkResult( r );

        cfg->a0->set();
        cfg->cs->reset();

        memset( this->lcdImage, 0, 128 );

        for ( uint32_t string_l = 0; string_l < 8; string_l++ ) {
            uint32_t us_p_string;
            if ( ( this->cfg->mode == ST7565_MODE::IVERT_Y ) || ( this->cfg->mode == ST7565_MODE::IVERT_X_AMD_Y ) ) {
                us_p_string = (7 - page_l) * 128 + ( 7 - string_l ) * 16;
            } else {
                us_p_string = page_l * 128 + string_l * 16;
            }
            for ( uint32_t column_l = 0; column_l < 128; column_l++ ) {
                switch ( (uint32_t)this->cfg->mode ) {
                case (uint32_t)ST7565_MODE::STANDARD:
                case (uint32_t)ST7565_MODE::IVERT_Y:
					this->lcdImage[ column_l ] |= ( ( this->userBuf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                case (uint32_t)ST7565_MODE::IVERT_X:
                case (uint32_t)ST7565_MODE::IVERT_X_AMD_Y:
					this->lcdImage[ 127 - column_l ] |= ( ( this->userBuf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                }
            }
        }

		r = this->cfg->s->tx( this->lcdImage, 128, 100 );
		checkResult( r );

        cfg->cs->set();
    }
	return r;
}

McHardwareInterfaces::BaseResult ST7565::lcdClear ( void ) {
    uint8_t buf = 0;
	McHardwareInterfaces::BaseResult r;

    for(int p = 0; p < 8; p++) {
		r = this->comOut( CMD_SET_PAGE | p);
		checkResult( r );
		r = this->comOut( CMD_SET_COLUMN_UPPER );
		checkResult( r );

        cfg->a0->set();
        cfg->cs->reset();
		r = this->cfg->s->txOneItem ( buf, 128, 100 );
		checkResult( r );
        cfg->cs->set();
    }
	return r;
}

void ST7565::bufClear ( void ) {
	memset(this->userBuf, 0, 1024);
}

}

#endif
