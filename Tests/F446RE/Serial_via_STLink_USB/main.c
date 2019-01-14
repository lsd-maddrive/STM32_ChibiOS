#include <ch.h>
#include <hal.h>

#include <chprintf.h>

SerialDriver    *test_dr    = &SD2;

static const SerialConfig sdcfg = {
  .speed = 9600,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

int main(void)
{
    /* RT Core initialization */
    chSysInit();
    /* HAL (Hardware Abstraction Layer) initialization */
    halInit();

    sdStart( test_dr, &sdcfg );
    palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

    while (true)
    {
        chprintf( (BaseSequentialStream *)test_dr, "Hello1 =)\n" );
        chThdSleepSeconds(1);
    }
}
