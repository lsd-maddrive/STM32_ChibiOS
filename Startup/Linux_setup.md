# Установка и настройка окружения для работы с STM32 под ChibiOS под Linux или "Хацкер - начало" =)

Весь туториал построим по принципу сборки и настройки.

> Если используется оболочка shell не стандартная `bash`, а, например, `zsh`, то все прописывания в `bashrc` заменяются на соответствующий оболочке rc-файл (для `zsh` - `zshrc`). 

> Спойлер - в конце есть инструкция для ленивых =)

> Опции команд постараюсь не объяснять, чтобы у вас был шанс самостоятельно разобраться. Например, по команде `mkdir` можно посмотреть хелп так `mkdir --help` или `man mkdir`. Аналогично по другим утилитам.

Начнем с того, что в домашней директории создадим директорию `local_libs`:
```bash
mkdir -p ~/local_libs
```

В нее мы будем складывать все наши собранные либы и бинарники. Также рядом создайте директорию `ext_repos`, в которую будем класть исходники перед сборкой.

```bash
mkdir -p ~/ext_repos
```

# Необходимые пакеты из репозитория

Ставим пакеты, которые нам понадобятся для сборки
```bash
sudo apt install cmake libtool pkg-config autoconf automake texinfo
```
и работы утилит
```bash
sudo apt install libusb-1.0-0-dev default-jdk default-jre
```

# ST-Link

Набор утилит для прошивки контроллера, стягиваем с [этого репозитория](https://github.com/texane/stlink) в `ext_repos`:
```bash
cd ~/ext_repos; git clone https://github.com/texane/stlink.git
```

Заходим в стянутую директорию `stlink` и видим, что есть файл `CMakeFiles.txt`, что говорит о поддержке системы сборки `cmake`. Значит пользоваться будем ею. Создаем папку для сборки `_build`, переходим в нее:
```bash
mkdir -p _build; cd _build
```

Вызываем `cmake` с настройкой на инсталяцию в директорию `$HOME/local_libs`:
```bash
cmake -D CMAKE_INSTALL_PREFIX=$HOME/local_libs ..
```

> Помним, что `~` = `$HOME`? Когда вы устанавливаете пути куда-то, лучше пользоваться `$HOME`, в случае набора команд - можно и `~`.

Собираем и устанавливаем:
```bash
make -j4
sudo make install
```

> Почитайте про опцию `-j` у команды `make`. Я вызывал с такой опцией, потому что у меня 4 потока на процессор.

При установке нужен `sudo`, так как сразу ставятся правила для правильной работы драйвера при подключении программатора. Это влияет на права файлов и папок в `local_libs`, так что вернем нам наши права:
```bash
sudo chown -R `whoami`:`whoami` ~/local_libs
```

> whoami - утилита, которая отображает имя пользователя, а в команде мы используем кавычки, чтобы результат выполнения утилиты подставился в качестве строки в команде.

> Вот так вы познакомились со сборкой пакета через `cmake`.

## Настройка системы

Утилиту для программирования освоили, надо настроить пути до исполняемого файла, делаем:
```bash
export PATH="$HOME/local_libs/bin:$PATH"
```
Добавляем эту строку в конец файла `~/.bashrc`, чтобы переменная устанавливалась при каждом создании сессии (терминала).

Далее проверяем, что у нас вылазиет ошибка:
```bash
st-flash --version

st-flash: error while loading shared libraries: libstlink.so.1: cannot open shared object file: No such file or directory
```

Ошибка сообщает, что не может найти библиотеку, что достаточно распространено, есть два пути решения:
- Установить путь для поиска библиотек в переменную LD_LIBRARY_PATH;
- Установить системный путь поиска библиотек.

Дальше на ваш выбор, сделаем оба варианта:

### Вариант 1 - LD_LIBRARY_PATH

Этот вариант больше подходит для установки путей до библиотек для запуска программ.

Устанавливаем переменную:
```bash
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/local_libs/lib"
```
Добавляем эту строку в конец файла `~/.bashrc`, чтобы переменная устанавливалась при каждом создании сессии (терминала).

В результате:
```bash
st-flash --version

v1.5.1-15-g3295ab4
```

### Вариант 2 - ldconfig

Этот вариант больше подходит для установки путей до библиотек для сборки программ/других библиотек.

Системным он называется, так как установка пути таким образом распространяется на всех юзеров.

Убедимся, что система не видит этой библиотеки:
```bash
ldconfig -p | grep libstlink.so.1
```

Вывода нет, а значит фильтр (`grep libstlink.so.1`) не нашел совпадений. Ну значит сообщим системе, где искать. Сделаем файл и запишем в него путь до библиотек:
```bash
sudo echo "$HOME/local_libs/lib" | sudo tee /etc/ld.so.conf.d/user_libs.conf
```

> Тут используется не просто перенаправление в файл, а еще и команда `tee`. Сделано это, чтобы выполнить с `sudo` команду, наподобие этой `echo "$HOME/local_libs/lib" > /etc/ld.so.conf.d/user_libs.conf`

Подгрузим пути и проверим еще раз:
```bash
sudo ldconfig
ldconfig -p | grep libstlink.so.1

libstlink.so.1 (libc6,x86-64) => /home/alexey/local_libs/lib/libstlink.so.1
```

> Username у каждого свой =)

