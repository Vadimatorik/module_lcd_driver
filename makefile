#**********************************************************************
# Для сборки библиотеки LCD драйверов (mono_lcd_lib).
#**********************************************************************
LCD_LIB_H_FILE		:= $(shell find mono_lcd_lib/ -maxdepth 3 -type f -name "*.h" )
LCD_LIB_CPP_FILE	:= $(shell find mono_lcd_lib/ -maxdepth 3 -type f -name "*.cpp" )
LCD_LIB_DIR		:= $(shell find mono_lcd_lib/ -maxdepth 3 -type d -name "*" )
LCD_LIB_PATH		:= $(addprefix -I, $(LCD_LIB_DIR))
LCD_LIB_OBJ_FILE	:= $(addprefix build/obj/, $(LCD_LIB_CPP_FILE))
LCD_LIB_OBJ_FILE	:= $(patsubst %.cpp, %.o, $(LCD_LIB_OBJ_FILE))

build/obj/mono_lcd_lib/%.o:	mono_lcd_lib/%.cpp $(USER_CFG_H_FILE) $(FREE_RTOS_H_FILE) $(MK_INTER_H_FILE)
	@echo [CPP] $<
	@mkdir -p $(dir $@)
	@$(CPP) $(CPP_FLAGS) $(LCD_LIB_PATH) $(STM32_F2_API_PATH) $(FREE_RTOS_PATH) $(USER_CFG_PATH) $(LCD_LIB_OPTIMIZATION) $(MK_INTER_PATH) -c $< -o $@

# Добавляем к общим переменным проекта.
PROJECT_PATH			+= $(LCD_LIB_PATH)
PROJECT_OBJ_FILE		+= $(LCD_LIB_OBJ_FILE)