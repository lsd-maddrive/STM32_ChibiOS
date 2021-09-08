#include "ch.h"
#include "hal.h"

// Определим количество каналов
#define NUM_OF_CHANNELS 3
// И глубину измерений
#define ADC_DEPTH 1502


void tim1cb(GPTDriver* gptp)
{
    (void)gptp;
    palToggleLine(PAL_LINE(GPIOB, 8));
}

// Настроим таймер (не забыв его включить в halconf.h и mcu.h)
static const GPTConfig tim1_conf = {
    // Зададим частоту
    .frequency = 27000000,
    // Прерывание не обязательно, но им все еще можно пользоваться
    .callback = tim1cb,
    // Скажем таймеру делать TRGO событие
    .cr2 = TIM_CR2_MMS_1,
    .dier = 0
};

// Настроим прерывание для АЦП
void endcbfun(ADCDriver* adcp)
{
    (void)adcp;
    // Прерывание сработает дважды за одно измерение, делать что-нибудь будем только по окончанию буфера
    if(adcIsBufferComplete(&ADCD1))
        palToggleLine(PAL_LINE(GPIOB, 9));
    // Здесь есть нюанс связанный с работой кэша. DMA может класть это в память, отведенную под кэш, в этом случае данные могут переписываться.
    // В этом примере этот вопрос опускается, имейте в виду потенциальное существование проблемы
}

ADCConversionGroup adcconf1 = {
    // Непрерывный режим работы
    .circular = TRUE,
    // Количество каналов через дефайн, чтобы не менять во всех местах при необходимости
    .num_channels = NUM_OF_CHANNELS,
    // Укажем функцию прерывания
    .end_cb = endcbfun,
    // Ошибку игнорируем, хотя возможно и не стоит
    .error_cb = NULL,
    // Без сторожевого пса
    .cr1 = 0,
    // Запуск преобразованию по TRGO первого таймера, так как именно его мы включили
    .cr2 = ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(0b1001),
    // Самое быстрое сэмплирование для всех каналов
    .smpr1 = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_3) | ADC_SMPR1_SMP_AN13(ADC_SAMPLE_3),
    .smpr2 = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_3),
    // Без системы watchdog эти два поля неактуальны
    .htr = 0,
    .ltr = 0,
    // Первыми тремя элементами укажем выбранные каналы
    .sqr1 = 0,
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN13)

};

// Определим буфер. Несмотря на то, что данные по сути дела являются матрицей, массив всё еще одномерный, с размером равным глубине преобразования умноженному на количество каналов
static adcsample_t buf[NUM_OF_CHANNELS * ADC_DEPTH] = {0};

// Буферы для разделения на каналы
uint16_t bufA[ADC_DEPTH] = {0};
uint16_t bufB[ADC_DEPTH] = {0};
uint16_t bufC[ADC_DEPTH] = {0};

int main(void) {

    halInit();
    chSysInit();
    // Запустим модуль таймера
    gptStart(&GPTD1, &tim1_conf);
    // И АПЦ
    adcStart(&ADCD1, NULL);
    // Поставим ноги на вход
    palSetLineMode(PAL_LINE(GPIOA, 3), PAL_MODE_INPUT_ANALOG);
    palSetLineMode(PAL_LINE(GPIOC, 0), PAL_MODE_INPUT_ANALOG);
    palSetLineMode(PAL_LINE(GPIOC, 3), PAL_MODE_INPUT_ANALOG);
    // Эти нужны были для отладки, в работе АЦП не участвуют
    palSetLineMode(PAL_LINE(GPIOB, 8), PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(PAL_LINE(GPIOB, 9), PAL_MODE_OUTPUT_PUSHPULL);
    // Запустим преобразование
    adcStartConversion(&ADCD1, &adcconf1, buf, ADC_DEPTH);
    // Стартанем таймер с частотой 27кГц. 27МГц частота тактирования и 1000 тактов до совпадения. К слову, если преобразование займёт больше времени, чем период таймера, ошибки никакой не случится (в смысле программной, так-то это не нормальное поведение). Судя по экспериментам преобразование начнётся мгновенно после окончания предыдущего, так что про синхронизацию можно будет забыть. Имейте это в виду.
    gptStartContinuous(&GPTD1, 1000);

    while (true) {
        // Так делать не стоит, так как потеряете данные полученные во время паузы. Это просто демонстрация как расположены данные в буфере
        uint16_t i;
        for(i = 0; i < ADC_DEPTH; i++) {
            bufA[i] = buf[i * 3];
            bufB[i] = buf[i * 3 + 1];
            bufC[i] = buf[i * 3 + 2];
        }
        chThdSleepMilliseconds(1000);
    }
}
