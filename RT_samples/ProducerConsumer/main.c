#include <ch.h>
#include <hal.h>

#include <chprintf.h>

thread_reference_t  button_ref = NULL;

/*===========================================================================*/
/* EXT driver related.                                                       */
/*===========================================================================*/

static void extcb(EXTDriver *extp, expchannel_t channel)
{
  extp = extp; channel = channel;

  chSysLockFromISR();
  chThdResumeI( &button_ref, MSG_OK );
  chSysUnlockFromISR();
}

static const EXTConfig extcfg =
{
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb}, //PC13 = Button
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};

/*===========================================================================*/
/* Application code                                                          */
/*===========================================================================*/

SEMAPHORE_DECL(plansForWorldDestruction, 0);

static THD_WORKING_AREA(waButtonProc, 128);// 128 - stack size
static THD_FUNCTION(ButtonProc, arg)
{
  arg = arg;

  while ( 1 )
  {
    chSysLock();
    chThdSuspendS( &button_ref );
    chSysUnlock();

    chThdSleepMilliseconds( 15 );

    if ( palReadLine( LINE_BUTTON ) )
    {
            chSemSignal( &plansForWorldDestruction );
    }
  }
}

void startTask( uint8_t led_num )
{
        switch( led_num )
    {
      case 0:
        palSetLine( LINE_LED1 );
        break;
      case 1:
        palSetLine( LINE_LED2 );
        break;
      case 2:
        palSetLine( LINE_LED3 );
        break;
    }
}

void stopTask( uint8_t led_num )
{
        switch( led_num )
    {
      case 0:
        palClearLine( LINE_LED1 );
        break;
      case 1:
        palClearLine( LINE_LED2 );
        break;
      case 2:
        palClearLine( LINE_LED3 );
        break;
    }
}

static THD_WORKING_AREA(waConsumer1, 128);// 128 - stack size
static THD_WORKING_AREA(waConsumer2, 128);// 128 - stack size
static THD_WORKING_AREA(waConsumer3, 128);// 128 - stack size
static THD_FUNCTION(Consumer, arg)
{
  while ( 1 )
  {
    chSemWait( &plansForWorldDestruction );
        startTask( arg );

        switch( (int)arg )
    {
      case 0:
        chThdSleepMilliseconds( 1200 );
        break;
      case 1:
        chThdSleepMilliseconds( 1400 );
        break;
      case 2:
        chThdSleepMilliseconds( 1600 );
        break;
    }

    stopTask( arg );


    chThdSleepMilliseconds( 500 );
  }

}

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

int main(void)
{
  chSysInit();
  halInit();

  sdStart( &SD6, &sdcfg );
  palSetPadMode( GPIOG, 14, PAL_MODE_ALTERNATE(8) );  // TX = PG_14
  palSetPadMode( GPIOG, 9, PAL_MODE_ALTERNATE(8) );   // RX = PG_9

  extStart( &EXTD1, &extcfg );
  chThdCreateStatic( waButtonProc, sizeof(waButtonProc), NORMALPRIO, ButtonProc, NULL );

    chThdCreateStatic( waConsumer1, sizeof(waConsumer1), NORMALPRIO, Consumer, 0 );
  chThdCreateStatic( waConsumer2, sizeof(waConsumer2), NORMALPRIO, Consumer, 1 );
  chThdCreateStatic( waConsumer3, sizeof(waConsumer3), NORMALPRIO, Consumer, 2 );


  while (true)
  {
    chThdSleepSeconds( 1 );
  }
}
