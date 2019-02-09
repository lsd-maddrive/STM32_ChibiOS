#include <ch.h>
#include <hal.h>

#include <chprintf.h>

BaseSequentialStream *debug_stream = (BaseSequentialStream *)&SD3;

uint32_t database = 0;

event_source_t      ev_error;

#define EVENT_FLAGS_OVERFLOW    1 << 0
#define EVENT_FLAGS_ERROR       1 << 1
#define EVENT_FLAGS_TIMEOUT     1 << 2

event_source_t      ev_buffer;

static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) 
{
    arg = arg;

    while (true)
    {
        chThdSleepMilliseconds( 101 );

        if ( chVTGetSystemTimeX() % 100 == 25 )
            chEvtBroadcastFlags( &ev_error, EVENT_FLAGS_ERROR );

        if ( chVTGetSystemTimeX() % 100 == 50 )
            chEvtBroadcastFlags( &ev_error, EVENT_FLAGS_TIMEOUT );

        if ( chVTGetSystemTimeX() % 100 == 55 )
            chEvtBroadcastFlags( &ev_error, EVENT_FLAGS_OVERFLOW );

        database = chVTGetSystemTimeX();
        chEvtBroadcast( &ev_buffer );
    }
}


int main(void)
{
    /* RT Core initialization */
    chSysInit();
    /* HAL (Hardware Abstraction Layer) initialization */
    halInit();

    static const SerialConfig sd_st_cfg = {
      .speed = 9600,
      .cr1 = 0, .cr2 = 0, .cr3 = 0
    };
    /* ST-Link debug */
    sdStart( &SD3, &sd_st_cfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );

    /* Events initialization */
    chEvtObjectInit( &ev_error );
    chEvtObjectInit( &ev_buffer );
    /* There must be only one register per listener! */
    event_listener_t    ev_lnr_buffer;
    event_listener_t    ev_lnr_error;

#define EVNT_MSK_BUFFER  EVENT_MASK( 0 )
#define EVNT_MSK_ERROR   EVENT_MASK( 1 )
    chEvtRegisterMask( &ev_buffer, &ev_lnr_buffer, EVNT_MSK_BUFFER );
    chEvtRegisterMaskWithFlags( &ev_error, &ev_lnr_error, EVNT_MSK_ERROR, EVENT_FLAGS_OVERFLOW |
                                                                          EVENT_FLAGS_ERROR |
                                                                          EVENT_FLAGS_TIMEOUT );

    /* Create notifiers */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL /* arg is NULL */);

    chprintf( debug_stream, "%d: Processing started!\n", chVTGetSystemTimeX() );

    while (true)
    {
        eventmask_t evt = chEvtWaitAny( ALL_EVENTS );

        if ( evt & EVNT_MSK_BUFFER ) { 
            chprintf( debug_stream, "%d: Buffer event: %d!\n", chVTGetSystemTimeX(), database );
        }
        if ( evt & EVNT_MSK_ERROR ) { 
            chprintf( debug_stream, "%d: Error happened!\n", chVTGetSystemTimeX() );

            eventflags_t flags = chEvtGetAndClearFlags( &ev_lnr_error );
            if ( flags & ( EVENT_FLAGS_OVERFLOW | EVENT_FLAGS_ERROR ) )
                chprintf( debug_stream, "    Overflow/unknown error!\n" );
            if ( flags & EVENT_FLAGS_TIMEOUT )
                chprintf( debug_stream, "    Timeout!\n" );
        }
    }
}