В результате:
```bash
st-flash --version

v1.5.1-15-g3295ab4
```

# Компилятор

На этом этапе мы не будем собирать компилятор с нуля. Просто скачаем [отсюда](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads). Переходим к архиву в папку (например, если он в папке `~/Downloads`) и распаковываем:
```bash
cd ~/Downloads
tar xvf gcc-arm-none-eabi-8-2018-q4-major-linux.tar.bz2
```

В результате получаем папку `gcc-arm-none-eabi-8-2018-q4-major`. Если посмотреть внутрь, то структура очень напоминает структуру, которую создал `st-link` при установке - `bin`, `lib`, `share`... Теперь нужно начинку разместить в папку `~/local_libs`:
```bash
cp -r gcc-arm-none-eabi-8-2018-q4-major/* ~/local_libs
```

Проверяем:
```bash
arm-none-eabi-gcc -v

Using built-in specs.
COLLECT_GCC=arm-none-eabi-gcc
COLLECT_LTO_WRAPPER=/home/alexey/local_libs/bin/../lib/gcc/arm-none-eabi/8.2.1/lto-wrapper
Target: arm-none-eabi
Configured with: /tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/src/gcc/configure --target=arm-none-eabi --prefix=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native --libexecdir=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/lib --infodir=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/share/doc/gcc-arm-none-eabi/info --mandir=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/share/doc/gcc-arm-none-eabi/man --htmldir=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/share/doc/gcc-arm-none-eabi/html --pdfdir=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/share/doc/gcc-arm-none-eabi/pdf --enable-languages=c,c++ --enable-plugins --disable-decimal-float --disable-libffi --disable-libgomp --disable-libmudflap --disable-libquadmath --disable-libssp --disable-libstdcxx-pch --disable-nls --disable-shared --disable-threads --disable-tls --with-gnu-as --with-gnu-ld --with-newlib --with-headers=yes --with-python-dir=share/gcc-arm-none-eabi --with-sysroot=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/install-native/arm-none-eabi --build=x86_64-linux-gnu --host=x86_64-linux-gnu --with-gmp=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/build-native/host-libs/usr --with-mpfr=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/build-native/host-libs/usr --with-mpc=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/build-native/host-libs/usr --with-isl=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/build-native/host-libs/usr --with-libelf=/tmp/jenkins/jenkins-GCC-8-build_toolchain_docker-519_20181216_1544945247/build-native/host-libs/usr --with-host-libstdcxx='-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' --with-pkgversion='GNU Tools for Arm Embedded Processors 8-2018-q4-major' --with-multilib-list=rmprofile
Thread model: single
gcc version 8.2.1 20181213 (release) [gcc-8-branch revision 267074] (GNU Tools for Arm Embedded Processors 8-2018-q4-major)
```

Если после копирования компилятор не виден (не находит утилиту) - либо не туда скопировали (проверьте `~/local_libs/bin`), либо не установили переменную `PATH`.

# ChibiOS исходники

Для начала стянем исходники, в папку `ext_repos`:
```bash
cd ~/ext_repos
git clone -b stable_17.6.x https://github.com/KaiL4eK/ChibiOS.git
```

Далее остается только установить переменную указывающую на корень ChibiOS:
```bash
export CHIBIOS_ROOT="$HOME/ext_repos/ChibiOS"
```
Добавляем эту строку в конец файла `~/.bashrc`, чтобы переменная устанавливалась при каждом создании сессии (терминала).

На этом установка все =)

# Соберем первый проект под ChibiOS

О том, как создавать проект написано [здесь](New_project.md). После создания проекта переходим в папку и вызывает сборку:
```bash
make -j4
```

Пустой новый проект должен собраться без проблем и это говорит о том, что с этого момента вы можете работать с проектом в любимой IDE =).

