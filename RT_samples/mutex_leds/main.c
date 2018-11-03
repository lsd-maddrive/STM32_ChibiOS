#include <ch.h>
#include <hal.h>

#include <chprintf.h>

#define DEADLOCK_ENABLE

MUTEX_DECL(led_mtx);

#ifdef DEADLOCK_ENABLE
MUTEX_DECL(dl_mtx);
#endif

static THD_WORKING_AREA(waBlinkerGreen, 128);
static THD_FUNCTION(BlinkerGreen, arg)
{
  arg = arg;                  //Just to avoid Warning

  while ( true )
  {
    chMtxLock(&led_mtx);

#ifdef DEADLOCK_ENABLE
    chThdSleepMilliseconds( 500 );
    chMtxLock(&dl_mtx);
#endif

    palClearLine( LINE_LED2 );
    for ( int i = 0; i < 4; i++ )
    {
      palToggleLine( LINE_LED1 );
      chThdSleepMilliseconds( 1000 );
    }

#ifdef DEADLOCK_ENABLE
    chMtxUnlock(&dl_mtx);
#endif

    chMtxUnlock(&led_mtx);
  }
}

static THD_WORKING_AREA(waFastBlinkerGreen, 128);
static THD_FUNCTION(FastBlinkerGreen, arg)
{
  arg = arg;                  //Just to avoid Warning

  while ( true )
  {
#ifdef DEADLOCK_ENABLE
    chMtxLock(&dl_mtx);
    chThdSleepMilliseconds( 500 );
#endif

    chMtxLock(&led_mtx);

    palSetLine( LINE_LED2 );
    for ( int i = 0; i < 10; i++ )
    {
      palToggleLine( LINE_LED1 );
      chThdSleepMilliseconds( 250 );
    }
    chMtxUnlock(&led_mtx);

#ifdef DEADLOCK_ENABLE
    chMtxUnlock(&dl_mtx);
#endif
  }
}

int main(void)
{
    chSysInit();
    halInit();

    chThdCreateStatic( waBlinkerGreen, sizeof(waBlinkerGreen), NORMALPRIO, BlinkerGreen, NULL );
    chThdCreateStatic( waFastBlinkerGreen, sizeof(waFastBlinkerGreen), NORMALPRIO, FastBlinkerGreen, NULL );

    while (true)
    {
      chThdSleepSeconds( 1 );
    }
}
