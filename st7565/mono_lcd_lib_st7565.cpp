#include "mono_lcd_lib_st7565.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CMD_DISPLAY_OFF             0xAE
#define CMD_DISPLAY_ON              0xAF

#define CMD_SET_DISP_START_LINE     0x40
#define CMD_SET_PAGE                0xB0

#define CMD_SET_COLUMN_UPPER        0x10
#define CMD_SET_COLUMN_LOWER        0x00

#define CMD_SET_ADC_NORMAL          0xA0
#define CMD_SET_ADC_REVERSE         0xA1

#define CMD_SET_DISP_NORMAL         0xA6
#define CMD_SET_DISP_REVERSE        0xA7

#define CMD_SET_ALLPTS_NORMAL       0xA4
#define CMD_SET_ALLPTS_ON           0xA5
#define CMD_SET_BIAS_9              0xA2
#define CMD_SET_BIAS_7              0xA3

#define CMD_RMW                     0xE0
#define CMD_RMW_CLEAR               0xEE
#define CMD_INTERNAL_RESET          0xE2
#define CMD_SET_COM_NORMAL          0xC0
#define CMD_SET_COM_REVERSE         0xC8
#define CMD_SET_POWER_CONTROL       0x28
#define CMD_SET_RESISTOR_RATIO      0x20
#define CMD_SET_VOLUME_FIRST        0x81
#define CMD_SET_VOLUME_SECOND       0
#define CMD_SET_STATIC_OFF          0xAC
#define CMD_SET_STATIC_ON           0xAD
#define CMD_SET_STATIC_REG          0x0
#define CMD_SET_BOOSTER_FIRST       0xF8
#define CMD_SET_BOOSTER_234         0
#define CMD_SET_BOOSTER_5           1
#define CMD_SET_BOOSTER_6           3
#define CMD_NOP                     0xE3
#define CMD_TEST                    0xF0


ST7565::ST7565 ( const ST7565Cfg* const cfg, uint8_t* const buf ) :
	cfg( cfg ), userBuf( buf ) {
    this->mutex     = USER_OS_STATIC_MUTEX_CREATE( &this->mutex_buf );
}

BASE_RESULT ST7565::com_out ( uint8_t command ) {
    cfg->a0->reset();
    cfg->cs->reset();
	BASE_RESULT r = this->cfg->s->tx( &command, 1, 100 );
    cfg->cs->set();
	return r;
}

BASE_RESULT ST7565::data_out ( uint8_t data ) {
    cfg->a0->set();
    cfg->cs->reset();
	BASE_RESULT r = this->cfg->s->tx( &data, 1, 100 );
    cfg->cs->set();
	return r;
}



BASE_RESULT ST7565::setContrast ( uint8_t val) {
	BASE_RESULT r = this->com_out(CMD_SET_VOLUME_FIRST);
	checkResult( r );
	r = this->com_out(CMD_SET_VOLUME_SECOND | (val & 0x3f));
	return r;
}

BASE_RESULT ST7565::reset ( void ) {
	cfg->cs->set();
	cfg->res->reset();
	USER_OS_DELAY_MS(5);
	cfg->res->set();
    USER_OS_DELAY_MS(5);

	BASE_RESULT r;

    // LCD bias select
	r = this->com_out(CMD_SET_BIAS_9);
	checkResult( r );

	// ADC select
	r = this->com_out(CMD_SET_ADC_REVERSE);
	checkResult( r );

	// SHL select
	r = this->com_out(CMD_SET_COM_NORMAL);
	checkResult( r );

	// Initial display line
	r = this->com_out(CMD_SET_DISP_START_LINE);
	checkResult( r );

	// turn on voltage converter (VC=1, VR=0, VF=0)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x4);
	checkResult( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x6);
	checkResult( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x7);
	checkResult( r );
	USER_OS_DELAY_MS(1);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	r = this->com_out(CMD_SET_RESISTOR_RATIO | 0x6);
	checkResult( r );

	r = this->com_out(CMD_SET_ALLPTS_NORMAL);
	return r;
}

BASE_RESULT ST7565::on ( void ) {
	BASE_RESULT r;
	r = this->com_out(CMD_DISPLAY_ON);
	return r;
}

BASE_RESULT ST7565::off ( void ) {
	BASE_RESULT r;
	r = this->com_out(CMD_DISPLAY_OFF);
	return r;
}

BASE_RESULT ST7565::update ( void ) {
	BASE_RESULT r;
    for ( int page_l = 0; page_l < 8; page_l++ ) {
		r = this->com_out( CMD_SET_PAGE | page_l);
		checkResult( r );
		r = this->com_out( CMD_SET_COLUMN_UPPER );
		checkResult( r );

        cfg->a0->set();
        cfg->cs->reset();

        memset( this->system_buf, 0, 128 );

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
					this->system_buf[ column_l ] |= ( ( this->userBuf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                case (uint32_t)ST7565_MODE::IVERT_X:
                case (uint32_t)ST7565_MODE::IVERT_X_AMD_Y:
					this->system_buf[ 127 - column_l ] |= ( ( this->userBuf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                }
            }
        }

		r = this->cfg->s->tx( this->system_buf, 128, 100 );
		checkResult( r );

        cfg->cs->set();
    }
	return r;
}

BASE_RESULT ST7565::clear ( void ) {
    uint8_t buf = 0;
	BASE_RESULT r;

    for(int p = 0; p < 8; p++) {
		r = this->com_out( CMD_SET_PAGE | p);
		checkResult( r );
		r = this->com_out( CMD_SET_COLUMN_UPPER );
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
