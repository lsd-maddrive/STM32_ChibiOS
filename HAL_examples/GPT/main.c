#include "ch.h"
#include "hal.h"

// Будем использовать первый и третий таймеры
GPTDriver *timer1 = &GPTD1;
GPTDriver *timer3 = &GPTD3;

// Определяем две функции обработчика прерываний

// Первая функция просто моргает лампочкой
void cbgptfun1(GPTDriver *gptp)
{
    (void)gptp;
    palToggleLine(LINE_LED1);
}

// Вторая моргает другой лампочкой и каждый раз перезапускает таймер
void cbgptfun2(GPTDriver *gptp)
{
    (void)gptp;
    // Узнаем период первого таймера
    uint16_t T = gptGetIntervalX(timer1);
    gptStartOneShotI(timer3, T);
    palToggleLine(LINE_LED2);
}

// Настраиваем частоту первого таймера 50_000Гц (предделитель 4320, целое число, меньше чем 2^16) и указывает первую функцию как обработчик прерываний
GPTConfig gpt1_conf = {
    .frequency = 50000,
    .callback = cbgptfun1,
    .cr2 = 0,
    .dier = 0
};

// Для третьего таймера настройка такая же (для той же частоты нужен другой предделитель, так как другая шина, но сквозь API этого не видно), но укажем вторую функцию
GPTConfig gpt3_conf = {
    .frequency = 50000,
    .callback = cbgptfun2,
    .cr2 = 0,
    .dier = 0
};

int main(void) {

    halInit();
    chSysInit();
    // Включим оба таймера
    gptStart(timer1, &gpt1_conf);
    gptStart(timer3, &gpt3_conf);
    // Первый запустим в непрерывном режиме с частотой 2Гц
    gptStartContinuous(timer1, 25000);
    // Определим период первого таймера (потому что можем)
    uint16_t T = gptGetIntervalX(timer1);
    // Стартуем третий таймер с той же частотой
    gptStartOneShot(timer3, T);
    while (true) {
        // Наблюдаем, как две лампочки синхронно моргают
        chThdSleepMilliseconds(1000);
    }
}
