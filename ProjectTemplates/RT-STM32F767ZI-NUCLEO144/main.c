#include "ch.h"
#include "hal.h"
#include <chprintf.h>

static const SerialConfig sd_st_cfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static SerialDriver         *debug_serial = &SD3;
static BaseSequentialStream *debug_stream = NULL;

void debug_stream_init( void )
{
    sdStart( debug_serial, &sd_st_cfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );

    debug_stream = (BaseSequentialStream *)debug_serial;
}

void dbgprintf( const char* format, ... )
{
    if ( !debug_stream )
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

int main(void) {

    halInit();
    chSysInit();

    debug_stream_init();
    dbgprintf("Test\n\r");
    uint16_t i = 0;
    while (true) {
        chThdSleepMilliseconds(1000);
        dbgprintf("%d\n\r", i);
        palToggleLine(LINE_LED2);
        i++;
    }
}
