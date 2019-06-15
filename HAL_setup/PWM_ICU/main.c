#include <ch.h>
#include <hal.h>

#include <chprintf.h>

/*
 * Setup:
 * halconf.h -  Enable HAL_USE_PWM / HAL_USE_ICU
 * mcuconf.h -  Choose required PWM unit STM32_PWM_USE_TIM*
 * 				Choose required ICU unit STM32_ICU_USE_TIM*
 */

/*
 * Intro:
 * The example represents mainly PWM - ICU drivers configuration
 * Serial is configured with no comments
 * To start receiving ICU callbacks short-circuit pin PA0 (ICU in) and PB0 (PWM out)
 */

/*===========================================================================*/
/* PWM driver related.                                                       */
/*===========================================================================*/

/* Pointer to PWM driver 3, as we gonna use PWM on base timer 3 */
PWMDriver *pwmDriver      	= &PWMD3;
/* Configuration structure */
PWMConfig pwmConf = {
	/* Frequency of base timer */
    .frequency 		= 10000,
	/* Period of PWM measured in base time ticks */
	/* To obtain period of PWM: per_time = <.period> / <.frequency> [s] */
	/* Dont forget about size of timer counter register */
    .period    		= 10000,
	/* Callback invoked on PWM counter overflow reset */
	/* Here invoked every <.period> ticks */
    .callback  		= NULL,
	/* PWM channels configuration */
	/* Here initialized as array */
    .channels  		= {
    					/*
    					 * Each channels has two fields:
    					 * 	.mode     - disable / active high (direct) / active low (inversed) PWM
    					 * 				PWM_OUTPUT_DISABLED / PWM_OUTPUT_ACTIVE_HIGH / PWM_OUTPUT_ACTIVE_LOW
    					 * 	.callback - callback invoked on channel compare event
    					 */
						  {.mode = PWM_OUTPUT_DISABLED, 	.callback = NULL},
						  {.mode = PWM_OUTPUT_DISABLED, 	.callback = NULL},
						  {.mode = PWM_OUTPUT_ACTIVE_HIGH, 	.callback = NULL},
						  {.mode = PWM_OUTPUT_DISABLED, 	.callback = NULL}
					  },
	/* Timer direct registers */
    .cr2        	= 0,
    .dier       	= 0
};

/*===========================================================================*/
/* ICU driver related.                                                       */
/*===========================================================================*/

/* Callback of ICU module, here used for width callback */
/* Keep measured width in ICU base timer ticks in the global variable */
icucnt_t measured_width = 0;

static void icucb (ICUDriver *icup)
{
	/* Write new measured value */
	measured_width = icuGetWidthX(icup);
}

/* Pointer to ICU driver 5, as we gonna use ICU on base timer 5 */
ICUDriver	*icuDriver		= &ICUD5;
/* Configuration structure */
ICUConfig	icuConf = {
	/* Set mode of ICU */
	/*
	 * ICU_INPUT_ACTIVE_HIGH - trigger on rising edge as start event
	 * ICU_INPUT_ACTIVE_LOW  - trigger on falling edge as start event
	 */
	.mode         	= ICU_INPUT_ACTIVE_HIGH,
	/* Frequency of base timer */
	.frequency    	= 10000,
	/*
	 * Callback invoked on width measured
	 * Example: mode = HIGH, first trigger - rising edge, second trigger - falling edge
	 * 			<.width_cb> called on falling edge
	 */
	.width_cb     	= icucb,
	/* Callback invoked on period measured */
	.period_cb    	= NULL,
	/* Callback invoked on base timer overflow */
	.overflow_cb  	= NULL,
	/*
	 * Select channel to measure pulse on
	 * The issue of STM32 - ICU module captures full timer for only one pulse measurement
	 * <.channel> selects first (ICU_CHANNEL_1) or second (ICU_CHANNEL_2) channel as main input
	 * Only first two channels can be used
	 * Respectively, ICU_CHANNEL_1 - connect pulse to timer channel 1
	 */
	.channel      	= ICU_CHANNEL_1,
	/* Timer direct register */
	.dier         	= 0
};

/*===========================================================================*/
/* Serial driver related.                                                    */
/*===========================================================================*/

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = USART_CR2_LINEN,
  .cr3 = 0
};

/*===========================================================================*/
/* Application code.                                                         */
/*===========================================================================*/

/* Thread to write string with measured ticks in tty (terminal) */
static THD_WORKING_AREA(waThread, 256);
static THD_FUNCTION(Thread, arg) 
{
    arg = arg;

    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX

    while (true)
    {
    	chprintf( (BaseSequentialStream *)&SD7, "Measured: %d\n", measured_width );
        chThdSleepMilliseconds(100);
    }
}

int main(void)
{
    chSysInit();
    halInit();

    /* Setup PWM pin */
    palSetPadMode( GPIOB, 0, PAL_MODE_ALTERNATE(2) );	// PB_0 - PWM3/3, set for timer function
    /* Start PWM driver */
    pwmStart( pwmDriver, &pwmConf );

    /* Setup ICU pin */
    palSetPadMode( GPIOA, 0, PAL_MODE_ALTERNATE(1) );	// PA_0 - ICU2/1, set for timer function
    /* Start ICU driver */
    icuStart( icuDriver, &icuConf );
    /* Start capture */
    icuStartCapture( icuDriver );
    /* Enable notifications (callbacks) */
    icuEnableNotifications( icuDriver );

    chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL /* arg is NULL */);

    while (true)
    {
    	/* Change PWM duty cycle for 3rd channel each 3 seconds */
    	/* Third arg - tick count of PWM base timer for duty cycle */
        pwmEnableChannel( pwmDriver, 2, 9500 );
        chThdSleepSeconds( 3 );
        pwmEnableChannel( pwmDriver, 2, 5000 );
        chThdSleepSeconds( 3 );
        pwmEnableChannel( pwmDriver, 2, 2500 );
        chThdSleepSeconds( 3 );
        pwmEnableChannel( pwmDriver, 2, 1000 );
        chThdSleepSeconds( 3 );
    }
}
