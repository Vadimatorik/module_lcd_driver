/*
 * В данном каталоге содержатся интерфейсы, один из которых
 * обязательно должен быть унаследован конечным драйвером LCD
 * Например, для монохромного LCD на базе st7565 с разрешением 128x64
 * должен быть унаследован mono_lcd_128x64_base.
 */

#include "mc_hardware_interfaces_base.h"

/*
 * Этот класс является чисто виртуальным базовым классом
 * для всех монохромных LCD с расширением 128x64.
 */
class monoLcd128x64Base {
public:
	constexpr monoLcd128x64Base() {}
	/*
	 * Метод должен сбрасывать LCD в начальное состояние (но не включать!).
	 */

	virtual BASE_RESULT  reset		( void ) = 0;

   /*
	* Включить/выключить дисплей.
	*/
	virtual BASE_RESULT on			( void ) = 0;
	virtual BASE_RESULT off			( void ) = 0;


	/*
	 * Вывод данных из буфера в сам LCD.
	 */
	virtual BASE_RESULT update		( void ) = 0;


	/*
	 * Очистка непосредственно экрана.
	 */
	virtual BASE_RESULT clear		( void ) = 0;

	/*
	 * Очистка буфера экрана.
	 */
	virtual void bufClear			( void ) = 0;

	virtual ~monoLcd128x64Base() {}
};