```bash
Makefile:96: /home/alexey/ext_repos/ChibiOS_bad/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f7xx.mk: No such file or directory
Makefile:98: /home/alexey/ext_repos/ChibiOS_bad/os/hal/hal.mk: No such file or directory
Makefile:99: /home/alexey/ext_repos/ChibiOS_bad/os/hal/ports/STM32/STM32F7xx/platform.mk: No such file or directory
Makefile:100: /home/alexey/ext_repos/ChibiOS_bad/os/hal/boards/ST_NUCLEO144_F767ZI/board.mk: No such file or directory
Makefile:101: /home/alexey/ext_repos/ChibiOS_bad/os/hal/osal/rt/osal.mk: No such file or directory
Makefile:103: /home/alexey/ext_repos/ChibiOS_bad/os/rt/rt.mk: No such file or directory
Makefile:104: /home/alexey/ext_repos/ChibiOS_bad/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk: No such file or directory
Makefile:107: /home/alexey/ext_repos/ChibiOS_bad/os/hal/lib/streams/streams.mk: No such file or directory
Makefile:227: /home/alexey/ext_repos/ChibiOS_bad/os/common/startup/ARMCMx/compilers/GCC/rules.mk: No such file or directory
make: *** No rule to make target '/home/alexey/ext_repos/ChibiOS_bad/os/common/startup/ARMCMx/compilers/GCC/rules.mk'.  Stop.
```

Вот такой набор ошибок говорит о том, что путь до ChibiOS плохой, проверьте его!

# Прошивка

Для прошивки используйте скрипт `load.sh` внутри шаблона проекта. При корректно установленном пути до `st-link` бинарников все должно пройти гладко =).

Если при использовании скрипта `load.sh` у вас возникает ошибка
```bash
./load.sh: Permission denied
```
по каким-то причинам у вас сбросились права на исполнение скрипта, установите их командой
```bash
chmod +x load.sh
```

# Отладчик

Допустим в будущем вы соберетесь отлаживать проект, давайте соберем отладчик. Стягиваем с [этого репозитория](https://github.com/ntfreak/openocd.git): 
```bash
cd ~/ext_repos; git clone https://github.com/ntfreak/openocd.git
```

> На самом деле официального репозитория на `github` у `openocd` нет, есть только гитовское зеркало - https://repo.or.cz/w/openocd.git, с которого можно стянуть. Мы же пользуемся другим зеркалом, так как у сервака проблемы со скачиванием..)

Переходим в папку `cd openocd` и делаем сборку (взято прямо из README):
```bash
./bootstrap
./configure --prefix=$HOME/local_libs
make install -j4
```

> Вот так вы познакомились со сборкой пакета через `configure`.

После установки проверяем:
```bash
openocd -f board/st_nucleo_f7.cfg

Open On-Chip Debugger 0.10.0+dev-00693-gfd8a3c9 (2019-02-15-22:57)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
adapter speed: 2000 kHz
adapter_nsrst_delay: 100
srst_only separate srst_nogate srst_open_drain connect_deassert_srst
srst_only separate srst_nogate srst_open_drain connect_deassert_srst
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : clock speed 2000 kHz
Error: open failed
```

Это вывод запуска при отсутствующем подключении к контроллеру. Видно, что настройки прошли, но подключение не произошло (а к чему, соответственно? =)).

# Eclipse

Тянем архив с [облака](https://drive.google.com/open?id=1z3HXfS4O87fx9uob131J1U2Xe4XaW6RG). Распаковываем:
```bash
tar xzvf ChibiStudio17v1_Linux.tar.gz
```

Переходим в `ChibiStudio` и видим следующие папки:
  - chibios176 - директория исходников ChibiOS
  - eclipse - директория IDE Eclipse
  - templates - шаблоны проектов
  - tools - все те утилиты, которые мы ставили, с подготовленным скриптом настройки
  - workspace - рабочее пространство, на него надо указать при запуске, чтобы Eclipse корректно подтянул все плагины

Перейдя в папку `ChibiStudio`, запустить IDE можно локально:
```bash
./ChibiStudio
```

или указав переменную PATH до этой папки и тогда можно вызывать из любого места:
```bash
export PATH="$PATH:$HOME/ChibiStudio"
ChibiStudio
```

Добавляем строку установки переменной `PATH` в конец файла `~/.bashrc`, чтобы переменная устанавливалась при каждом создании сессии (терминала).


# For Lazy

Ставим пакеты:
```bash
sudo apt install libusb-1.0-0-dev default-jdk default-jre
```

Тянем архив с [облака](https://drive.google.com/open?id=1d_n7p8SNheWWZ7LWnjXoyWJ8H5a-aWcX), распаковываем:
```bash
tar xzvf ChibiStudio17_Linux.tar.gz
```

Пишем в конец `~/.bashrc`:
```bash
source $HOME/ChibiStudio/tools/setup_vars.sh
```

> Обратите внимание, что путь указывает на расположение папки `ChibiStudio` в домашней директории.

Перезапускаем консоль - PROFIT!!!

У нас есть рабочий ChibiStudio из любого места, а также настроенные пути до компилятора и других утилит (`PATH`), необходимых библиотек (`LD_LIBRARY_PATH`) и исходников ChibiOS (`CHIBIOS_ROOT`).

> Если ранее не были устанановлены драйверы для `stlink`, то переходим в папку `ChibiStudio/tools` и вызываем скрипт установки драйверов `install_udev_stlink.sh` (нужен интернет и права sudo).
