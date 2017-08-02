#include "mono_lcd_lib_st7565.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CMD_DISPLAY_OFF           0xAE
#define CMD_DISPLAY_ON            0xAF

#define CMD_SET_DISP_START_LINE 0x40
#define CMD_SET_PAGE              0xB0

#define CMD_SET_COLUMN_UPPER      0x10
#define CMD_SET_COLUMN_LOWER      0x00

#define CMD_SET_ADC_NORMAL      0xA0
#define CMD_SET_ADC_REVERSE     0xA1

#define CMD_SET_DISP_NORMAL     0xA6
#define CMD_SET_DISP_REVERSE     0xA7

#define CMD_SET_ALLPTS_NORMAL     0xA4
#define CMD_SET_ALLPTS_ON          0xA5
#define CMD_SET_BIAS_9             0xA2
#define CMD_SET_BIAS_7             0xA3

#define CMD_RMW                  0xE0
#define CMD_RMW_CLEAR            0xEE
#define CMD_INTERNAL_RESET      0xE2
#define CMD_SET_COM_NORMAL      0xC0
#define CMD_SET_COM_REVERSE      0xC8
#define CMD_SET_POWER_CONTROL      0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST      0x81
#define CMD_SET_VOLUME_SECOND      0
#define CMD_SET_STATIC_OFF      0xAC
#define CMD_SET_STATIC_ON          0xAD
#define CMD_SET_STATIC_REG      0x0
#define CMD_SET_BOOSTER_FIRST      0xF8
#define CMD_SET_BOOSTER_234      0
#define CMD_SET_BOOSTER_5          1
#define CMD_SET_BOOSTER_6          3
#define CMD_NOP                     0xE3
#define CMD_TEST                    0xF0


void mono_lcd_lib_st7565::reinit ( void ) const {
    this->mutex     = USER_OS_STATIC_MUTEX_CREATE( &this->mutex_buf );
}

void mono_lcd_lib_st7565::com_out ( uint8_t command ) const {
    cfg->a0->reset();
    cfg->cs->reset();
    this->cfg->p_spi->tx( &command, 1, 100 );
    cfg->cs->set();
}

void mono_lcd_lib_st7565::data_out ( uint8_t data ) const {
    cfg->a0->set();
    cfg->cs->reset();
    this->cfg->p_spi->tx( &data, 1, 100 );
    cfg->cs->set();
}



void mono_lcd_lib_st7565::set_contrast ( uint8_t val) const {
    this->com_out(CMD_SET_VOLUME_FIRST);
    this->com_out(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void mono_lcd_lib_st7565::reset ( void ) const {
        cfg->res->reset();
        USER_OS_DELAY_MS(50);
        cfg->res->set();
        USER_OS_DELAY_MS(50);

         // LCD bias select
        this->com_out(CMD_SET_BIAS_9);
          // ADC select
        this->com_out(CMD_SET_ADC_REVERSE);
          // SHL select
        this->com_out(CMD_SET_COM_NORMAL);
        // Initial display line
        this->com_out(CMD_SET_DISP_START_LINE);

         // turn on voltage converter (VC=1, VR=0, VF=0)
        this->com_out(CMD_SET_POWER_CONTROL | 0x4);
         // wait for 50% rising
        USER_OS_DELAY_MS(50);

         // turn on voltage regulator (VC=1, VR=1, VF=0)
        this->com_out(CMD_SET_POWER_CONTROL | 0x6);
         // wait >=50ms
        USER_OS_DELAY_MS(50);

         // turn on voltage follower (VC=1, VR=1, VF=1)
        this->com_out(CMD_SET_POWER_CONTROL | 0x7);
         // wait
        USER_OS_DELAY_MS(10);

         // set lcd operating voltage (regulator resistor, ref voltage resistor)
        this->com_out(CMD_SET_RESISTOR_RATIO | 0x6);

        this->com_out(CMD_SET_ALLPTS_NORMAL);
}

void mono_lcd_lib_st7565::on ( void ) const {
    this->com_out(CMD_DISPLAY_ON);
}

void mono_lcd_lib_st7565::off ( void ) const {
    this->com_out(CMD_DISPLAY_OFF);
}

void mono_lcd_lib_st7565::update ( void ) const {
    for(int p = 0; p < 8; p++) {
        this->com_out( CMD_SET_PAGE | p);

        this->com_out( CMD_SET_COLUMN_LOWER );
        this->com_out( CMD_SET_COLUMN_UPPER );

        cfg->a0->set();
        cfg->cs->reset();
        this->cfg->p_spi->tx( &this->buf[(128*p)], 128, 100 );

        cfg->cs->set();
    }
}

void mono_lcd_lib_st7565::clear ( void ) const {
    uint8_t buf = 0;
    for(int p = 0; p < 8; p++) {
        this->com_out( CMD_SET_PAGE | p);
        this->com_out( CMD_SET_COLUMN_LOWER );
        this->com_out( CMD_SET_COLUMN_UPPER );

        cfg->a0->set();
        cfg->cs->reset();
        this->cfg->p_spi->tx_one_item ( buf, 128, 100 ) ;
        cfg->cs->set();
    }
}

void mono_lcd_lib_st7565::buf_clear ( void ) const {
    memset(this->buf, 0, 1024);
}
