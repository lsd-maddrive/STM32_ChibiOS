/*
        ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

                http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
*/

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

    /*
     * Normal main() thread activity, in this demo it does nothing.
     */
    while (true) 
    {
        msg_t msg = sdReadTimeout( test_dr, (void *)&input_params, sizeof(input_params), MS2ST( 500 ) );
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
