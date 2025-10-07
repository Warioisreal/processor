CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations \
         -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body \
         -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline \
         -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
         -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default \
         -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
         -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector \
         -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer \
         -Wlarger-than=8192 -fPIE -Werror=vla \
         #-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
DEBUG_FLAGS = -DDEBUG
LDFLAGS =
CC = g++

# Разделяем исходные файлы на библиотечные и приложения
LIB_SOURCES = file_func.cpp work_with_buffer.cpp executor.cpp
APP_SOURCES = main.cpp

# Пути к проекту стека
STACK_DIR = ../stack
STACK_INCLUDE = $(STACK_DIR)/include
STACK_LIB = $(STACK_DIR)/lib

# Пути для библиотеки калькулятора
CALC_LIB_DIR = lib
CALC_INCLUDE_DIR = include
CALC_LIB_NAME = libcalculator.a

# Тип сборки проекта по умолчанию
BUILD_TYPE ?= release

OBJDIR_RELEASE = obj/release
OBJDIR_DEBUG = obj/debug

EXECUTABLE_RELEASE = binary_file
EXECUTABLE_DEBUG = binary_file_debug

ifeq ($(BUILD_TYPE), debug)
    CFLAGS += $(DEBUG_FLAGS)
    OBJDIR = $(OBJDIR_DEBUG)
    EXECUTABLE = $(EXECUTABLE_DEBUG)
    STACK_TARGET = debug_lib
else
    OBJDIR = $(OBJDIR_RELEASE)
    EXECUTABLE = $(EXECUTABLE_RELEASE)
    STACK_TARGET = release_lib
endif

# Флаги для инклудов и линковки
INCLUDE_FLAGS = -I$(STACK_INCLUDE) -I$(CALC_INCLUDE_DIR)
LIB_FLAGS = -L$(STACK_LIB) -lstack -L$(CALC_LIB_DIR) -lcalculator

# Объектные файлы
LIB_OBJECTS = $(addprefix $(OBJDIR)/, $(LIB_SOURCES:.cpp=.o))
APP_OBJECTS = $(addprefix $(OBJDIR)/, $(APP_SOURCES:.cpp=.o))
OBJECTS = $(LIB_OBJECTS) $(APP_OBJECTS)

all: release

debug:
	$(MAKE) BUILD_TYPE=debug $(EXECUTABLE_DEBUG)

release:
	$(MAKE) BUILD_TYPE=release $(EXECUTABLE_RELEASE)

# Цели для библиотеки калькулятора
lib: $(CALC_LIB_DIR)/$(CALC_LIB_NAME)

debug_lib:
	$(MAKE) BUILD_TYPE=debug lib

release_lib:
	$(MAKE) BUILD_TYPE=release lib

# Исполняемый файл (зависит от библиотек стека и калькулятора)
$(EXECUTABLE): $(APP_OBJECTS) | $(STACK_LIB)/libstack.a $(CALC_LIB_DIR)/$(CALC_LIB_NAME)
	$(CC) $(LDFLAGS) $(APP_OBJECTS) -o $@ $(LIB_FLAGS)

# Создание библиотеки калькулятора
$(CALC_LIB_DIR)/$(CALC_LIB_NAME): $(LIB_OBJECTS)
	mkdir -p $(CALC_LIB_DIR)
	mkdir -p $(CALC_INCLUDE_DIR)
	ar rcs $@ $^

    # Подключается [project].h файл со всеми include в main.cpp
    #cp $(MAIN_HEADER) $(INCLUDE_DIR)/


    #Копируем заголовочные файлы в include
	cp *.h $(CALC_INCLUDE_DIR)/ 2>/dev/null || true

# Зависимость от библиотеки стека
$(STACK_LIB)/libstack.a:
	$(MAKE) -C $(STACK_DIR) $(STACK_TARGET)

# Компиляция объектных файлов
$(OBJDIR)/%.o: %.cpp | $(STACK_LIB)/libstack.a
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean:
	rm -rf obj
	rm -rf $(CALC_LIB_DIR)
	rm -rf $(CALC_INCLUDE_DIR)
	rm -f $(EXECUTABLE_RELEASE) $(EXECUTABLE_DEBUG)
