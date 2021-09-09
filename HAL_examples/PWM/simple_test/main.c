#include "ch.h"
#include "hal.h"

// Положим указатель на драйвер в переменную
static PWMDriver *pwm1Driver = &PWMD1;

PWMConfig pwm1conf = {
    // Укажем частоту 500кГц (предделитель равен 436, так что значение допустимое)
    .frequency = 500000,
    // Период равен 10000 тактам таймера, то есть 0,02 секунды
    .period    = 10000,
    // Не будем использовать прерывание по окончанию периода
    .callback  = NULL,
    // Настроим только первый кнал в состояние ACTIVE_HIGH и так же не будем использовать прерывание
    .channels  = {
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    // Регистры оставим в покое. Обратите внимание, advanced функции таймера выключены, поэтому регистра bdtr нет
    .cr2        = 0,
    .dier       = 0
};

int main(void)
{
    chSysInit();
    halInit();
    // Переведем ногу E9 в альтернативный режим 1, так как имнно он является первым каналом первого таймера
    palSetLineMode( PAL_LINE( GPIOE, 9 ),  PAL_MODE_ALTERNATE(1) );
    // Запустим модуль в работу
    pwmStart( pwm1Driver, &pwm1conf );
    // Запустим канал с коэффициентом заполнения 50% (здесь третий аргумент это количество тактов, 5000 / 10000 = 0,5)
    pwmEnableChannel( pwm1Driver, 0, 5000 );

    while (TRUE);
}
