#include "ch.h"
#include "hal.h"
#include "test.h"

static uint8_t buf[1024] =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

test_params_t   input_params;
SerialDriver    *test_dr    = &SD2;

static const SerialConfig sdcfg = {
  .speed = 1843200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

/*
 * Application entry point.
 */
int main(void) {

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    sdStart( test_dr, &sdcfg );
    palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

    while (true) 
    {
        msg_t msg = sdReadTimeout( test_dr, (void *)&input_params, sizeof(input_params), TIME_MS2I( 500 ) );
        if ( msg == sizeof(input_params) )
        {
            if ( input_params.chunk_size == 0   || 
                 input_params.chunk_size > 1024 ||
                 input_params.chunk_count == 0  ||
                 input_params.check_id != CONST_INPUT_PARAMS_CHECK_ID )
            {
                palSetLine( LINE_LED3 );
                chSysHalt("Input params invalid");
            }

            palSetLine( LINE_LED1 );

            for ( uint32_t i = 0; i < input_params.chunk_count; i++ )
            {
                sdWrite( test_dr, buf, input_params.chunk_size );
            }

            palClearLine( LINE_LED1 );
        }

        palToggleLine( LINE_LED2 );
    }
}
