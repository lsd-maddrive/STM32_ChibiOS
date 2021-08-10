# Инструкция по работе с драйвером PAL

`PAL` - драйвер, управляющий портами ввода-вывода, также известными как `GPIO`. 

Все функции данного модуля начинаются с префикса `pal`, что позволяет пользоваться автозаполнением (ctrl + пробел при неполном введении функции в Eclipse) для просмотра всех возможных функций.

Чтобы включить драйвер, необходимо в `halconf.h` установить `HAL_USE_PAL` в состояние `TRUE` (см. [сюда](https://github.com/lsd-maddrive/STM32_ChibiOS/blob/stable_17.6.x/HAL_topics/Basics.md)).

Для управления ногами контроллера модуль оперирует следующими понятиями:

  - `pad` - одна нога контроллера. Задается с помощью порта и бита. Например `(GPIOA, 0)` означает нога `A0`.
  - `line` - альтернативный способ задать ногу контроллера. Отличие от `pad` в том, что `line` задается одним параметром, а не двумя. Для определения `line` можно воспользоваться дефайном `PAL_LINE(<port>, <bit>)`.
  - `port` - все ноги одного порта, например `GPIOA`.
  - `group` - несколько ног контроллера, заданные в виде: порт, маска, сдвиг влево. Например, `(GPIOB, 0x4081, 0)` соответствует выбору ног `B14`, `B7` и `B0` (три светодиода на `NUCLEO-F767ZI`). Или `(GPIOB, 0x81, 7)` выбирает ноги `B14` и `B7` (если не очень понятно, поиграйте с калькулятором и сравните двоичные и шестнадцатиричные записи использованных значений).

Если используемый контроллер установлен на поддерживаемой плате, в заголовочном файле board.h можно найти набор дефайнов для выведенных куда-либо ног. Напрмиер, на плате `NUCLEO-F767ZI` среди прочих определены имена `LINE_LED1`, `LINE_LED2`, `LINE_LED3`, для трех светодиодов, доступных разработчику.

Для того, чтобы сделать что-либо с ногами контроллера, нужно выбрать соответствующий режим их работы. Для этого есть несколько функций:

  - `palSetLineMode(line, mode)` - выбирает режим для одной линии.
  - `palSetPadMode(port, pad, mode)` - также выбирает режим для одной линии, но через `pad`.
  - `palSetGroupMode(port, mask, offset, mode)` - выбирает режим для нескольких линий.
  
Режим определяет поведение ноги. Например, будет она на вход, выход или будет обеспечивать работу какого-либо модуля. Для удобного выбора режима в ChibiOS есть несколько дефайнов:

  - `PAL_MODE_RESET` - устанавливает (сбрасывает) режим линии в исходное для используемой платформы состояние.
  - `PAL_MODE_UNCONNECTED` - безопасное состояние для неиспользуемых ног.
  - `PAL_MODE_INPUT` - линия на вход без подтягивающих резисторов.
  - `PAL_MODE_INPUT_PULLUP` - линия на вход с подтяжкой к питанию.
  - `PAL_MODE_INPUT_PULLDOWN` - линия на вход с подтяжкой к земле.
  - `PAL_MODE_INPUT_ANALOG` - линия на вход для АЦП.
  - `PAL_MODE_OUTPUT_PUSHPULL` - нога на выход в режиме `PUSHPULL` (активный выход, при низком логическом уровне напряжение на выводе равно 0, при высоком - напряжение близко к напряжению питания МК)
  - `PAL_MODE_OUTPUT_OPENDRAIN` - нога на выход в режиме `OPENDRAIN` (режим открытого стока, не связанного с питанием напрямую. Обычно требует внутреннего или внешнего подтягивающего к питанию резистора)
  - `PAL_MODE_ALTERNATE(n)` - выбор альтерантивного режима работы ноги. n - номер режима, который можно узнать из даташита на контроллер с помощью таблицы alternate function mapping.

Предположим, у абстрактного контроллера к ноге A5 подключен светодиод. Для того чтобы иметь возможность этот светодиод зажечь переведем ногу в режим работы на выход.
```c
palSetLineMode(PAL_LINE(GPIOA, 5), PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
palSetGroupMode(GPIOA, 0x30, 0, PAL_MODE_OUTPUT_PUSHPULL);
palSetGroupMode(GPIOA, 1, 5, PAL_MODE_OUTPUT_PUSHPULL);
```

Все четыре представленные варианта делают одно и то же, выбирайте какой комфортнее использовать.

Ну и наконец, непосредственно работа с настроенной ногой. Случаи алтернативных режимов тут рассматриваться не будут, а значит это оставляет все два варианта: прочитать состояние на ноге и записать его.

Сначала разберемся с записью. Сделать это можно с помощью четырех операций:

  - `Set` - устанавливает высокий уровень на ноге.
  - `Clear` - устанавливает низкий уровень на ноге.
  - `Toggle` - изменяет состояние ноги на противоположное текущему.
  - `Write(state)` - устанавливает выбранное состояние.
  
Эти операции можно выполнять как над одиночными линиями, так и над группами линий с помощью функций:

  - `palSetLine(line)`
  - `palSetPad(port, pad)`
  - `palSetPort(port)`
  - `palClearLine(line)`
  - `palClearPad(port, pad)`
  - `palClearPort(port)`
  - `palToggleLine(line)`
  - `palTogglePad(port, pad)`
  - `palTogglePort(port)`
  - `palWriteLine(line, state)`
  - `palWritePad(port, pad, state)`
  - `palWritePort(port, state)`
  - `palWriteGroup(port, mask, offset, state)`
  
Для операции `Write` определены два дефайна `PAL_HIGH` и `PAL_LOW`, означающие высокое и низкое состояния (state) соответственно.

В случае записи нескольких линий, например, с помощью `palWritePort(port, state)`, state является 16-битным числом, означающим желаемую конфигурацию состояний. То есть `palWritePort(GPIOA, 7)`, означает установку ног А0 - А2 в высокое состояние, а А3 - А15 в низкое.

Для чтения линий необходимо выполнить функцию read. Сделать это можно с помощью следующих функций:

  - `palReadLine(line)`
  - `palReadPad(port, pad)`
  - `palReadPort(port)`
  - `palReadGroup(port, mask, offset)`
  - `palReadLatch(port)`

Поведение функций вероятнее всего должно быть очевидным, однако остановимся на последней чуть подробнее. Дело в том, что если записать какое-либо состояние для ноги, настроенной на вход, это не изменит напряжения на ноге, однако установленное значение останется в регистре и если нога изменит режим работы на output, установленное состояние применится. Если же читать ногу настроенную на выход, то `palReadLine(line)` вернет состояние, соответствующее напряжению на ноге. А `palReadLatch(port)` позволяет узнать записанные состояния, независимо от режима работы ноги и уровня напряжения на ней.

Начиная с ChibiOS18 внешние прерывания переехали из модуля ext в pal.

Чтобы воспользоваться внешними прерываниями необходимо в `halconf.h` установить `PAL_USE_CALLBACKS` в состояние `TRUE`.

Настройка прерываний заключается в разрешении прерываний и установке функции-обработчика.

Для разрешения/запрета прерываний есть следующие функции:
  
  - `palEnableLineEvent(line, mode)`
  - `palEnablePadEvent(port, pad, mode)`
  - `palDisableLineEvent(line)`
  - `palDisablePadEvent(port, pad)`
  
У каждой из этих функций есть вариант вариант API `I`-класса (оканчивается на `I`, `palEnableLineEvent()` -> `palEnableLineEventI()`). Такие функции можно испоьзовать в обработчиках прерываний и в зонах системной блокировки.

Допустимыми режимами для прерываний являются следующие варианты:

  - `PAL_EVENT_MODE_RISING_EDGE`
  - `PAL_EVENT_MODE_FALLING_EDGE`
  - `PAL_EVENT_MODE_BOTH_EDGES`
  
Из названий очевидно, что это нарастающий фронт, падающий фронт или оба события.

Назначение функции обработчика осуществляется с помощью функций

  - `palSetLineCallback(line, cb_function, arg)`
  - `palSetPadCallback(port, pad, cb_function, arg)`
  
Функция обработчик должна принимать в себя один аргумент - указатель на передаваемый аргумент. Например:

```c
void cb_function(void* args)
{
	some_type arg = *((some_type*)args);
	...
}
```

Обратите внимание, функция всегда принимает указатель типа void, то есть неопределенный тип. Чтобы воспользоваться этим аргументом его необходимо привести к желаемому типу указателя, например, `(uint8_t\*)args`. Затем, если это необходимо, можно сделать разыменовывание, чтобы получить значение по переданному адресу.

Если передавать никаких аргументов не хочется, в `palSetLineCallback()` можно указать третьим аргументом `NULL`.

И последнее, есть функция ожидания фронта на ноге.

  - `palWaintPadTimeout(prot, pad, timeout)`
  - `palWaintLineTimeout(line, timeout)`
  
Функция ждет заданное время и возвращает значение типа `msg_t`. возможные значения:

  - `MSG_OK` - Оно случилось;
  - `MSG_TIMEOUT` - Оно не случилось за отведенное время;
  - `MSG_RESET` - Оно не случилось, но прерывание было запрещено до окончания отведенного времени.
  
Время указывается в тиках, для того чтобы не приходилось сильно думать о переводе времени в тики есть помощник `chTimeMS2I()` (milliseconds to interval) (аналогично есть с секундами и микросекундами, а так же обратные преобразования). Так же можно установить значение таймаута в `TIME_INFINITE` и `TIME_IMMEDIATE`. Последнее кажется бессмысленным, но таймаут используется и в других модулях.

## Ну и наконец, пример

Код для платы NUCLEO-F767ZI
```c
#include "ch.h"
#include "hal.h"

// callback функция, которая должна сработать по настроенному событию
void palcb_button(void* args){
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
    if (arg == 5) palToggleLine(LINE_LED3);
}

int main(void) {
    halInit();
    chSysInit();
    // Задаем число, которое передадим в функцию-обработчик
    uint8_t arg = 5;
    // Настраиваем ногу с лампочкой на выход
    palSetLineMode(LINE_LED3, PAL_MODE_OUTPUT_PUSHPULL);
    // А ногу с кнопкой на вход
    palSetLineMode(GPIOC_BUTTON, PAL_MODE_INPUT_PULLDOWN);
    // Разрешаем прерывание, указываем callbcak функцию и передаем указатель на аргумент
    palEnableLineEvent(GPIOC_BUTTON, PAL_EVENT_MODE_RISING_EDGE);
    palSetLineCallback(GPIOC_BUTTON, palcb_button, &arg);
    while (1) {
        chThdSleepMilliseconds(1);
    }
}
```