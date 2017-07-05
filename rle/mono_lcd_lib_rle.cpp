#include "mono_lcd_lib_rle.h"

// byte_size_x - колличество байт на по оси X. На строку.
uint8_t mono_lcd_lib_rle::get_byte ( void ) {
    uint8_t buffer;
    if ( this->rle_loop != 0 ) {                // Если еще не закончились повторения.
        if (this->flag_rle == false) {
            this->rle_loop--;
            buffer = ( uint8_t )( *( this->p - 1 ) ); // Если выдем loop раз значение справа от счетчика. Но мы уже приготовились смотреть на новый счетсчик, так что для нас он слева.
        } else {
            buffer = *this->p;                // Если у нас неповторяющиеся значения.
            this->p++;
            this->rle_loop--;
        };
    } else {
        if (((char)(*this->p)) > 0){		// > 0 - повторяющиеся, < 0 не повторяющиеся.
            flag_rle = false;
            buffer = *(this->p+1);			// Выдаем повторяющийся байт.
            this->rle_loop = (uint8_t)(*this->p)-1;		// Сохроняем колличество одинковых байт.
            this->p += 2;
        } else {						// Если байт не повторяющийся.
            this->flag_rle = true;
           this-> rle_loop = ((uint8_t)(abs((char)(*this->p))))-1;
            this->p++;
            buffer = *this->p;			// Выдаем нужный байт.
            this->p++;
        }
    };
    return buffer;
}
