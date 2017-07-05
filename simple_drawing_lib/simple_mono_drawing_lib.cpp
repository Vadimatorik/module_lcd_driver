#include "simple_mono_drawing_lib.h"
#include <cstring>

// Рисуем пиксель в LCD буфере.
void simple_drawing_lib::point_set ( const uint8_t& x, const uint8_t& y ) const {
    if ( ( x < 128 ) && ( y < 64 ) ) {
        this->buf[ ( y / 8 ) * 128 + x ] |=
                1 << ( y % 8 );	// Особенности памяти...
    }
}

// Рисуем пиксель в LCD буфере.
void simple_drawing_lib::point_reset( const uint8_t& x, const uint8_t& y ) const {
    if ( ( x < 128 ) && ( y < 64 ) ) {
        this->buf[ ( y / 8 ) * 128 + x ] &=
                ( uint8_t )( ( ~( 1 << ( y % 8 ) ) ) );	// Особенности памяти...
    }
}

void simple_drawing_lib::set_pixel ( const uint8_t& x, const uint8_t& y, const uint8_t& color ) const {
    if ( color ) {
        this->point_set( x, y );
    } else {
        this->point_reset( x, y );
    }
}

void    simple_drawing_lib::fill_rect ( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t& color ) const {
    uint8_t i;

    if ( x1 > x2 ) { // swap
        i = x1;
        x1 = x2;
        x2 = i;
    }

    if ( y1 > y2 ) { // swap
        i = y1;
        y1 = y2;
        y2 = i;
    }

    if (color != 0) {		// Закрашиваем экран или чистим.
        for (uint8_t x_point = x1; x_point<=x2; x_point++) {
            for (uint8_t y_point = y1; y_point<=y2; y_point++) {
                this->point_set( x_point, y_point );
            }
        }
    } else {
        for (uint8_t x_point = x1; x_point<=x2; x_point++) {
            for (uint8_t y_point = y1; y_point<=y2; y_point++) {
                this->point_reset( x_point, y_point );
            }
        }
    }
}


void simple_drawing_lib::draw_line ( uint8_t x1, uint8_t y1,const  uint8_t& x2, const uint8_t& y2, const uint8_t& color ) const {
    int deltaX, deltaY, signX, signY, error, error2;
    int dX, dY;
    uint16_t x, y;

    if ( x1 == x2 ){
        if ( y2 > y1 ){
            for ( y = y1; y < y2; y ++ )
                this->set_pixel( x1, y, color );
        } else {
            for ( y = y2; y < y1; y ++ )
               this->set_pixel ( x1, y, color );
        }
        return;
    };
    if ( y1 == y2 ){
        if ( x2 > x1 ){
            for ( x = x1; x < x2; x ++ )
                this->set_pixel ( x, y1, color );
        } else {
            for ( x = x2; x < x1; x ++ )
               this->set_pixel( x, y1, color );
        };
        return;
    }

    dX = x2 - x1;
    dY = y2 - y1;
    deltaX = abs(dX);
    deltaY = abs(dY);
    signX = (x1 < x2) ? 1 : -1;
    signY = (y1 < y2) ? 1 : -1;
    error = deltaX - deltaY;

    this->set_pixel ( x2, y2, color );
    while ( x1 != x2 || y1 != y2 ){
        this->set_pixel ( x1, y1, color );
        error2 = error * 2;
        if ( error2 > -deltaY ){
            error -= deltaY;
            x1 += signX;
        }
        if ( error2 < deltaX ){
            error += deltaX;
            y1 += signY;
      };
    };
}

