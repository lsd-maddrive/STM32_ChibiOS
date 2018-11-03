#include <ch.h>
#include <hal.h>

static THD_WORKING_AREA(waBlinkerGreen, 128);
static THD_FUNCTION(BlinkerGreen, arg)
{
  (void *)arg;                  //Just to avoid Warning
  while ( true )
  {
    palToggleLine( LINE_LED1 );
    chThdSleepMilliseconds( 300 );
  }
}

static THD_WORKING_AREA(waBlinkerRed, 128);
static THD_FUNCTION(BlinkerRed, arg)
{
  (void *)arg;                  //Just to avoid Warning
  while ( true )
  {
    palToggleLine( LINE_LED3 );
#if 1
    chThdSleepMilliseconds( 500 );
#else
    gptPolledDelay( &GPTD4, 5000 );
#endif
  }
}

static THD_WORKING_AREA(waBlinkerBlue, 128);
static THD_FUNCTION(BlinkerBlue, arg)
{
  (void *)arg;                  //Just to avoid Warning
  while ( true )
  {
    palToggleLine( LINE_LED2 );
    chThdSleepMilliseconds( 700 );
  }
}

/******************************/
/* GPT Driver 				  */
/******************************/

GPTConfig gpt4conf = {
    .frequency    = 10000,
    .callback     = NULL,
    .cr2          = 0,
    .dier         = 0
};

int main(void)
{
    chSysInit();
    halInit();

    gptStart( &GPTD4, &gpt4conf );

    chThdCreateStatic( waBlinkerBlue, sizeof(waBlinkerBlue), NORMALPRIO, BlinkerBlue, NULL );
    chThdCreateStatic( waBlinkerGreen, sizeof(waBlinkerGreen), NORMALPRIO, BlinkerGreen, NULL );
    chThdCreateStatic( waBlinkerRed, sizeof(waBlinkerRed), NORMALPRIO, BlinkerRed, NULL );

    while (true)
    {
    	chThdSleepMilliseconds( 500 );
    }
}
