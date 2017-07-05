
#include "mono_lcd_lib_cfg.h"

/*
 * В данном каталоге содержатся интерфейсы, один из которых
 * обязательно должен быть унаследован конечным драйвером LCD
 * Например, для монохромного LCD на базе st7565 с разрешением 128x64
 * должен быть унаследован mono_lcd_128x64_base.
 */


/*
 * Этот класс является чисто виртуальным базовым классом
 * для всех монохромных LCD с расширением 128x64.
 */
class mono_lcd_128x64_base {
public:
    constexpr mono_lcd_128x64_base() {}
    /*
     * Инициализация mutex-ов и прочего, не требующего
     * пользовательской ОС.
     */

    virtual void    reinit          ( const spi_base* spi_obj ) const= 0;

    /*
     * Метод должен сбрасывать LCD в начальное состояние (но не включать!).
     */

    virtual void  reset ( void ) const = 0;

   /*
    * Включить/выключить дисплей.
    */
    virtual void on ( void ) const = 0;
    virtual void off ( void ) const = 0;


    /*
     * Вывод данных из буфера в сам LCD.
     */
    virtual  void update ( void ) const = 0;


    /*
     * Очистка непосредственно экрана.
     */
    virtual  void clear ( void ) const = 0;

    /*
     * Очистка буфера экрана.
     */
    virtual void buf_clear ( void ) const = 0;

    virtual ~mono_lcd_128x64_base() {}
};
