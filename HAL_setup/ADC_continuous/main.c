#include <ch.h>
#include <hal.h>

#include <chprintf.h>

/*
 * Setup:
 * halconf.h -  Enable HAL_USE_ADC
 * mcuconf.h -  Choose required ADC unit STM32_ADC_USE_ADC*
 *              set STM32_ADC_ADCPRE for ADC clock divider
 *              values are: ADC_CCR_ADCPRE_DIV8
 *                          ADC_CCR_ADCPRE_DIV6
 *                          ADC_CCR_ADCPRE_DIV4
 *                          ADC_CCR_ADCPRE_DIV2
 */

/*
 * There are some modes (explained later):
 *    one-shot  - the driver performs one group conversion (all channels)
 *    linear    - the driver performs N group conversions (N - depth)
 *    circular  - the driver performs continuously conversions (M channels * N depth)
 *                buffer is looped (after end reaches - start from beginning)
 */

/*
 * Common variables to set buffer parameters
 * ADC1_NUM_CHANNELS - amount of channels to read from, set by user
 * ADC1_BUF_DEPTH  - depth of buffer (how many times ADC is readed) set by user
 * Full buffer size is ADC1_NUM_CHANNELS * ADC1_BUF_DEPTH
 * Buffer is writed in sequential order
 * buffer -> [ch1, ch2, ch1, ch2, ch1, ...]
 */
#define ADC1_NUM_CHANNELS   2       // because only 2 sensors for now
#define ADC1_BUF_DEPTH      20

/*
 * adcsample_t - type of samples
 * From src:
 *    typedef uint16_t adcsample_t;
 */
static adcsample_t adc_buffer[ADC1_NUM_CHANNELS * ADC1_BUF_DEPTH];

/*
 * ADC streaming callback.
 */
/*
 * When ADC conversion ends, this func will be called
 * Next is only for circular mode:
 * If depth == 1 - callback is called once (when buffer is full-filled)
 * If depth != 1 - callback is called when buffer is half-filled (first part) and 
 *                                                   full-filled (second part)
 * <adcp>   - driver structure
 */

static void adccallback(ADCDriver *adcp __attribute__((unused)))
{
  if ( adcIsBufferComplete(adcp) )
  {
    /* second half of buffer is filled */
  } 
  else 
  {
    /* first half is filled */
  }
}

/* 
 * ADC needs no configuration - only conversion group setup 
 */
static const ADCConversionGroup adc1conf = {
  /*
   * Mode chose
   *   circular (true)  - filling of the buffer is looped (after end reaced - start from beginning)
   *   linear   (false) - buffer is fully filled once (callback not called on half filling in linear)
   */
  .circular     = true,
  /*
   * Set number of channels ( directly from macros =) )
   */
  .num_channels = ADC1_NUM_CHANNELS,
  /* 
   * Set callback function to call
   */
  .end_cb       = adccallback,
  /*
   * Callback for function if error occurs
   */
  .error_cb     = NULL,
  /*
   * Boooring registers =)
   * cr1 - here we can set resolution, but by default it is 12 bit and it is enough
   */
  .cr1          = 0,
  /*
   * cr2 - For simple continuous conversion it is not required
   */
  .cr2          = 0,
  /*
   * Next registers are really important!
   * smpr1 and smpr2 set sample rate
   * can be 3, 15, 28, 56, 84, 112, 144, 480
   * defined with macros ADC_SAMPLE_*
   * set amount of time for sampling 
   * Ex: 
   *   ADC_SAMPLE_112 means (112 * Tadc) = Time for one channel sampling
   *   Max ADC freq = 108 MHz (for this microcontroller)
   *   Divider (STM32_ADC_ADCPRE) is 4 now
   *   So fadc = 108 / 4 = 27 MHz
   *   Tadc = 1/fadc
   * You choose to set smpr1 or smpr2 by name of ADC_SMPR*_SMP_AN*() function
   * In our microcontroller: smpr1 - ADC 10-15
   *                         smpr2 - ADC 0-9
   * If you want to set sampling for 6th channel find function ADC_SMPR?_SMP_AN6()
   * and see if there is 1 or 2 in ADC_SMPR?_SMP_AN6() name
   * ADC_SMPR2_SMP_AN3() - set sampling for chennel 3 (AN3), write to register smpr2 (SMPR2)
   */
  .smpr1        = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_480),
  .smpr2        = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_480),
  /*
   * These are very valuable too
   * sqr1, sqr2, sqr3 - set sequence of channels
   * Function: ADC_SQR*_SQ*_N()
   * For example you want to set sequence: ch2, ch5, ch1
   * You should use function ADC_SQR?_SQ1_N(), ADC_SQR?_SQ2_N(), ADC_SQR?_SQ3_N()
   * Name: SQ1 - means sequence order 1, SQ2 - order 2, ...
   *       SQR2 - write to register sqr2
   * In out microcontroller: sequence (SQ*) 1-6   - sqr3
   *                         sequence (SQ*) 7-12  - sqr2
   *                         sequence (SQ*) 13-16 - sqr1
   *                         
   * Channels are refered through ADC_CHANNEL_IN* macros
   */
  .sqr1         = 0,
  .sqr2         = 0,
  /*
   * In example: sequence ch3, ch10
   * To gather some instructions for one register use logical OR ('|')
   */
  .sqr3         = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10)
};

int main(void)
{
    chSysInit();
    halInit();

    /* Enable ADC1, no config */
    adcStart(&ADCD1, NULL);
    /* Set pins for ADC channels 10 and 3 as analog input */
    palSetLineMode( LINE_ADC123_IN10, PAL_MODE_INPUT_ANALOG );  // PC0
    palSetLineMode( LINE_ADC123_IN3, PAL_MODE_INPUT_ANALOG );   // PA3
    /* 
     * Start conversion, args:
     *   pointer to driver, pointer to conversion group config, pointer to buffer, depth of buffer
     * During conversions samples are in buffer
     * Proto: 
     * void adcStartConversion (ADCDriver *adcp, const ADCConversionGroup *grpp, adcsample_t *samples, size_t depth)
     * In circular mode it start converting asynchronously
     */
    adcStartConversion(&ADCD1, &adc1conf, adc_buffer, ADC1_BUF_DEPTH);

    /* 
     * There are some useful functions (check them at docs):
     * - Synchronous convert:
     * http://chibios.sourceforge.net/docs3/hal/group___a_d_c.html#gae4f5c1e2d4283b40625a1d238bc49a7a
     * msg_t  adcConvert (ADCDriver *adcp, const ADCConversionGroup *grpp, adcsample_t *samples, size_t depth)
     * - Stop async conversion:
     * http://chibios.sourceforge.net/docs3/hal/group___a_d_c.html#gabce14d8f240e85715cded02c85b6585a
     * void   adcStopConversion (ADCDriver *adcp)
     */
    
    /*
     * Moreover, you can enable temperature and internal reference voltage sensors (only ADC1):
     * This function should be called in initialiation: adcSTM32EnableTSVREFE()
     * For config group:
     *   To set in sequence: ADC_CHANNEL_SENSOR, ADC_CHANNEL_VREFINT 
     *   To set sample rate: ADC_SMPR1_SMP_SENSOR(), ADC_SMPR1_SMP_VREF()
     */

    while (true)
    {
      chThdSleepSeconds( 1 );
    }
}
