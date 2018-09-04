/*

@startuml

namespace MonoLcd {

interface Base {
	{abstract}{method}+	McHardwareInterfaces::BaseResult	reset		( void )
	{abstract}{method}+	McHardwareInterfaces::BaseResult	on		( void )
	{abstract}{method}+	McHardwareInterfaces::BaseResult	off		( void )
	{abstract}{method}+	McHardwareInterfaces::BaseResult 	update	( void )
	{abstract}{method}+	McHardwareInterfaces::BaseResult 	lcdClear	( void )
	{abstract}{method}+	void							bufClear	( void )
}

}

@enduml

*/

#include "mc_hardware_interfaces_base.h"

namespace MonoLcd {

/*
 * Этот класс является чисто виртуальным базовым классом
 * для всех монохромных LCD с расширением 128x64.
 */
class Base {
public:
	constexpr Base() {}
	/*
	 * Метод должен сбрасывать LCD в начальное состояние (но не включать!).
	 */

	virtual McHardwareInterfaces::BaseResult	reset			(	void	) = 0;

   /*
	* Включить/выключить дисплей.
	*/
	virtual McHardwareInterfaces::BaseResult	on				(	void	) = 0;
	virtual McHardwareInterfaces::BaseResult	off				(	void	) = 0;


	/*
	 * Вывод данных из буфера в сам LCD.
	 */
	virtual McHardwareInterfaces::BaseResult 	update			(	void	) = 0;


	/*
	 * Очистка непосредственно экрана.
	 */
	virtual McHardwareInterfaces::BaseResult 	lcdClear		(	void	) = 0;

	/*
	 * Очистка буфера экрана.
	 */
	virtual void								bufClear		(	void	) = 0;

	virtual ~Base() {}
};

}

