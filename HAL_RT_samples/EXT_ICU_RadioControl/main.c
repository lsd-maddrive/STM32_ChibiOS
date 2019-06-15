#include <ch.h>
#include <hal.h>

#include <chprintf.h>

/*===========================================================================*/
/* Serial driver related.                                                    */
/*===========================================================================*/


static const SerialConfig sdcfg = {
  .speed = 460800,
  .cr1 = 0,
  .cr2 = USART_CR2_LINEN,
  .cr3 = 0
};

/*===========================================================================*/
/* GPT driver related.                                                       */
/*===========================================================================*/

uint32_t overflow_cntr = 0;

void gpt4cb(GPTDriver *gptp __attribute__((unused)))
{
//  overflow_cntr++;
}

/*
 * GPT4 configuration. This timer is used as trigger for the ADC.
 */
static const GPTConfig gpt4cfg1 = {
  .frequency =  100000,
  .callback  =  NULL,
  .cr2       =  0, // TIM_CR2_MMS_1,  /* MMS = 010 = TRGO on Update Event.        */
  .dier      =  0U
};

/*===========================================================================*/
/* Mailbox code.                                                             */
/*===========================================================================*/
#define MAILBOX_SIZE 50
static mailbox_t steer_mb;
static msg_t b_steer[MAILBOX_SIZE];

static mailbox_t speed_mb;
static msg_t b_speed[MAILBOX_SIZE];


static mailbox_t test_mb;
static msg_t buffer_test_mb[MAILBOX_SIZE];

/*===========================================================================*/
/* PAL_EVENT (EXT) driver related.                                           */
/*===========================================================================*/

static void extcb1( void *arg __attribute__((unused)) ) 
{
  static gptcnt_t front_time, edge_time;

  if ( palReadPad( GPIOC, 0 ) == PAL_HIGH )
  {
    front_time = gptGetCounterX( &GPTD4 );
  } else {
    edge_time  = gptGetCounterX( &GPTD4 );

    gptcnt_t result = edge_time < front_time ? edge_time + (UINT16_MAX - front_time) :
                                               edge_time - front_time;

    chSysLockFromISR();
    chMBPostI(&test_mb, (msg_t)result);
    chSysUnlockFromISR();
  }

}

/*===========================================================================*/
/* ICU driver related.                                                       */
/*===========================================================================*/

static void icuwidthcb_steer(ICUDriver *icup)
{
  icucnt_t last_width = icuGetWidthX(icup);

  chSysLockFromISR();
  chMBPostI(&steer_mb, (msg_t)last_width);
  chSysUnlockFromISR();
}

static void icuwidthcb_speed(ICUDriver *icup)
{
  icucnt_t last_width = icuGetWidthX(icup);

  chSysLockFromISR();
  chMBPostI(&speed_mb, (msg_t)last_width);
  chSysUnlockFromISR();
}

static const ICUConfig icucfg_steer = {
  .mode         = ICU_INPUT_ACTIVE_HIGH,
  .frequency    = 100000,
  .width_cb     = icuwidthcb_steer,
  .period_cb    = NULL,
  .overflow_cb  = NULL,
  .channel      = ICU_CHANNEL_1,
  .dier         = 0
};

static const ICUConfig icucfg_speed = {
  .mode         = ICU_INPUT_ACTIVE_HIGH,
  .frequency    = 100000,
  .width_cb     = icuwidthcb_speed,
  .period_cb    = NULL,
  .overflow_cb  = NULL,
  .channel      = ICU_CHANNEL_1,
  .dier         = 0
};

/*===========================================================================*/
/* Application code.                                                         */
/*===========================================================================*/

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED attached to TP1.
 */
static THD_WORKING_AREA(waBlinker, 128);
static THD_FUNCTION(Blinker, arg __attribute__((unused)))
{
  while (true)
  {
    palToggleLine(LINE_LED1);
    chThdSleepSeconds(1);
  }
}

int main(void)
{
    chSysInit();
    halInit();

    // Serial driver
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX

    // Mailbox init
    chMBObjectInit(&steer_mb, b_steer, MAILBOX_SIZE);
    chMBObjectInit(&speed_mb, b_speed, MAILBOX_SIZE);

    chMBObjectInit(&test_mb, buffer_test_mb, MAILBOX_SIZE);       // For external interrupt

    // ICU driver
    icuStart(&ICUD9, &icucfg_steer);
    palSetPadMode( GPIOE, 5, PAL_MODE_ALTERNATE(3) );
    icuStartCapture(&ICUD9);
    icuEnableNotifications(&ICUD9);

    icuStart(&ICUD8, &icucfg_speed);
    palSetPadMode( GPIOC, 6, PAL_MODE_ALTERNATE(3) );
    icuStartCapture(&ICUD8);
    icuEnableNotifications(&ICUD8);

    // GPT driver
    gptStart(&GPTD4, &gpt4cfg1);
    gptStartContinuous( &GPTD4, UINT16_MAX );    // For external interrupt

    // PAL_EVENT (EXT) driver
    palSetPadCallback( GPIOC, 0, extcb1, NULL );
    palEnablePadEvent( GPIOC, 0, PAL_EVENT_MODE_BOTH_EDGES );
    
    chThdCreateStatic(waBlinker, sizeof(waBlinker), NORMALPRIO, Blinker, NULL);

    msg_t msg;

    while (true)
    {
      if ( chMBFetchTimeout(&steer_mb, &msg, TIME_IMMEDIATE) == MSG_OK )
        chprintf(((BaseSequentialStream *)&SD7), "Steer      : %d\n", msg);

      if ( chMBFetchTimeout(&speed_mb, &msg, TIME_IMMEDIATE) == MSG_OK )
        chprintf(((BaseSequentialStream *)&SD7), "Speed      : %d\n", msg);

      if ( chMBFetchTimeout(&test_mb, &msg, TIME_IMMEDIATE) == MSG_OK )
        chprintf(((BaseSequentialStream *)&SD7), "Steer (GPT): %d\n", msg);

      chThdSleepMilliseconds( 1 );
    }
}
