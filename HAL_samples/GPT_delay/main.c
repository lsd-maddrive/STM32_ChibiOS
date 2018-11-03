//#include <ch.h>
#include <hal.h>

GPTConfig gpt4conf = {
    .frequency    = 10000,
    .callback     = NULL,
    .cr2          = 0,
    .dier         = 0
};

PWMConfig pwm3conf = {
    .frequency = 1000000,
    .period    = 10000, /* 1/100 s = 10 ms */
    .callback  = NULL,
    .channels  = {
                  {PWM_OUTPUT_DISABLED, NULL},
                  {PWM_OUTPUT_DISABLED, NULL},
                  {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                  {PWM_OUTPUT_DISABLED, NULL}
                  },
    .cr2        = 0,
    .dier       = 0
};

// Delay in sec = tick/freq

int main(void)
{
//  chSysInit();
  halInit();

  GPTDriver *delayDriver    = &GPTD4;
  PWMDriver *pwmDriver      = &PWMD3;

  palSetLineMode( LINE_LED1, PAL_MODE_ALTERNATE(2) );

  pwmStart( pwmDriver, &pwm3conf );
  gptStart( delayDriver, &gpt4conf );

  while (true)
  {
//    palToggleLine( LINE_LED1 );
    pwmEnableChannel( pwmDriver, 2, 10000 );
    gptPolledDelay( delayDriver, 10000 );
    pwmEnableChannel( pwmDriver, 2, 5000 );
    gptPolledDelay( delayDriver, 10000 );
    pwmEnableChannel( pwmDriver, 2, 2500 );
    gptPolledDelay( delayDriver, 10000 );
    pwmEnableChannel( pwmDriver, 2, 1000 );
    gptPolledDelay( delayDriver, 10000 );
  }
}
