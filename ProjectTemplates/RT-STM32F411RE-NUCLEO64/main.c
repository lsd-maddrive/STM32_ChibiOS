#include "ch.h"
#include "hal.h"

int main(void) {

    halInit();
    chSysInit();
    palSetLineMode(LINE_LED_GREEN, PAL_MODE_OUTPUT_OPENDRAIN);
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED_GREEN);
    }
}
