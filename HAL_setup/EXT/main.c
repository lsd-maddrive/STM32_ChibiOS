#include <ch.h>
#include <hal.h>

/*===========================================================================*/
/* PAL_EVENT (EXT) driver related.                                           */
/*===========================================================================*/

/* 
 * Callback function of the EXT
 * It is triggered on event that is configured in config structure
 */
static void extcb(void *arg __attribute__((unused))) 
{
  palToggleLine( LINE_LED1 );
}

/*===========================================================================*/
/* Application code.                                                         */
/*===========================================================================*/

int main(void)
{
  chSysInit();
  halInit();

  palSetPadCallback( GPIOD, 0, extcb, NULL );
  palEnablePadEvent( GPIOD, 0, PAL_EVENT_MODE_RISING_EDGE );

  palSetPadCallback( GPIOB, 3, extcb, NULL );
  palEnablePadEvent( GPIOB, 3, PAL_EVENT_MODE_BOTH_EDGES );

  while (true)
  {
    chThdSleepMilliseconds( 100 );
  }
}
