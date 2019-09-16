#include <ch.h>
#include <hal.h>

#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

int main(void)
{
    /* RT Core initialization */
    chSysInit();
    /* HAL (Hardware Abstraction Layer) initialization */
    halInit();

    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

    systime_t time = chVTGetSystemTimeX();
    while (true)
    {
        time += MS2ST(1000);

        chprintf( (BaseSequentialStream *)&SD7,
                   "Time: %d\n\r", chVTGetSystemTimeX() );

        chThdSleepUntil( time );
    }
}
