# Инструкция по работе с драйвером ICU

## Содержание <a id="content"></a>

* [Что такое ICU?](#icu)
* [Выбор таймера](#tim)
* [Настройка конфигурации](#conf)
* [Что модуль вообще делает](#work)
* [Работа модуля](#usage)
* [Пример](#example)

## Что такое ICU? <a id="icu"></a>

Очередная задача, решаемая таймером микроконтроллера - Input Capture Unit. Модуль, который умеет определять временные характеристики входного ШИМ сигнала. Можно узнать период сигнала и длительность пульса.

Одно из применений данного модуля - управление роботом с пульта, ICU позволит вам изменять длительность импульсов от пульта, а дальше уже можете делать то управление, которое вам нужно. Другими словами, этот модуль даёт возможность определять фронт и срез сигнала. 

По сути на нижнем уровне всё делает библиотека таймера, так что имеет смысл сначала [прочитать про GPT](GPT_setup.md#gpt).

[Содержание](#content)

## Выбор таймера <a id="tim"></a>

Как и со всеми остальными драйверами, нужно в `halconf.h` включить модуль определив `HAL_USE_PWM` в состояние `TRUE`, а в `mcuconf.h` аналогично включить один из предложенных таймеров среди `STM32_ICU_USE_TIMx`, где `x` - номер таймера.

При выборе таймера руководствоваться можно тремя факторами - разрешение, максимальная частота, доступные для использования ноги. По частоте и разрешению всё написано в инструкции на таймер. Ноги выбираем исходя из [таблички альтернативных функций пинов](https://www.st.com/content/ccc/resource/technical/document/datasheet/group3/c5/37/9c/1d/a6/09/4e/1a/DM00273119/files/DM00273119.pdf/jcr:content/translations/en.DM00273119.pdf#page=89) (**Не забываем, что для каждого контроллера своя документация!**).

`ICU` позволяет работать только с первыми двумя каналами таймера, так что ищите в табличке ячейки с `TIMx_CHy`, где `x` - желаемый таймер, а `y` - 1 или 2.

[Содержание](#content)

## Настройка конфигурации <a id="conf"></a>

Аналогично любому другому модулю `HAL`, в `ICU` надо определить структуры драйвера и конфигурации. Структура драйвера уже готова и имеет тип и название вида `ICUDriver ICUDx`, где `x` - номер выбранного таймера. А вот конфигурацию надо заполнить.

Выглядит конфиг следующим образом:

'''c
typedef struct {
  icumode_t                 mode;
  icufreq_t                 frequency;
  icucallback_t             width_cb;
  icucallback_t             period_cb;
  icucallback_t             overflow_cb;
  icuchannel_t              channel;
  uint32_t                  dier;
  uint32_t                  arr;
} ICUConfig;
'''

* `mode` позволяет выбрать, что мы будет считать за сигнал, который нужно измерить. Есть всего два варианта: `ICU_INPUT_ACTIVE_HIGH` и `ICU_INPUT_ACTIVE_LOW`. Очевидно, первый вариант будет считать за начало сигнала нарастающий фронт, а второй вариант - падающий.

* `frequency` задаёт частоту тактирования таймера в Гц. Важно помнить, что из-за целочисленной математики предделителя есть ограничения на возможные значения. Подробнее в статье про [`GPT`](GPT_setup.md#gpt).

* `width_cb`, `period_cb` и `overflow_cb` хранят указатели на функции, которые вызовутся при наступлении соответствующего события. События - окончание импульса, начало нового периода и переполнение счётчика, соответственно. Про сами функции чуть ниже. Если реагировать на все или какие-либо события нет необходимости - ставим в ненужные поля `NULL`.

* `channel` задаёт используемый канал. Возможны варианты `ICU_CHANNEL_1` и `ICU_CHANNEL_2`.

Последние два поля это регистры `DMA/Interrupt Enable` и `Auto-Reload`. Для работы модуля они не нужны, так что в принципе в структуру их и писать не обязательно. Если уж очень хочется их определить, то в `dier` по умолчанию все нули, а в `arr` - все единицы.

Функция обработчик события может иметь любое имя, не должна ничего возвращать и принимает один аргумент - указатель на структуру драйвера `ICU`:

```c 
void cb_fun(ICUDriver *icup) {
	some code
}
```

Сами по себе функции вызываться не будут, предварительно надо разрешить уведомления, но об этом чуть ниже.

[Содержание](#content)

## Что модуль вообще делает <a id="work"></a>

`ICU` использует два регистра счётчика для того, чтобы узнать сколько тактов таймера длится импульс и сколько длится период импульсов. Так как длительности записываеются количеством тактов, чтобы привести их к времени необходимо поделить полученное значение на частоту указанную в структуре конфигурации. 

Отсюда можно сделать вывод, что частота влияет на разрешение измеряемого сигнала: **чем больше частота, тем точнее измерение**. С другой стороны, регистр счётчика ограничен по размеру (16 или 32 бита, в зависимости от таймера), а значит **чем больше частота, тем меньше максимальная длительность, которую можно измерить**.

Если по какой-то причине сигнал не измерить, например, на ноге нет периодического сигнала, или пульс длинее, чем максимальная длительность с учетом выбранной частоты, случится событие переполнения.

[Содержание](#content)

## Работа модуля <a id="usage"></a>

API `ChibiOS` предлагает следующие функции:

```c
  void icuStart(ICUDriver *icup, const ICUConfig *config);
  void icuStop(ICUDriver *icup);
  void icuStartCapture(ICUDriver *icup);
  void icuStartCaptureI(ICUDriver *icup);
  bool icuWaitCapture(ICUDriver *icup);
  void icuStopCapture(ICUDriver *icup);
  void icuStopCaptureI(ICUDriver *icup);
  void icuEnableNotifications(ICUDriver *icup);
  void icuEnableNotificationsI(ICUDriver *icup);
  void icuDisableNotifications(ICUDriver *icup);
  void icuDisableNotificationsI(ICUDriver *icup);
  uint32_t icuGetWidthX(ICUDriver *icup)
  uint32_t icuGetPeriodX(ICUDriver *icup)
```

Как обычно, перед началом работы с модулем запускаем его с помощью `icuStart()`, передав в него указатель на драйвер и указатель на заполненный конфиг. Если решили что закончили, можем остановить с помощью `icuStop()`.

Когда модуль приведён в работу, единственное что мы можем сделать, это запустить измерения. Сделать это можно вызвав функцию `icuStartCapture()` или аналогичную функцию `I`-класса, если делаем это из прерывания. С этого момента модуль перейдёт в режим ожидания, пока не случится первый нарастающий фронт, после этого драйвер оказывается в состоянии `Active` и будет считать пульсы до тех пор, пока его не остановить с помощью `icuStopCapture()` или `icuStopCaptureI()` (если из прерывания). 

> Однако, судя по экспериментам, драйвер сразу после начала захвата переходит в состояние `Active` и кидает ошибку переполнения, если не дать на ногу импульсов. Возможно вмешались наводки и система таки увидела пульс, хотя это довольно сомнительно. В любом случае это не должно как-то повлиять на логику работы программы.

С помощью `icuGetPeriodX()` и `icuGetWidthX()` можно узнать период сигнала и ширину импульса в тактах таймера, соответственно. `X` в конце имени функции означает, что функция `X`-класса, то есть её можно вызывать откуда угодно.

Функция `icuWaitCapture()` позволяет дождаться окончания измерения. Возвращаемое значение говорит удалось ли измерить что-то или нет. `false` означает что измерение прошло успешно, `true` в свою очередь говорит что произошло переполнение. То есть, если мы хотим узнать длительность пульсов без использования прерываний, можно написать что-то вроде такого:

```c 
if(icuWaitCapture(&ICUD3)) {
    <some code to handle overflow>
}
else duration = icuGetWidthX(&ICUD3);
```

Чтобы воспользоваться прерываниями (если определили их в структуре конфигурации), необходимо вызвать функцию `icuEnableNotifications()`. Есть аналогичная функция `I`-класса. После этого при возникновении событий будет вызываться соответствующая функция, в которой можно что-нибудь сделать, например узнать длительность импульса с помощью той же функции `icuGetWidthX()`. Если возникает необходимость остановить отслеживание событий есть функция `icuDisableNotifications()` и аналог `I`-класса. Это не остановит измерения, только прервёт вызов функций-обработчиков. Согласно документации включение и выключение уведомлений можно делать только после `icuStartCapture()`, но эксперименты показали, что от порядка вызова этих функций ничего не зависит. С другой стороны несколько нарушается логика, так что всё же стоит делать как предлагают.

[Содержание](#content)

## Пример <a id="example"></a>

Код для платы NUCLEO-F767ZI

Будем генерировать ШИМ сигнал и подавать его на ногу работающую как вход для `ICU` модуля. ШИМ сигнал генерируется на ноге `E9`, читаем ногу `A6`, между ногами кинута перемычка. Коэффициент заполнения сигнала задается из терминала. Для работы с терминалом используется поток (stream), прочитать про него можно в инструкции к модулю [Serial](SD_setup.md#chprintf). В этот же терминал непрерывно выводится текущее значение ширины измеряемого импульса. Проект можно найти [тут](../HAL_examples/ICU).

<details>
<summary>Код main.c под спойлером</summary>

```c
#include "ch.h"
#include "hal.h"
#include "stdlib.h"
// Для работы с терминалом
#include "uart_debug.h"

// Конфигурация ШИМ модуля для генерации сигнала
static PWMDriver* pwm1 = &PWMD1;

static PWMConfig pwm_conf = {
    .frequency = 50000,
    .period = 10000,
    .channels = {
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL}
    },
    .cr2 = 0,
    .dier = 0
};

// Настройка ICU

// Здесь храним измеренную длительность импулься
uint32_t duration = 0;

// Использовать будем третий таймер
static ICUDriver* icu3 = &ICUD3;


// Определяем функции обработчики событий
void cbWidth(ICUDriver *icup){
    // Просто чтобы убрать ворнинги о неиспользуемой переменной
    (void) icup;
    // Сразу после окончания импульса определяем его длительность
    duration = icuGetWidthX(icu3);
    palSetLine(LINE_LED1);
}

void cbPeriod(ICUDriver *icup){
    (void) icup;
    palClearLine(LINE_LED1);
}

void cbOverflow(ICUDriver *icup){
    (void) icup;
    // Мигаем красной лампочкой, если измерить не удаётся
    palToggleLine(LINE_LED3);
}

//Определяем конфигурацию
static ICUConfig icu_conf = {
    // Частота 50 кГц. То есть один такт соответствует 20 мкс
    .frequency = 50000,
    // Измеряем длительность высокого состония сигнала
    .mode = ICU_INPUT_ACTIVE_HIGH,
    // Определяем какая функция в каком случае должна сработать
    .width_cb = cbWidth,
    .period_cb = cbPeriod,
    .overflow_cb = cbOverflow,
    // Указываем канал таймера где будем мерить
    .channel = ICU_CHANNEL_1
    // Регистры DIER и ARR игнорируем
};

int main(void) {
    // Запускаем ChibiOS
    halInit();
    chSysInit();

    // Запустим юарт и отправим первую строку для теста
    debug_stream_init();
    dbgprintf("Start\n\r");

    // Запустим модуль ШИМ и начнём генерацию на ноге E9
    palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));
    pwmStart(pwm1, &pwm_conf);
    pwmEnableChannel(pwm1, 0, PWM_PERCENTAGE_TO_WIDTH(pwm1, 2500));

    // Запустим модуль ICU и переведём ногу А6 в режим первого канала третьего таймера
    palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(2));
    icuStart(icu3, &icu_conf);

    // Запустим измерение и разрешим вызов callback функций
    icuStartCapture(icu3);
    icuEnableNotifications(icu3);

    while (true) {
        // Читаем из терминала число от 1 до 9
        char val = sdGetTimeout(&SD3, TIME_MS2I(500));
        // Отсеиваем перевод строки, которые также прилетают из терминала
        // Да, это отвратительно, но слишком лениво делать лучше
        sdGetTimeout(&SD3, TIME_IMMEDIATE);
        // Преобразуем символ в цифру. Если прилетела не цифра, будет 0
        uint8_t num = atoi(&val);
        // Обновляем коэффициент заполнения сигнала если не 0
        if(num != 0) pwmEnableChannel(pwm1, 0, PWM_PERCENTAGE_TO_WIDTH(pwm1, num*1000));
        // Выводим измеренное значение в терминал
        dbgprintf("%d\n\r", duration);
    }
}

```

</details>


[Содержание](#content)
