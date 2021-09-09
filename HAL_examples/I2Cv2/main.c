#include "ch.h"
#include "hal.h"

// Задаем конфигурацию драйвера, частота тактирования i2c 54МГц
static const I2CConfig i2c1_conf = {
    .timingr = STM32_TIMINGR_PRESC(14U)  |
    STM32_TIMINGR_SCLDEL(3U)  | STM32_TIMINGR_SDADEL(2U) |
    STM32_TIMINGR_SCLH(14U)   | STM32_TIMINGR_SCLL(17U),
    .cr1 = 0,
    .cr2 = 0
};

// Запишем указатель на драйвер в переменную
static I2CDriver* i2c1 =  &I2CD1;

int main(void) {
    halInit();
    chSysInit();
    // Запустим дравйвер
    i2cStart(i2c1, &i2c1_conf);
    // Переведем ноги B8 и B9 в режим i2c1
    palSetLineMode(PAL_LINE(GPIOB, 8), PAL_MODE_ALTERNATE(4));
    palSetLineMode(PAL_LINE(GPIOB, 9), PAL_MODE_ALTERNATE(4));
    // Создадим массивы для записи и чтения данных
    uint8_t txbuf[1] = {0x0F};
    uint8_t rxbuf[1] = {0};
    // Прочитаем с устройства с адресом 1101010 значение из регистра по адресу 0x0F (регистр WHO_AM_I гироскопа l3gd20h)
    msg_t msg = i2cMasterTransmitTimeout(i2c1, 0b1101010, txbuf, 1, rxbuf, 1, 1000);
    // Если полученное значение соответсвует ожидаемому, зажжем первый светодиод
    if (rxbuf[0] == 0b11010111) palSetLine(LINE_LED1);
    // Проверим наличие ошибок
    i2cflags_t flag = i2cGetErrors(i2c1);
    // Если не пришел ACK бит, зажжем второй светодиод
    if (flag == I2C_ACK_FAILURE) palSetLine(LINE_LED2);
    while (true) {
        chThdSleepMilliseconds(1000);
    }
}
