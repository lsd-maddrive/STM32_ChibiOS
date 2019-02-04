# Работа с внешними прерываниями на STM-ке

## Первый шаг - выбор пина

Стандартная процедура по включению модуля внешних прерываний (EXT) и выявлению незадействованных пинов микроконтроллера (МК). 

> Предположим, что мы хотим генерировать прерывание при нажатии на кнопку, установленную на плате - синяя такая. Она подключена к пину PC13.  

Для работы с внешними прерываниями доступны все пины I/O, любой из них можно указать в одном из 16-ти каналов модуля внешних прерываний. 

## Второй шаг - Познакомим STM-ку с EXT

Нужно дать понять МК, что мы хотим использовать модуль EXT. Как это сделать - написано в [основах работы с модулями](Basics.md) :grin:.

## Третий шаг - настройка модуля EXT 

```cpp
static void extcb( EXTDriver *extp, expchannel_t channel )
{
    extp = extp;
    channel = channel;

    palToggleLine( LINE_LED1 ); 
}

static const EXTConfig extcfg = {
  .channels = 
  {
    [0]  = {EXT_CH_MODE_DISABLED, NULL},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    [3]  = {EXT_CH_MODE_DISABLED, NULL},
    [4]  = {EXT_CH_MODE_DISABLED, NULL},
    [5]  = {EXT_CH_MODE_DISABLED, NULL},
    [6]  = {EXT_CH_MODE_DISABLED, NULL},
    [7]  = {EXT_CH_MODE_DISABLED, NULL},
    [8]  = {EXT_CH_MODE_DISABLED, NULL},
    [9]  = {EXT_CH_MODE_DISABLED, NULL},
    [10] = {EXT_CH_MODE_DISABLED, NULL},
    [11] = {EXT_CH_MODE_DISABLED, NULL},
    [12] = {EXT_CH_MODE_DISABLED, NULL},
    [13] = {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb}, //PC13 = Button
    [14] = {EXT_CH_MODE_DISABLED, NULL},
    [15] = {EXT_CH_MODE_DISABLED, NULL},
  }
};

```

В структуре `EXTConfig extcfg` происходит настройка пина на канал модуля внешних прерываний. Поскольку нам нужно, чтобы программа реагирована на нажатие кнопки (PC13), то нужно выбрать 13-й канал EXT и записать туда порт С. Нумерация каналов начинается с 0 сверху вниз.  

* `EXT_CH_MODE_FALLING_EDGE` - выбор режима работы (по срезу сигнала);
* `EXT_CH_MODE_AUTOSTART` - канал запускается вместе с запуском драйвера `extStart()`, иначе требуется использовать функцию `extChannelEnable()`
* `EXT_MODE_GPIOC` - порт, которому принадлежит выбранный пин МК;
* `extcb` - имя функции-callback, которая вызывается, когда происходит прерывание. 

В функции `extcb` строки 
```cpp
extp = extp;
channel = channel;
``` 
нужны только для того, чтобы избежать warnings от компилятора. 

В функции `main()` достаточно только запустить модуль EXT. 

```cpp
void main( void )
{
    chSysInit();
    halInit();
    
    extStart( &EXTD1, &extcfg );
    
    while (true)
    {
      chThdSleepMilliseconds( 10 );
    }
}
```
В STM единственный модуль EXT c 16-ю каналами. Поэтому при старте всегда будет модуль №1. 

* `extStart( &EXTD1, &extcfg );` - запуск модуля EXT1 с конфигурацией `extcfg`.
