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
// Определяем буфер типа float на 10 элементов
    float buf[10] = {1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5};
    while (true) {
// Раз в секунду отправляем весь буфер по последовательному порту. Обратите внимание, тип буфера приводится к uint8_t *, а количество данных увеличивается в 4 раза, так как float это 4 байта.
        sdWrite(uart3, (uint8_t *)buf, 40);
        chThdSleepMilliseconds(1000);
    }
}
