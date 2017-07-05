#include "mono_lcd_lib_st7565.h"

void mono_lcd_lib_st7565::reinit (  const spi_base* spi_obj  ) const {
    this->mutex     = USER_OS_STATIC_MUTEX_CREATE( &this->mutex_buf );
    this->spi       = spi_obj;
}
