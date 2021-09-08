#include "ch.h"
#include "hal.h"

// Зададим функцию прерывания по окончанию преобразования
void endcbfun(ADCDriver* adcp)
{
    (void)adcp;
    // Просто моргаем лампочкой
    palToggleLine(LINE_LED2);
}

// И определим функцию если произойдет ошибка
void errcbfun(ADCDriver* adcp, adcerror_t err)
{
    (void)adcp;
    // Если ошибку бросит watchdog, будем мигать другой лампочкой
    if(err == ADC_ERR_WATCHDOG)
        palToggleLine(LINE_LED1);
}

ADCConversionGroup adcconf1 = {
    // Без непрерывного режима
    .circular = FALSE,
    // Все еще 1 канал
    .num_channels = 1,
    // Укажим определенные ранее функции
    .end_cb = endcbfun,
    .error_cb = errcbfun,
    // Включим watchdog, разрешим ему поднимать флаг прерывания, скажем ему смотреть на каждый канал по отдельности и укажем третий канал
    .cr1 = ADC_CR1_AWDEN | ADC_CR1_AWDIE | ADC_CR1_AWDSGL | ADC_CR1_AWDCH_0 | ADC_CR1_AWDCH_1,
    // Оставим запуск преобразования на ChibiOS
    .cr2 = ADC_CR2_SWSTART,
    // Поставим самое долгое время преобразования, 480 тактов (потому что можем)
    .smpr1 = 0,
    .smpr2 = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_480),
    // Зададим пределы для сторожевой системы. Сверху предела нет, снизу примерно четверть от максимального напряжения
    .htr = 0xFFF,
    .ltr = 1000,
    // Укажем третий канал
    .sqr1 = 0,
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3)

};

// Определим буфер для результата преобразования
static adcsample_t buf[4] = {0, 0, 0, 0};

int main(void) {
    halInit();
    chSysInit();
    // Переведем ногу в режим на выход и включим АЦП
    palSetLineMode(PAL_LINE(GPIOA, 3), PAL_MODE_INPUT_ANALOG);
    adcStart(&ADCD1, NULL);
    while (true) {
        // Раз в секунду делаем преобразование. Если напряжение выше нижнего порога, то моргает вторая лампочка (LINE_LED2, то есть все ок), если ниже, то моргает первая лампочка (LINE_LED1, то есть все не ок). Причем первое преобразование в любом случае выполнится и первый элемент обновится, но тут случится прерывание, преобразование остановится и оставшиеся элементы буфера не изменятся. С другой стороны, если решили считать это за ошибку, использовать данные скорее всего и так не имеет смысла.
        adcConvert(&ADCD1, &adcconf1, buf, 4);
        chThdSleepMilliseconds(1000);
    }
}