#include "ch.h"
#include "hal.h"
// Подключаем библиотеку
#include "chprintf.h"

// Определяем объекты драйвера и потока глобальными переменными
static SerialDriver *debug_serial = &SD3;
static BaseSequentialStream *debug_stream = NULL;

// Конфигурируем работу UART
static const SerialConfig sd_st_cfg = {
    .speed = 115200,
    .cr1 = 0,
    .cr2 = 0,
    .cr3 = 0
};

// Функция инициализации
void debug_stream_init( void )
{
// Запускаем драйвер
    sdStart( debug_serial, &sd_st_cfg );
// Переводим ноги в состояние Rx, Tx
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );
// Переопределяем указатель на поток
    debug_stream = (BaseSequentialStream *)debug_serial;
}

// Функция отправки строки в терминал
void dbgprintf( const char* format, ... )
{
// Проверяем, что debug_stream_init() случился
    if ( !debug_stream )
    return;

// Отправляем в chvprintf() данные и ждем чуда
    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

int main(void) {
    halInit();
    chSysInit();
// Запускаем драйвер и инициализируем поток
    debug_stream_init();
    uint8_t i = 0;
    while (true) {
// Отправляем в терминал бессмысленное сообщение, с кучей подстановок. Обратите внимание на символы новой строки и возврата каретки.
        dbgprintf("Test %s #%u or %0.1f\n\r", "message", i, (float)i + 0.5f);
        chThdSleepMilliseconds(1000);
        i++;
    }
}
