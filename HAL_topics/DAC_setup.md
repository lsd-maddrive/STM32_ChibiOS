# Как завести DAC (ЦАП) на STM-ке

## Первый шаг - выбор пина

Стандартная процедура по включению модуля ЦАП (DAC) и выявлению незадействованных пинов микроконтроллера (МК). 

Модуль ЦАП на STM32 Nucleo F767ZI состоит из 2-х каналов ([PA4 - DACout1, PA5 - DACout2](https://www.st.com/content/ccc/resource/technical/document/datasheet/group3/c5/37/9c/1d/a6/09/4e/1a/DM00273119/files/DM00273119.pdf/jcr:content/translations/en.DM00273119.pdf#page=69))

> Предположим, что мы хотим использовать только один канал - PA4.  

## Второй шаг - Познакомим STM-ку с DAC

Нужно дать понять МК, что мы хотим использовать модуль ЦАП (DAC). Как это сделать - написано в [основах работы с модулями](Basics.md) :grin:

## Третий шаг - настройка модуля DAC

```cpp
static const DACConfig dac_cfg = {
    /* Initial value of DAC out */
    .init         = 0,
    /*
     * Mode of DAC:
     *      DAC_DHRM_12BIT_RIGHT - 12 bit with right alignment
     *      DAC_DHRM_12BIT_LEFT  - 12 bit with left alignment
     *      DAC_DHRM_8BIT_RIGHT  - 8 bit no alignment (half of dacsample_t [uint16_t] type)
     */
    .datamode     = DAC_DHRM_12BIT_RIGHT,
    /* Direct register set, future used for triggering DAC */
    .cr           = 0
};
``` 

В функции `main()` необходимо прописать конфигурацию пина и запустить модуль

```cpp
void main( void )
{
    palSetLineMode( PAL_LINE( GPIOA, 4 ), PAL_MODE_INPUT_ANALOG );

    /* Start DAC driver with configuration */
    dacStart( &DACD1, &dac_cfg );

    while( true )
    {
    	chThdSleepMilliseconds( 10 );
    }
}
```

Чтобы записать значение в канал ЦАП можно использовать следующую функцию:

```cpp
dacPutChannelX( &DACD1, channel, dacInput );
```

где
* `dacInput` - целочисленное значение в диапазоне [0; 4096] при 12-битном режиме работы ЦАП, [0; 255] при 8-битном режиме работы ЦАП
* `channel` - номер канала ЦАП (нумерация с 0-ля)

Хорошей практикой является создание функций, аргументами которых является величина напряжения, а не условные единицы. 
Чтобы перевести вольты в значения ЦАП, можно воспользоваться следующей формулой:

```cpp
#define VOLTAGE_2_DAC(v)  ((v) / REF_V * DAC_BIT)
```

где 
* `REF_V` - величина опорного напряжения [B];
* `DAC_BIT` - максимальное значение (зависит от битности), если 12-битный режим - 4096, 8-битный режим - 255