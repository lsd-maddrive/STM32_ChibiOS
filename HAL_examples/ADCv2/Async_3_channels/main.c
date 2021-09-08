#include "ch.h"
#include "hal.h"

// Зададим функцию прерывания по окончанию преобразования
void endcbfun(ADCDriver* adcp)
{
    (void)adcp;
    // Просто моргаем лампочкой. В этот раз лампочка будет мигать так часто, что глазом мигания видно не будет, просто будет гореть
    palToggleLine(LINE_LED2);
}

// В этот раз посмотрим другу ошибку. В DMAFAILURE попадает в том числе некорректный запуск преобразования
void errcbfun(ADCDriver* adcp, adcerror_t err)
{
    (void)adcp;
    if(err == ADC_ERR_DMAFAILURE)
        palToggleLine(LINE_LED1);
}

ADCConversionGroup adcconf1 = {
    // Включаем непрерывный режим
    .circular = TRUE,
    // Укажем три канала
    .num_channels = 3,
    // Все те же callback функции
    .end_cb = endcbfun,
    .error_cb = errcbfun,
    // Watchdog работает, но смотрит все каналы сразу
    .cr1 = ADC_CR1_AWDEN | ADC_CR1_AWDIE,
    // За пуск преобразования отвечает все еще ChibiOS
    .cr2 = ADC_CR2_SWSTART,
    // Укажем каждому каналу свое время преобразования, не, нуачо
    .smpr1 = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144) | ADC_SMPR1_SMP_AN13(ADC_SAMPLE_15),
    .smpr2 = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_480),
    // Пределы для сторожевой собаки
    .htr = 0xFFF,
    .ltr = 100,
    // Укажем каналы для первых трех элементов
    .sqr1 = 0,
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN13)

};

// Зададим буфер. Обратите внимание, в предыдщем примере он был на три элемента из-за глубины, в этот раз из-за количества каналов
static adcsample_t buf[3] = {0};

int main(void) {

    halInit();
    chSysInit();
    // Переведем ноги в режим на вход
    palSetLineMode(PAL_LINE(GPIOA, 3), PAL_MODE_INPUT_ANALOG);
    palSetLineMode(PAL_LINE(GPIOC, 0), PAL_MODE_INPUT_ANALOG);
    palSetLineMode(PAL_LINE(GPIOC, 3), PAL_MODE_INPUT_ANALOG);
    // Запустим модуль АЦП
    adcStart(&ADCD1, NULL);
	// Начнем преобразование
    adcStartConversion(&ADCD1, &adcconf1, buf, 1);
    // А затем все сломаем к чертям. Здесь стоит обратить внимание, что передача остановится в любом случае и мы можем об этом даже не узнать, поэтому используем прерывание по ошибке
    adcStartConversion(&ADCD1, &adcconf1, buf, 1);
    // Так как на предыдущей строке передача остановилась, можем сразу запускать заново
    adcStartConversion(&ADCD1, &adcconf1, buf, 1);
    while (true) {
	    // Раз в секунду будем забирать данные из буфера. Если в процессе работы напряжение на любом канале упадет ниже 100 (настройки watchdog), АЦП остановится, данные в буфере перестанут обновляться, а мы об этом даже не узнаем (а вот если бы обработали ошибку в прерывании...)
        uint16_t ch1 = buf[0];
        uint16_t ch2 = buf[1];
        uint16_t ch3 = buf[2];
        chThdSleepMilliseconds(1000);

    }
}