#include "mono_lcd_lib_st7565.h"

void mono_lcd_lib_st7565::reinit (  const spi_base* spi_obj  ) const {
    this->mutex     = USER_OS_STATIC_MUTEX_CREATE( &this->mutex_buf );
    this->spi       = spi_obj;
}

void mono_lcd_lib_st7565::com_out ( uint8_t command ) const {
    cfg->a0->reset();
    cfg->cs->reset();
    this->spi->tx( &command, 1, 100 );
    cfg->cs->set();
}

void mono_lcd_lib_st7565::data_out ( uint8_t data ) const {
    cfg->a0->set();
    cfg->cs->reset();
    this->spi->tx( &data, 1, 100 );
    cfg->cs->set();
}



void mono_lcd_lib_st7565::set_brightness ( uint8_t val) const {
    this->com_out(CMD_SET_VOLUME_FIRST);
    this->com_out(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void mono_lcd_lib_st7565::reset ( uint8_t contrast ) const {        
        cfg->res->reset();
        USER_OS_DELAY_MS(50);
        cfg->res->set();
        USER_OS_DELAY_MS(50);

         // LCD bias select
        this->com_out(CMD_SET_BIAS_9);
          // ADC select
        this->com_out(CMD_SET_ADC_NORMAL);
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

  this->com_out(CMD_DISPLAY_ON);
  this->com_out(CMD_SET_ALLPTS_NORMAL);
  this->set_brightness(contrast);
}

void mono_lcd_lib_st7565::update ( void ) const {
    for(int p = 0; p < 8; p++) {
        this->com_out( CMD_SET_PAGE | p);
        uint8_t col = 0;

        /*
         * 4, т.к. дисплей рассчитан на 132 пикселя, а в LCD их всего 128 (в ширину).
         */
        this->com_out( CMD_SET_COLUMN_LOWER | ((col + 4) & 0x0F));
        this->com_out( CMD_SET_COLUMN_UPPER | (((col + 4) >> 4) & 0x0F));
        this->com_out( CMD_RMW);

        for( ; col < 128; col++) {
            this->data_out( this->buf[(128*p)+col]);
        }
    }
}
