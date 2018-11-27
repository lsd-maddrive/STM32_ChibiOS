#include <ch.h>
#include <hal.h>

/*===========================================================================*/
/* EXT driver related.                                                       */
/*===========================================================================*/

/* Callback function of the EXT
 * It is triggered on event that is configured in config structure
 * args:  <extp>    - pointer to the driver, now it has 
 *                    the only driver (EXTD1) and pointer will be like &EXTD1
 *        <channel> - channel where callback was triggered,
 *                    if trigger was from PD_0 - channel equals 0,
 *                    if from PB_3 - channel equals 3, like index in array
 */
static void extcb(EXTDriver *extp, expchannel_t channel) 
{
  /* The input arguments are not used now */
  /* Just to avoid Warning from compiler */
  extp = extp; channel = channel;
  palToggleLine( LINE_LED1 );
}

#define EXT_STATIC_INIT

#ifdef EXT_STATIC_INIT
/* 
 * Current config for inputs PD_0 (rising edge) and PB_3 (rising and falling edge)
 * https://os.mbed.com/platforms/ST-Nucleo-F767ZI/
 * There are EXT_MAX_CHANNELS (in our board it equals 24) channels in config 
 */
static const EXTConfig extcfg = {
  .channels = 
  {
    /* Setup for 0 channel (index), trigger on rising edge, autostart (not manual), port D
     * callback for trigger - 'extcb' function
     */
    [0]  = {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOD, extcb},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    /* Setup for 3rd channel (index), trigger on both edge, autostart (not manual), port B
     * callback for trigger - 'extcb' function
     */
    [3]  = {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, extcb},
    [4]  = {EXT_CH_MODE_DISABLED, NULL},
    [5]  = {EXT_CH_MODE_DISABLED, NULL},
    [6]  = {EXT_CH_MODE_DISABLED, NULL},
    [7]  = {EXT_CH_MODE_DISABLED, NULL},
    [8]  = {EXT_CH_MODE_DISABLED, NULL},
    [9]  = {EXT_CH_MODE_DISABLED, NULL},
    [10] = {EXT_CH_MODE_DISABLED, NULL},
    [11] = {EXT_CH_MODE_DISABLED, NULL},
    [12] = {EXT_CH_MODE_DISABLED, NULL},
    [13] = {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb}, //PC13 = Button
    [14] = {EXT_CH_MODE_DISABLED, NULL},
    [15] = {EXT_CH_MODE_DISABLED, NULL},
  }
};
#else   /* EXT_STATIC_INIT */
static EXTConfig extcfg;
#endif  /* EXT_STATIC_INIT */

/*===========================================================================*/
/* Application code.                                                         */
/*===========================================================================*/

int main(void)
{
  chSysInit();
  halInit();

#ifndef EXT_STATIC_INIT
  /* Another way to set config (in comparison with static initialization) */
  /* First set all channels disabled */
  for ( expchannel_t ch = 0; ch < EXT_MAX_CHANNELS; ch++ )
  {
    extcfg.channels[ch].mode  = EXT_CH_MODE_DISABLED;
    extcfg.channels[ch].cb    = NULL;
  }
#endif  /* EXT_STATIC_INIT */

  /* Start working EXT driver, current STM has only one driver */
  extStart( &EXTD1, &extcfg );

#ifndef EXT_STATIC_INIT
  /* Define channel config structure */
  EXTChannelConfig ch_conf;

  /* Fill in configuration for channel */
  ch_conf.mode 	= EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOD;
  ch_conf.cb	= extcb;

  /* Set channel (second arg) mode with filled configuration */
  extSetChannelMode( &EXTD1, 0, &ch_conf );

  /* Second channel */
  ch_conf.mode 	= EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB;
  ch_conf.cb	= extcb;
  extSetChannelMode( &EXTD1, 3, &ch_conf );
#endif

  while (true)
  {
    chThdSleepMilliseconds( 100 );
  }
}