// Рисуем символ на экране выбранным шрифтом.
// font - указатель на структуру типа tFont, описанную в файле font_struct.h.
// x, y - положения левого верхнего угла.
// ch - символ на вывод UTF-8.
// color цвет.
// or - ореинтация экрана: 0 - портретная, 1 - горизонтальная.
void simple_drawing_lib::print_char ( const font_t& font, const uint8_t& x, const uint8_t& y, uint8_t ch, const uint8_t& color, const uint8_t& rle ) const {
    if (ch<0x20) return;			// Защита от пустых пикселей.
    ch -= 0x20;						// Первые 0x20 элементов отсутствуют.
    uint8_t byte_w = 0;				// Нужно узнать, сколько uint8_t приходится на одну строку.
    int buffer_w = font.chars[ch].image->x; // Смотрим, сколько пикселей приходится на ширину.
    while(buffer_w>0){
        byte_w++;			// Минимум 1.
        buffer_w-=8;		// Шаг - 8 бит.
    };
    const uint8_t* buf = font.chars[ch].image->p_array;	// Получаем указатель на 0-й байт 0-й строки.
    if (rle == 1) {			// Если используем rle, то указываем начало символа там.
        mono_lcd_lib_rle rle(buf);
        volatile uint8_t buffer;
        for (int y_loop = 0; y_loop<font.chars[ch].image->size; y_loop++) {
            volatile int bit_loop = 7;
            buffer = rle.get_byte();
            for (volatile int x_loop = 0; x_loop<font.chars[ch].image->x; x_loop++){
                if ((buffer & (1<<bit_loop)) != 0){
                    this->set_pixel( x_loop+x, y_loop+y, color );
                };
                bit_loop--;
                if (bit_loop<0) {
                    if ((x_loop+1) != font.chars[ch].image->x){	// Это защита от кривого отображения. На случай, если шрифт имеет кратную 8 битам ширину. 8, 16, 24...
                        bit_loop = 7;
                        buffer = rle.get_byte();
                    };
                };
            };
        };
    } else {
        for (int y_loop = 0; y_loop < font.chars[ch].image->size; y_loop++){	// Идем по строкам. Но каждая строка = byte_w байт. Это учитываем ниже.
            uint16_t loop_byte = 0;	// Перемещаемся по массиву строки.
            uint8_t bit_string = 8;	// Для отслеживания положения в uint8_t.
            for (int x_loop = 0; x_loop<font.chars[ch].image->x; x_loop++){	// Только считаем колличество байт в строке. Обращаемся к пикселю через переменные выше.
                if ((buf[y_loop*byte_w+loop_byte] & (1<<(bit_string-1))) != 0){
                    this->set_pixel( x_loop+x, y_loop+y, color);
                };
                bit_string--;				// Перемещаемся по элементам массива строки. Слева направо. От младшего к старшему.
                if (bit_string == 0){
                    bit_string = 8;
                    loop_byte++;
                }
            };
        };
    };
}


// rle = 1 - расшифровываем с помощью RLE.
// rle = 0 - фигачим напрямую.
void simple_drawing_lib::print_string (const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t color, uint8_t rle) const {
    uint16_t x_point = x, y_point = y;			// Чтобы следить за расположением пикселей.
    uint16_t loop_char = 0;
    while((string[loop_char] != 0)&&(string[loop_char] != '\n')){
        this->print_char (font, x_point, y_point, string[loop_char], color, rle);
        x_point += font.chars[(uint8_t)string[loop_char]-0x20].image->x; // Смещаемся по x.
        loop_char++;
    };
}

// Печатаем первые number_char символов строки.
// rle = 1 - расшифровываем с помощью RLE.
// rle = 0 - фигачим напрямую.
void simple_drawing_lib::print_string_number ( const font_t& font, uint8_t x, uint8_t y, char *string, uint8_t number_char, uint8_t color, uint8_t rle ) const {
    uint16_t x_point = x, y_point = y;			// Чтобы следить за расположением пикселей.
    for (uint8_t loop_char = 0; loop_char<number_char; loop_char++){
        this->print_char  ( font, x_point, y_point, string[loop_char], color, rle );
        if (string[loop_char] >= 0x20){	// Защита от пустой строки.
            x_point += font.chars[(uint8_t)string[loop_char]-0x20].image->x; // Смещаемся по x.
        };
    };
}

// Рисуем рамку заданной толщины.
// Толщина от 0.
void    simple_drawing_lib::draw_frame ( const uint8_t& x, uint8_t& y, const uint8_t& width, const uint8_t& height, const uint8_t& thickness, const uint8_t& color) const {
    // Рисуем рамку 4-мя прямоугольниками.
    this->fill_rect( x, y, x+width, y+thickness, color);	// Слева направо от левого верхнего угла.
    this->fill_rect( x, y, x+thickness, y+height,	color);	// С левого верхнего угла вниз.
    this->fill_rect( x, y+height, x+width, y+height-thickness,color);	// С левого верхнего угла вниз.
    this->fill_rect( x+width, y, x+width-thickness, y+height,color);	// С правого верхнего вниз.
}

// Рисуем прямоугольник и внутри него рамку заданной толщены и цвета.
void    simple_drawing_lib::fill_rect_and_frame ( const uint8_t& x, uint8_t& y, const uint8_t& width, const uint8_t& height, const uint8_t& thickness, const uint8_t& color_rect, const uint8_t& color_line ) const {
    this->fill_rect( x + thickness + 1, y + thickness + 1, x - thickness + width - 1, y + height - thickness - 1, color_rect);
    this->draw_frame( x, y, width, height, thickness, color_line);
}

void simple_drawing_lib::clear ( void ) const {
    memset(this->buf, 0, 1024);
}
