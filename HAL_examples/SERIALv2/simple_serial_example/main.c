#include "ch.h"
#include "hal.h"

// Инициализируем структуру конфигурации, указав только частоту
static const SerialConfig uart_conf = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

// Кладем указатель на драйвер в переменную
SerialDriver *uart3 = &SD3;

int main(void) {
    halInit();
    chSysInit();
// запускаем драйвер в работу
    sdStart(uart3, &uart_conf);
// Переводим ноги в состояние Rx, Tx
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );	
// Определяем буфер типа uint8_t на 10 элементов
    uint8_t buf[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    while (true) {
// Раз в секунду отправляем весь буфер по последовательному порту
        sdWrite(uart3, buf, 10);
        chThdSleepMilliseconds(1000);
    }
}