#include <ch.h>
#include <hal.h>

#include <chprintf.h>

static THD_WORKING_AREA(waThread, 128);
static THD_FUNCTION(Thread, arg) 
{
    arg = arg;

    while (true)
    {
        chThdSleepSeconds(1);
    }
}

int main(void)
{
    /* RT Core initialization */
    chSysInit();
    /* HAL (Hardware Abstraction Layer) initialization */
    halInit();

    chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL /* arg is NULL */);

    while (true)
    {
        chThdSleepSeconds(1);
    }
}
