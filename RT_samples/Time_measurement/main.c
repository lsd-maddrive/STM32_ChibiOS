#include <ch.h>
#include <hal.h>

#include <chprintf.h>


/*** Serial configuration ***/

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


/***        Defines Zone      ***/
/*
 * Each define will unlock code for different tools:
 * Time Measurement Unit or Measurement with GPT, correspondingly
 * TM Unit works slower, because it's able to get statistics
 *  - TM Unit is recommended for diagnostic and debugging
 *  - Using GPT for time measurement is recommended in real control system
 */

#define TM_USE

//#define GPT_USE

/*********************************/

#ifdef TM_USE

#define SYSTEM_FREQUENCY   216000000UL
static time_measurement_t  processing_time;

#else


static void gpt_cb ( GPTDriver *GPT_Timer5 );
static GPTDriver    *GPT_Timer5     = &GPTD5;

int32_t gpt_ticks       = 0;
int32_t total_time      = 0;

static const GPTConfig gpt5cfg = {
  .frequency =  100000,
  .callback  =  gpt_cb,
  .cr2       =  0,
  .dier      =  0U
};

/* Must be no more that 65k */
#define TMR_TICKS_2_OVRFLOW   gpt5cfg.frequency/2

static void gpt_cb (GPTDriver *gptd)
{
    gptd = gptd;
    gpt_ticks += TMR_TICKS_2_OVRFLOW;
}

#define SYSTEM_FREQUENCY   gpt5cfg.frequency

#endif

double test_function( void )
{
    float var;
    for( uint32_t g = 0; g < 400000; g++)
    {
        var = chVTGetSystemTimeX();
        var *= 2;
        var *= chVTGetSystemTimeX();
        var /= chVTGetSystemTimeX();
        var = var - 5.2;
    }

    return var;
}

double result;


int main(void)
{
    chSysInit();
    halInit();

    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

#ifdef TM_USE
    chTMObjectInit( &processing_time );
#else
    gptStart(GPT_Timer5, &gpt5cfg);
#endif

    while ( true )
    {
#ifdef TM_USE
        chTMStartMeasurementX( &processing_time );
#else
        gpt_ticks = 0;
        gptStartContinuous(GPT_Timer5, TMR_TICKS_2_OVRFLOW);
#endif
        result = test_function();

#ifdef TM_USE
        chTMStopMeasurementX( &processing_time );

        chprintf( (BaseSequentialStream *)&SD7, "%d / Best time:(%d)\tWorst time:(%d)\n\r",
                          (int)result,
                          RTC2US( SYSTEM_FREQUENCY, processing_time.best ),
                          RTC2US( SYSTEM_FREQUENCY, processing_time.worst ) );
#else
        /***    time in GPT ticks   ***/
        total_time = gpt_ticks + gptGetCounterX(GPT_Timer5);

        chprintf( (BaseSequentialStream *)&SD7, "Time:(%d / %d) - %d\n\r",
                  (int)RTC2US( SYSTEM_FREQUENCY * 1.0, total_time ), total_time,
                  (int)result );
#endif

        chThdSleepMilliseconds(500);
        palToggleLine( LINE_LED1 );
    }
}
