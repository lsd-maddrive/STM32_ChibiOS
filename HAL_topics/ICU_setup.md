# Работа с модулем ICU на STM-ке c ChibiOS/HAL

ICU = Input Capture Unit = Модуль захвата импульса

## Первый шаг - выбор пина

Стандартная процедура по включению модуля захвата импульсов (ICU) и выявлению незадействованных пинов микроконтроллера (МК). 

> Предположим, что мы хотим ловить периодический сигнал с 2-х каналов и определять ширину "пойманного" импульса. Для этого выберем пины, которые принадлежат первому каналу таймера №9 - PE5 и первому каналу таймера №8 - PC6. 

> Супер важно! В ChibiOS драйвер ICU может работать только с первым или вторым каналом таймера. Только! Таким образом, если мы выбрали для ICU8 ногу PC6 (первый канал), то больше никто не может зайдествовать ни второй канал, ни третий, ни четвертый. Вывод: для ICU под один сигнал уходит целый таймер. *Я бы тут задумался об использовании EXT и GPT =)*

## Второй шаг - Познакомим STM-ку с ICU

Нужно дать понять МК, что мы хотим использовать модуль ICU. Как это сделать - написано в [основах работы с модулями](Basics.md) :grin:.

В случае с ICU в файле `mcuconf.h` выбираем подмодуль исходя из выбранных ранее таймеров. 

## Третий шаг - настройка модуля ICU 

Немножко конфига в студию:

```cpp
static const ICUConfig icucfg1 = {
  .mode         = ICU_INPUT_ACTIVE_HIGH,  // Trigger on rising edge
  .frequency    = 1000000,                // do not depend on PWM freq
  .width_cb     = icucb1,            
  .period_cb    = NULL,
  .overflow_cb  = NULL,
  .channel      = ICU_CHANNEL_1,          // for Timer
  .dier         = 0
};

static const ICUConfig icucfg2 = {
  .mode         = ICU_INPUT_ACTIVE_HIGH,  // Trigger on rising edge
  .frequency    = 1000000,                // do not depend on PWM freq
  .width_cb     = icucb2,  
  .period_cb    = NULL,
  .overflow_cb  = NULL,
  .channel      = ICU_CHANNEL_1,          // for Timer
  .dier         = 0
};
```

Поскольку мы хотим ловить 2 сигнала, то, соответственно, нужна конфигурация двух модулей ICU. 

Пройдемся по полям конфигурации:

* `mode` - режим работы захвата, возможные значения:  
  - `ICU_INPUT_ACTIVE_HIGH` - началом импульса сигнала считается фронт, концом - срез  
  - `ICU_INPUT_ACTIVE_LOW` - началом импульса сигнала считается срез, концом - фронт  
* `frequency` - частота работы таймера, по которому будет производиться замер тиков  
* `width_cb` - функция-обработчик, которая вызывается, когда были пойманы начало и конец импульса сигнала
* `period_cb` - функция-обработчик, которая вызывается, когда был пойман период сигнала (от начала импульса до начала импульса)
* `overflow_cb` - функция-обработчик, которая вызывается, когда таймер переполнился
* `channel` - выбор канала таймера, возможные значения:  
  - `ICU_CHANNEL_1` - сигнал подключен к первому каналу таймера
  - `ICU_CHANNEL_2` - сигнал подключен ко второму каналу таймера
* `dier` - регистр DIER таймера (нативный регистр), обычно ставится в 0

> При настройке не забывайте про переполнение таймера!

В функции `main()` нужно инициализировать модули и указать функции пинов.

```cpp
void main( void )
{
    icuStart( &ICUD9, &icucfg1 );
    /* Dont forget to setup pin for timer function - sample for palSetPadMode() */
    palSetPadMode( GPIOE, 5, PAL_MODE_ALTERNATE(3) );
    icuStartCapture( &ICUD9 );
    icuEnableNotifications(  &ICUD9 );

    icuStart( &ICUD8, &icucfg2 );
    /* Dont forget to setup pin for timer function - sample for palSetLineMode() and PAL_LINE() */
    palSetLineMode( PAL_LINE( GPIOC, 6 ), PAL_MODE_ALTERNATE(3) );
    icuStartCapture( &ICUD8 );
    icuEnableNotifications( &ICUD8 );

    while( 1 )
    {
      chThdSleepMilliseconds( 10 );
    }
}
```

* `icuStart( &ICUD9, &icucfg1 )` - конфигурирует модуль и запускает (периферийно) модуль ICU9;
* `icuStartCapture(  &ICUD9 )` - запускает процесс захвата импульса;
* `icuEnableNotifications( &ICUD9 )` - запуск оповещений

Примеры определения функций обработки полученной ширины импульса сигнала (`width_cb`), назначили мы их `icucb1` / `icucb2`. 

```cpp
uint32_t channel_1 = 0; 
uint32_t channel_2 = 0; 

static void icucb1(ICUDriver *icup)
{
  channel_1 = icuGetWidthX(icup);   // ... X - can work anywhere
                                    // return width in ticks
}

static void icucb2(ICUDriver *icup)
{
  channel_2  = icuGetWidthX(icup);  // ...X - can work anywhere
                                    // return width in ticks
}
```

* `icuGetWidthX( icup )` - функция, которая возвращает ширину последнего пойманного импульса. Подробнее про неё можно почитать [здесь](http://chibios.sourceforge.net/docs3/hal/group___i_c_u.html#gaaca94f57ae11a7b9511e3c8878af93a5). Говорят, вызывать ее нужно обязательно в callback функции.



### Полезные ссылки
* [Функционал ICU](http://chibios.sourceforge.net/docs3/hal/group___i_c_u.html)


