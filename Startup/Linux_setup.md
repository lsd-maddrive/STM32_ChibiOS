# Установка и настройка окружения для работы с STM32 под ChibiOS под Linux

Для установки ChibiStudio необходимо выполнить три пункта:

  - Развернуть дистрибутив самой студии;
  - Установить Java Run-Time Environment;
  - Установить драйвер для ST-Link.
  
  Ниже разберем каждый пунт с блэкджеком и примерами.
  
## Загрузка дистрибутива

Этот пункт самый простой, надо скачать [архив](https://disk.yandex.ru/d/_-4XPyA-fdCSyw) из облака и разархивировать.

Для того чтобы разархивировать дистрибутив, перенесите его в желаемое место и выполните в терминале
```bash
tar -zxvf *name*
```

## Установка Java

На Linux Java представлена в двух вариантах:
  - Java Runtime Environment (JRE) - среда для запуска приложений
  - Java Development Kit (JDK) - среда для разработки
  
Достаточно установить JRE, но можно и JDK (вторая включает в себя первую). Установка зависит от выбранного дистрибутива Linux.

Archlinux
```bash
sudo pacman -Syu jre-openjdk
sudo pacman -Syu jdk-openjdk
```

Debian / Ubuntu
```bash
sudo apt update
sudo apt install default-jre
sudo apt install default-jdk
```

## Установка драйвера ST-Link

Так как подключение к программатору происходит через USB, необходимо убедиться, что установлен его драйвер.

Archlinux
```bash
sudo pacman -Syu libusb
```

Debian / Ubuntu
```bash
sudo apt update
sudo apt install libusb
```

Затем можно установить и драйвер для ST-Link.

Archlinux
```bash
sudo pacman -Syu stlink
```

Debian / Ubuntu
```bash
sudo apt update
sudo apt install stlink-tools
```

Установка на этом завершена, можно приступать к запуску.

# Запуск IDE

В дирректории студии находятся пять папок:
  - chibios203 - исходники chibios. Кстати, там же можно найти множество примеров;
  - eclipse_2020-03 - IDE Eclipse для помощи в написании кода;
  - Templates - заготовки проектов под разные платформы;
  - tools - компилятор и полезные тулзы;
  - workspace - рабочая папка, где будут лежать Ваши проекты;
  
 и один shell скрипт. Для запуска Eclipse необходимо выполнить этот скрипт:
 
```bash
sh ChibiStudio
```

Из терминала делать это может оказаться не очень удобно, так что можно захотеть сделать ярлык. Для этого сделайте файл *Желаемое_имя*.desktop со следующим содержанием

```bash
[Desktop Entry]
Version=1.0
Type=Application
Name=ChibiStudio
Comment=
Exec=sh ChibiStudio
Icon=
Path= *Сюда напишите путь до папки с Чиби, например, /home/ChibiStudio*
Terminal=false
StartupNotify=false
```

При запуске может произойти ошибка о недопустимости расположения рабочей папки. В этом случае согласитесь и укажите путь до папки workspace.

Если ошибка не произошла, на всякий случай все же укажите путь до workspace. Сделать это можно с помощью File -> Switch Workspace -> Other

На этом ChibiStudio полностью готова поспособствовать во всех запланированных великих свершениях!*

*Если это не так, обязательно дайте знать, будем править.
