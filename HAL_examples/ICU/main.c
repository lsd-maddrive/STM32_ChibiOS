#include "ch.h"
#include "hal.h"
#include "stdlib.h"
// Для работы с терминалом
#include "uart_debug.h"

// Конфигурация ШИМ модуля для генерации сигнала
static PWMDriver* pwm1 = &PWMD1;

static PWMConfig pwm_conf = {
    .frequency = 50000,
    .period = 10000,
    .channels = {
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL}
    },
    .cr2 = 0,
    .dier = 0
};

// Настройка ICU

// Здесь храним измеренную длительность импулься
uint32_t duration = 0;

// Использовать будем третий таймер
static ICUDriver* icu3 = &ICUD3;


// Определяем функции обработчики событий
void cbWidth(ICUDriver *icup){
    // Просто чтобы убрать ворнинги о неиспользуемой переменной
    (void) icup;
    // Сразу после окончания импульса определяем его длительность
    duration = icuGetWidthX(icu3);
    palSetLine(LINE_LED1);
}

void cbPeriod(ICUDriver *icup){
    (void) icup;
    palClearLine(LINE_LED1);
}

void cbOverflow(ICUDriver *icup){
    (void) icup;
    // Мигаем красной лампочкой, если померить не удается
    palToggleLine(LINE_LED3);
}

//Определяем конфигурацию
static ICUConfig icu_conf = {
    // Частота 50 кГц. То есть один такт соответствует 20 мкс
    .frequency = 50000,
    // Измеряем длительность высокого состония сигнала
    .mode = ICU_INPUT_ACTIVE_HIGH,
    // Определяем какая функция в каком случае должна сработать
    .width_cb = cbWidth,
    .period_cb = cbPeriod,
    .overflow_cb = cbOverflow,
    // Указываем канал таймера где будем мерить
    .channel = ICU_CHANNEL_1
    // Регистры DIER и ARR игнорируем
};

int main(void) {
    // Запускаем ChibiOS
    halInit();
    chSysInit();

    // Запустим юарт и отправим первую строку для теста
    debug_stream_init();
    dbgprintf("Start\n\r");

    // Запустим модуль ШИМ и начнем генерацию на ноге E9
    palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));
    pwmStart(pwm1, &pwm_conf);
    pwmEnableChannel(pwm1, 0, PWM_PERCENTAGE_TO_WIDTH(pwm1, 2500));

    // Запустим модуль ICU и переведем ногу А6 в режим первого канала третьего таймера
    palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(2));
    icuStart(icu3, &icu_conf);

    // Запустим измерение и разрешим вызов callback функций
    icuStartCapture(icu3);
    icuEnableNotifications(icu3);

    while (true) {
        // Читаем из терминала число от 1 до 9
        char val = sdGetTimeout(&SD3, TIME_MS2I(500));
        // Отсеиваем перевод строки которые так же прилетает из терминала
        // Да, это отвратительно, но слишком лениво делать лучше
        sdGetTimeout(&SD3, TIME_IMMEDIATE);
        // Преобразуем символ в цифру. Если прилетела не цифра, будет 0
        uint8_t num = atoi(&val);
        // Обновляем коэффициент заполнения сигнала если не 0
        if(num != 0) pwmEnableChannel(pwm1, 0, PWM_PERCENTAGE_TO_WIDTH(pwm1, num*1000));
        // Выводим измеренное значение в терминал
        dbgprintf("%d\n\r", duration);
    }
}
