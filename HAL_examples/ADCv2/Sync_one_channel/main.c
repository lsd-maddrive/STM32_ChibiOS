#include "ch.h"
#include "hal.h"
#include <chprintf.h>

// Включаем UART через USB, чтобы выводить значения в терминал эклипса
static const SerialConfig sd_st_cfg = {
    .speed = 115200,
    .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static SerialDriver         *debug_serial = &SD3;
static BaseSequentialStream *debug_stream = NULL;

void debug_stream_init( void )
{
    sdStart( debug_serial, &sd_st_cfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );

    debug_stream = (BaseSequentialStream *)debug_serial;
}

void dbgprintf( const char* format, ... )
{
    if ( !debug_stream )
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

ADCConversionGroup adcconf1 = {
    // Непрерывный режим не нужен
    .circular = FALSE,
    // Канал всего один
    .num_channels = 1,
    // Callback функции не нужны
    .end_cb = NULL,
    .error_cb = NULL,
    // Здесь можно поменять разрешение (номинально 12 бит) и включить watchdog (сейчас выключен)
    .cr1 = 0,
    // Пусть Chibi сама разбирается, как запускать преобразование
    .cr2 = ADC_CR2_SWSTART,
    // Выбираем для третьего канала АЦП самый быстрый режим работы, 3 такта на одно преобразование
    .smpr1 = 0,
    .smpr2 = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_3),
    // Watchdog выключен, значит и пределы никого не интересуют
    .htr = 0,
    .ltr = 0,
    // Так как канал только один, значит указываем только в первом элементе последовательности желаемый канал
    .sqr1 = 0,
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3)

};

// Выделяем буфер под преобразование
static adcsample_t buf[1] = {0};

int main(void) {

    halInit();
    chSysInit();
    debug_stream_init();
    // Устанавливаем ногу в режим на вход и запускаем АЦП модуль
    palSetLineMode(PAL_LINE(GPIOA, 3), PAL_MODE_INPUT_ANALOG);
    adcStart(&ADCD1, NULL);

    while (true) {
        // Просим сделать одно преобразование в синхронном режиме и выплевываем его в терминал
        adcConvert(&ADCD1, &adcconf1, buf, 1);
        dbgprintf("%d\n\r", buf[0]);
        chThdSleepMilliseconds(1000);

    }
}