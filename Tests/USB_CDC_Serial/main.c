#include <stdio.h>
#include <string.h>

#define min(a, b) ( (a) < (b) ? (a) : (b) )

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "test.h"
#include "usbcfg.h"

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
SerialUSBDriver *test_dr    = &SDU1;

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

    sduObjectInit(test_dr);
    sduStart(test_dr, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    /* Required for USB setup */
    chThdSleepMilliseconds(1000);

    /*
     * Normal main() thread activity, in this demo it does nothing.
     */
    while (true) 
    {
        msg_t msg = chnReadTimeout( test_dr, (void *)&input_params, sizeof(input_params), MS2ST( serusbcfg.usbp->state == USB_ACTIVE ? 250 : 1000 ) );
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

            uint32_t buffer_mod_chunk = min(input_params.chunk_size, SERIAL_USB_BUFFERS_SIZE);

            for ( uint32_t i = 0; i < input_params.chunk_count; i++ )
            {
#if 1
              /* Writing in channel mode.*/
              chnWrite( test_dr, buf, input_params.chunk_size );
#else
              /* Writing in buffer mode.*/
              (void) obqGetEmptyBufferTimeout( &test_dr->obqueue, TIME_INFINITE );
              memcpy( test_dr->obqueue.ptr, buf, buffer_mod_chunk );
              obqPostFullBuffer( &test_dr->obqueue, buffer_mod_chunk );
#endif
            }

            palClearLine( LINE_LED1 );
        }

        palToggleLine( LINE_LED2 );
    }
}
