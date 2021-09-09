#include "ch.h"
#include "hal.h"

// callback функция, которая должна сработать по настроенному событию
void palcb_button(void* args){
    // Преобразование аргумента к требуемому типу, в данному случае к uint8_t
    uint8_t arg = *((uint8_t*) args);
    // Проверка, что передача аргумента работает
    if (arg == 5) palToggleLine(LINE_LED3);
}

int main(void) {
    halInit();
    chSysInit();
    // Задаем число, которое передадим в функцию-обработчик
    uint8_t arg = 5;
    // Настраиваем ногу с лампочкой на выход
    palSetLineMode(LINE_LED3, PAL_MODE_OUTPUT_PUSHPULL);
    // А ногу с кнопкой на вход
    palSetLineMode(GPIOC_BUTTON, PAL_MODE_INPUT_PULLDOWN);
    // Разрешаем прерывание, указываем callbcak функцию и передаем указатель на аргумент
    palEnableLineEvent(GPIOC_BUTTON, PAL_EVENT_MODE_RISING_EDGE);
    palSetLineCallback(GPIOC_BUTTON, palcb_button, &arg);
    while (1) {
        chThdSleepMilliseconds(1);
    }
}
