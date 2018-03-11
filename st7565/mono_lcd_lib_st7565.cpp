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


mono_lcd_lib_st7565::mono_lcd_lib_st7565 ( const mono_lcd_lib_st7565_cfg_t* const cfg, uint8_t* const buf ): cfg(cfg), user_buf(buf) {
    this->mutex     = USER_OS_STATIC_MUTEX_CREATE( &this->mutex_buf );
}

BASE_RESULT mono_lcd_lib_st7565::com_out ( uint8_t command ) const {
    cfg->a0->reset();
    cfg->cs->reset();
	BASE_RESULT r = this->cfg->p_spi->tx( &command, 1, 100 );
    cfg->cs->set();
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::data_out ( uint8_t data ) const {
    cfg->a0->set();
    cfg->cs->reset();
	BASE_RESULT r = this->cfg->p_spi->tx( &data, 1, 100 );
    cfg->cs->set();
	return r;
}



BASE_RESULT mono_lcd_lib_st7565::set_contrast ( uint8_t val) const {
	BASE_RESULT r = this->com_out(CMD_SET_VOLUME_FIRST);
	check_result( r );
	r = this->com_out(CMD_SET_VOLUME_SECOND | (val & 0x3f));
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::reset ( void ) const {
	cfg->cs->set();
	cfg->res->reset();
	USER_OS_DELAY_MS(5);
	cfg->res->set();
    USER_OS_DELAY_MS(5);

	BASE_RESULT r;

    // LCD bias select
	r = this->com_out(CMD_SET_BIAS_9);
	check_result( r );

	// ADC select
	r = this->com_out(CMD_SET_ADC_REVERSE);
	check_result( r );

	// SHL select
	r = this->com_out(CMD_SET_COM_NORMAL);
	check_result( r );

	// Initial display line
	r = this->com_out(CMD_SET_DISP_START_LINE);
	check_result( r );

	// turn on voltage converter (VC=1, VR=0, VF=0)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x4);
	check_result( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x6);
	check_result( r );
	USER_OS_DELAY_MS(5);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	r = this->com_out(CMD_SET_POWER_CONTROL | 0x7);
	check_result( r );
	USER_OS_DELAY_MS(1);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	r = this->com_out(CMD_SET_RESISTOR_RATIO | 0x6);
	check_result( r );

	r = this->com_out(CMD_SET_ALLPTS_NORMAL);
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::on ( void ) const {
	BASE_RESULT r;
	r = this->com_out(CMD_DISPLAY_ON);
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::off ( void ) const {
	BASE_RESULT r;
	r = this->com_out(CMD_DISPLAY_OFF);
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::update ( void ) const {
	BASE_RESULT r;
    for ( int page_l = 0; page_l < 8; page_l++ ) {
		r = this->com_out( CMD_SET_PAGE | page_l);
		check_result( r );
		r = this->com_out( CMD_SET_COLUMN_UPPER );
		check_result( r );

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
                    this->system_buf[ column_l ] |= ( ( this->user_buf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                case (uint32_t)ST7565_MODE::IVERT_X:
                case (uint32_t)ST7565_MODE::IVERT_X_AMD_Y:
                    this->system_buf[ 127 - column_l ] |= ( ( this->user_buf[ us_p_string + column_l / 8 ] >> ( column_l % 8 ) ) & 1 ) << string_l; break;
                }
            }
        }

		r = this->cfg->p_spi->tx( this->system_buf, 128, 100 );
		check_result( r );

        cfg->cs->set();
    }
	return r;
}

BASE_RESULT mono_lcd_lib_st7565::clear ( void ) const {
    uint8_t buf = 0;
	BASE_RESULT r;

    for(int p = 0; p < 8; p++) {
		r = this->com_out( CMD_SET_PAGE | p);
		check_result( r );
		r = this->com_out( CMD_SET_COLUMN_UPPER );
		check_result( r );

        cfg->a0->set();
        cfg->cs->reset();
		r = this->cfg->p_spi->tx_one_item ( buf, 128, 100 );
		check_result( r );
        cfg->cs->set();
    }
	return r;
}

void mono_lcd_lib_st7565::buf_clear ( void ) const {
    memset(this->user_buf, 0, 1024);
}
