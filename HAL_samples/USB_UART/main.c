#include <ch.h>
#include <hal.h>

#include "chprintf.h"
#include "usbcfg.h"

static THD_WORKING_AREA(waBlinkerGreen, 128);
static THD_FUNCTION(BlinkerGreen, arg __attribute__((unused)))
{
  while ( true )
  {
    palToggleLine( LINE_LED1 );
    chThdSleepMilliseconds( 300 );
  }
}

static THD_WORKING_AREA(waBlinkerRed, 128);
static THD_FUNCTION(BlinkerRed, arg __attribute__((unused)))
{
  while ( true )
  {
    palToggleLine( LINE_LED3 );
    chThdSleepMilliseconds( 500 );
  }
}

static THD_WORKING_AREA(waBlinkerBlue, 128);
static THD_FUNCTION(BlinkerBlue, arg __attribute__((unused)))
{
  while ( true )
  {
    palToggleLine( LINE_LED2 );
    chThdSleepMilliseconds( 700 );
  }
}

static THD_WORKING_AREA(waSender, 128);
static THD_FUNCTION(Sender, arg __attribute__((unused)))
{
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1000);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  while ( true )
  {
    if ( SDU1.config->usbp->state == USB_ACTIVE )
      chprintf( (BaseSequentialStream *)&SDU1, "Hello!" );

    chThdSleepSeconds( 1 );
  }
}

int main(void)
{
  chSysInit();
  halInit();

  chThdCreateStatic( waBlinkerBlue, sizeof(waBlinkerBlue), NORMALPRIO, BlinkerBlue, NULL );
  chThdCreateStatic( waBlinkerGreen, sizeof(waBlinkerGreen), NORMALPRIO, BlinkerGreen, NULL );
  chThdCreateStatic( waBlinkerRed, sizeof(waBlinkerRed), NORMALPRIO, BlinkerRed, NULL );
  chThdCreateStatic( waSender, sizeof(waSender), NORMALPRIO+1, Sender, NULL );

  while (true)
  {
    chThdSleepMilliseconds( 500 );
  }
}
