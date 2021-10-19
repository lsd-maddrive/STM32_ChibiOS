#include "ch.h"
#include "hal.h"

int main(void) {

    halInit();
    chSysInit();
    while (true)
    {
        palTogglePad(GPIOA, 1);
        chThdSleepMilliseconds(500);
    }
}
