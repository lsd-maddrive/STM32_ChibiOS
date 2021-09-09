#include "ch.h"
#include "hal.h"

// Первая callback функция
void cb_fun1(PWMDriver *pwmp)
{
    (void)pwmp;
    palToggleLine(LINE_LED1);
}

// Вторая callback функция
void cb_fun2(PWMDriver *pwmp)
{
    (void)pwmp;
    palToggleLine(LINE_LED2);
}

// Структура конфигурации
static PWMConfig pwmcfg = {
  // Частота 10кГц
  .frequency = 100000,
  // Период 5000 тактов, или 0,05с
  .period = 5000,
  // Ссылка на функцию
  .callback = cb_fun1,
  .channels = {
   // Включаем первый канал в комплементарном режиме, с вызовом второй функции
   {PWM_OUTPUT_ACTIVE_HIGH | PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH, cb_fun2},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  .cr2 = 0,
  // Задаем мёртвое время 2мкс
  .bdtr = 0b11010110,
  .dier = 0
};
// Сохраняем указатель на первый драйвер в переменную
static PWMDriver *pwm = &PWMD1;

int main(void) {
    halInit();
    chSysInit();
    // Запускаем ШИМ
    pwmStart(pwm, &pwmcfg);
    // Разрешаем вызов первой функции
    pwmEnablePeriodicNotification(pwm);
    // Устанавливаем ноги первого канала первого таймера в режим ШИМ
    palSetLineMode(PAL_LINE(GPIOE, 9), PAL_MODE_ALTERNATE(1));
    palSetLineMode(PAL_LINE(GPIOE, 8), PAL_MODE_ALTERNATE(1));
    // Начинаем генерацию сигнала на ногах
    pwmEnableChannel(pwm, 0, PWM_PERCENTAGE_TO_WIDTH(pwm, 5000));
    // Разрешаем вызов второй функции
    pwmEnableChannelNotification(pwm, 0);
    uint16_t duty = 0;
    while (true) {
        // Рисуем пилообразный сигнал
        duty += 100;
        if(duty > 10000) duty = 0;
        pwmEnableChannel(pwm, 0, PWM_PERCENTAGE_TO_WIDTH(pwm, duty));
        chThdSleepMilliseconds(200);
    }
}
