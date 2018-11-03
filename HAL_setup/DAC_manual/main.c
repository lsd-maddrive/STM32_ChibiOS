#include <ch.h>
#include <hal.h>
#include <chprintf.h>

/*
 * Setup:
 * halconf.h -  Enable HAL_USE_DAC
 * mcuconf.h -  Choose required DAC channel unit STM32_DAC_USE_DAC1_CH*
 * 				There are two channels for one module
 */

/*
 * sin() requires math library
 * To connect math library '-lm' was set in project.mk
 * '-lm' - link libm.so library
 * That says linker to link math library
 * #include is not linking library, just preprocessor directive to include header
 */
#include <math.h>

/* Configuration structure */
static const DACConfig dac_cfg = {
	/* Initial value of DAC out */
	.init         = 0,
	/*
	 * Mode of DAC:
	 * 		DAC_DHRM_12BIT_RIGHT - 12 bit with right alignment
	 * 		DAC_DHRM_12BIT_LEFT  - 12 bit with left alignment
	 * 		DAC_DHRM_8BIT_RIGHT	 - 8 bit no alignment (half of dacsample_t [uint16_t] type)
	 */
	.datamode     = DAC_DHRM_12BIT_RIGHT,
	/* Direct register set, future used for triggering DAC */
	.cr           = 0
};

static THD_WORKING_AREA(waThread, 128);
static THD_FUNCTION(Thread, arg) 
{
    arg = arg;

    /*
     * DAC has two channels
     * Datasheet p69, PA4 - DACout1, PA5 - DACout2
     * Pin configuration for 1st channel
     */
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);

    /* Start DAC driver with configuration */
    dacStart(&DACD1, &dac_cfg);

    uint32_t	tick_count = 0;

    while (true)
    {
    	/* ((1 << 11) - 1) = 2047 			- half - amplitude */
    	/* (sin( tick_count / 100.0 ) + 1) 	- range [0; 2] */
    	uint16_t value = ((1 << 11) - 1) * (sin( tick_count / 100.0 ) + 1);

    	/*
    	 * Write value to DAC channel
    	 * Arguments: 	<&DACD1> - pointer to DAC driver
    	 * 				<0> 	 - channel number (first)
    	 * 				<value>	 - output value (according to mode/size)
    	 */
    	dacPutChannelX( &DACD1, 0, value );

    	tick_count++;
        /* Just to signal about working thread each 25th tick (25 * 20ms = 500 ms) */
        if ( tick_count % 25 == 0 )
        	palToggleLine( LINE_LED1 );

        chThdSleepMilliseconds( 20 );
    }
}

int main(void)
{
    chSysInit();
    halInit();

    chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL /* arg is NULL */);

    while (true)
    {
        chThdSleepSeconds( 1 );
    }
}
