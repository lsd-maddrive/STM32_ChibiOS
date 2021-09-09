#include "ch.h"
#include "hal.h"
#include <chprintf.h>
// Настройка вывода информации в терминал с помощью format string
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

// Положим указатель на драйвер в переменную
SPIDriver* spi1 = &SPID1;

// Структура конфигурации, circular режим выключен в halconf.h, CS задается как PAL_LINE
static SPIConfig conf = {
    // Не используем прерывания
    .end_cb = NULL,
    // CS вешаем на ногу А4
    .ssline = PAL_LINE(GPIOA, 4),
    // Значение предделителя 16
    .cr1 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
    // Длина слова 8 бит
    .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};

int main(void) {

    // Включаем Чиби
    halInit();
    chSysInit();

    // Включаем UART
    debug_stream_init();

    // Запускаем драйвер
    spiStart(spi1, &conf);

    // Настраиваем ноги. MISO, MOSI, SCLK в альтернативный режим согласно даташиту, CS просто на выход PUSHPULL
    palSetLineMode(PAL_LINE(GPIOA, 4), PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(PAL_LINE(GPIOA, 5), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOA, 6), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOA, 7), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

    // инициализируем буферы
    uint8_t txbuf[5] = {0x60, 0x0F, 0x00, 0x01, 0x30};
    uint8_t rxbuf[6] = {0};
    uint8_t data_reg[1] = {0b11101000};

    // Инициализируем датчик
    // Опустили ногу
    spiSelect(spi1);
    // Отправляем данные (сначала адрес регистра с автоинкрементированием, потом заполняем 4 ригистра конфигурации)
    spiSend(spi1, 5, txbuf);
    // Отпускаем ногу
    spiUnselect(spi1);

    while (true) {
        // С частотой 10 Герц читаем дачтик
        chThdSleepMilliseconds(100);
        // Опускаем ногу
        spiSelect(spi1);
        // Отправлем адрес регистра данных с автоинкреминтированием
        spiSend(spi1, 1, data_reg);
        // Читаем 6 байт
        spiReceive(spi1, 6, rxbuf);
        // Отпускаем ногу
        spiUnselect(spi1);
        // Обрабатываем информацию и отправляем на терминал
        uint8_t i;
        // Меняем тип буфера с 8 на 16 бит, чтобы склеить регистры
        int16_t* temp = (int16_t*)rxbuf;
        float buf[3] = {0, 0, 0};
        // Переводим данные с датчика в градусы в секунду
        for(i = 0; i < 3; i++){
            buf[i] = (float)temp[i] * 0.07f;
        }
        // Выводим полученные данные в терминал
        dbgprintf("x %.2f, y %.2f, z %.2f\r\n", buf[0], buf[1], buf[2]);
    }
}
