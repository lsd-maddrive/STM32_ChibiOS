# Заготовка под новый проект для конкретного контроллера для ChibiOS

## Зачем?

В [папке с демо проектами основного репозитория ChibiOS](https://github.com/ChibiOS/ChibiOS/tree/stable_17.6.x/demos/STM32) уже есть огромное количество заготовок для разных контроллеров. В данном случае шаблоны дорабатываются для упрощения работы с ними и реализации новых проектов.

> Всю подготовку рекоменуется выполнять после установки ChibiStudio на [Linux](../../Startup/Linux_setup.md) или [Windows](../../Startup/Windows_setup.md).

## Поехали!

Заходим в [папку с демо проектами основного репозитория](https://github.com/ChibiOS/ChibiOS/tree/stable_17.6.x/demos/STM32) и находим свой контроллер.

> Для нашего примера будем готовить под **NUCLEO-STM32F401RE**.

- Любым доступным способом стягиваем папку `RT-STM32F401RE-NUCLEO64` к себе и кладем в `workspace` директорию ChibiStudio.

- Скопируйте все файлы (кроме README.md) из данной папки (имеется ввиду данная папка на репозитории) в папку проекта.

- В начало файла Makefile (после первых комментов) добавляем инструкцию
```Makefile
# Build global options
# NOTE: Can be overridden externally.
#

include project.mk

# Compiler options here.
ifeq ($(USE_OPT),)
```

- (Makefile) Переменной `USE_OPT` добавляем значение переменной `PROJECT_OPT`:
```Makefile
ifeq ($(USE_OPT),)
  USE_OPT = -O2 -ggdb -fomit-frame-pointer -falign-functions=16 $(PROJECT_OPT)
endif
```

- (Makefile) Переменной `CSRC` добавляем значение переменной `PROJECT_CSRC` (убираем `main.c` и другие явные записи `.c` файлов):
```Makefile
CSRC = $(STARTUPSRC) \
       $(KERNSRC) \
       $(PORTSRC) \
       $(OSALSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) \
       $(TESTSRC) \
       $(STREAMSSRC) \
       $(PROJECT_CSRC)
```

- (Makefile) Переменной `CPPSRC` добавляем значение переменной `PROJECT_CPPSRC`:
```Makefile
# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(PROJECT_CPPSRC)
```

- (Makefile) Переменной `INCDIR` добавляем значение переменной `PROJECT_INCDIR`:
```Makefile
INCDIR = $(CHIBIOS)/os/license \
         $(STARTUPINC) $(KERNINC) $(PORTINC) $(OSALINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) $(TESTINC) $(STREAMSINC) \
         $(CHIBIOS)/os/various $(PROJECT_INCDIR)
```

- (Makefile) Переменной `ULIBS` добавляем значение переменной `PROJECT_LIBS`:
```Makefile
# List all user libraries here
ULIBS = $(PROJECT_LIBS)
```

- (Makefile) Находим следующий кусочек кода:
```Makefile
# Project, sources and paths
#

# Define project name here
PROJECT = ch

# Imported source files and paths
CHIBIOS = ../../..
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
```

- (Makefile) Переменной `CHIBIOS` присваиваем значение `$(CHIBIOS_ROOT)`:
```Makefile
CHIBIOS = $(CHIBIOS_ROOT)
```

- (Makefile) Если в наборе `include` директив есть строка подключения тестов - удаляем, так как она занимает лишнее время компиляции:
```Makefile
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk
# Other files (optional).
# >>> TODO - Delete next line!
include $(CHIBIOS)/test/rt/test.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F401xE.ld
```

- (Makefile) Если в наборе `include` директив нет директивы подключения `streams.mk`, добавляем (это позволит работать с функциями `ch*`, например, `chpritnf()`):
```Makefile
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk
# Other files (optional).
# >>> TODO - Add next line!
include $(CHIBIOS)/os/hal/lib/streams/streams.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F401xE.ld
```

- В `main.c` проекта напишите код:
```c++
#include <ch.h>
#include <hal.h>

static THD_WORKING_AREA(waThread, 128);
static THD_FUNCTION(Thread, arg)
{
    (void)arg;

    while (true)
    {
        chThdSleepSeconds(1);
    }
}

/*
 * Application entry point.
 */
int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL /* arg is NULL */);

    while (true)
    {
        chThdSleepSeconds(1);
    }
}
```

- В файле `load.bat` замените значение опции `-f` на соответствующее значение вашей платы:
```bash
openocd -s "%CHIBISTUDIO%/tools/openocd/scripts" -f board/st_nucleo_f4.cfg -c "program %BIN% reset exit 0x08000000"
```

> Можно попробовать просто заменить цифру на соответсвующую вашему МК, но если это не сработает - надо обратиться в папку утилиты `openocd` и посмотреть соответствующий файл конфигурации.

- В файле `openocd_server.bat` замените значения опций `-f` и `-c` на соответствующее значение вашей платы:
```bash
openocd -s "%CHIBISTUDIO%/tools/openocd/scripts" -f board/st_nucleo_f4.cfg -c "stm32f4x.cpu configure -rtos auto;"
```

- Далее по [инструкции работы с новым проектом](../New_project.md) можно импортировать проект в ChibiStudio (+ при желании изменить имя проекта) и проверить компиляцию. В результате у нас имеется готовый проект, с которым можно вести работу.
