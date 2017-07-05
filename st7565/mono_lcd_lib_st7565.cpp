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

void mono_lcd_lib_st7565::set_brightness ( uint8_t val) const {
    this->com_out(CMD_SET_VOLUME_FIRST);
    this->com_out(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void mono_lcd_lib_st7565::reset ( uint8_t contrast ) const {
        uint8_t command;

        cfg->cs->reset();
        cfg->a0->reset();
        cfg->res->set();
        vTaskDelay(500);
        cfg->res->reset();
        vTaskDelay(500);
        cfg->res->set();
        vTaskDelay(500);
        command = 0xA2; this->spi->tx(&command, 1, 100);
        command = 0xA0; this->spi->tx(&command, 1, 100);
        command = 0xC8; this->spi->tx(&command, 1, 100);
        command = 0x40; this->spi->tx(&command, 1, 100);
        command = 0x2C; this->spi->tx(&command, 1, 100);
        vTaskDelay(100);
        command = 0x2E; this->spi->tx(&command, 1, 100);
        vTaskDelay(100);
        command = 0x2F; this->spi->tx(&command, 1, 100);
        command = 0x26; this->spi->tx(&command, 1, 100);
        command = 0x81; this->spi->tx(&command, 1, 100);
        command = 0x1A; this->spi->tx(&command, 1, 100);
        command = 0xAF; this->spi->tx(&command, 1, 100);
        command = 0xB0; this->spi->tx(&command, 1, 100);
        command = 0x10; this->spi->tx(&command, 1, 100);
        command = 0x00; this->spi->tx(&command, 1, 100);
        cfg->a0->set();
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);
        command = 0xFF; this->spi->tx(&command, 1, 100);

        /*
        port_gpio_set(st7565_struct->cfg->pins_cs, 1, 100);
        vTaskDelay(100);
        cfg->cs->reset();
        cfg->res->set();
        vTaskDelay(100);
        cfg->res->reset();
        vTaskDelay(500);
        cfg->res->set();
        port_gpio_set(st7565_struct->cfg->pins_cs, 1, 100);
         // LCD bias select
        _st7565_command(cfg, CMD_SET_BIAS_7);
          // ADC select
        _st7565_command(cfg, CMD_SET_ADC_NORMAL);
          // SHL select
        _st7565_command(cfg, CMD_SET_COM_NORMAL);
        // Initial display line
        _st7565_command(cfg, CMD_SET_DISP_START_LINE);

         // turn on voltage converter (VC=1, VR=0, VF=0)
        _st7565_command(cfg, CMD_SET_POWER_CONTROL | 0x4);
         // wait for 50% rising
        vTaskDelay(50);

         // turn on voltage regulator (VC=1, VR=1, VF=0)
        _st7565_command(cfg, CMD_SET_POWER_CONTROL | 0x6);
         // wait >=50ms
        vTaskDelay(50);

         // turn on voltage follower (VC=1, VR=1, VF=1)
        _st7565_command(cfg, CMD_SET_POWER_CONTROL | 0x7);
         // wait
        vTaskDelay(10);

         // set lcd operating voltage (regulator resistor, ref voltage resistor)
        _st7565_command(cfg, CMD_SET_RESISTOR_RATIO | 0x6);
    */
  this->com_out(CMD_DISPLAY_ON);
  this->com_out(CMD_SET_ALLPTS_NORMAL);
  this->set_brightness(contrast);
}
