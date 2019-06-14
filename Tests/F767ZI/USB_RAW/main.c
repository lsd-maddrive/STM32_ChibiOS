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

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"

/* 1024 bytes */
static const uint8_t txbuf[] =
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

static uint8_t rxbuf[1024 + 1];

#define CONTINUOUS_WRITE

/*
 * USB writer. This thread writes data to the USB at maximum rate.
 * Can be measured using:
 *   dd if=/dev/xxxx of=/dev/null bs=512 count=10000
 */
static THD_WORKING_AREA(waWriter, 128);
static THD_FUNCTION(Writer, arg) {

    (void)arg;
    chRegSetThreadName("writer");
    
    palSetLine(LINE_LED3);
    while (true) {
#ifndef CONTINUOUS_WRITE
        uint8_t   byte;
        uint32_t  return_bytes = usbReceive(&USBD1, USBD1_DATA_AVAILABLE_EP, &byte, 1);

        if ( return_bytes == 1 && byte == 'a' )
        {
            palSetLine(LINE_LED2);
            for ( int i = 0; i < 1024; i++ )
            {
                msg_t msg = usbTransmit(&USBD1, USBD1_DATA_REQUEST_EP, txbuf, sizeof (txbuf));
                if ( msg == MSG_RESET )
                    chThdSleepMilliseconds(500);
            }
            palClearLine(LINE_LED2);
        }
#else
        msg_t msg = usbTransmit(&USBD1, USBD1_DATA_REQUEST_EP, txbuf, sizeof (txbuf));
        if ( msg == MSG_RESET )
            chThdSleepMilliseconds(500);
#endif
    }
}

/*
 * USB reader. This thread reads data from the USB at maximum rate.
 * Can be measured using:
 *   dd if=bigfile of=/dev/xxx bs=512 count=10000
 */
static THD_WORKING_AREA(waReader, 128);
static THD_FUNCTION(Reader, arg) {

    (void)arg;
    chRegSetThreadName("reader");
    while (true) {
#ifdef CONTINUOUS_WRITE
        msg_t msg = usbReceive(&USBD1, USBD1_DATA_AVAILABLE_EP,
                                                     rxbuf, sizeof (rxbuf));
        if (msg == MSG_RESET)
#endif
            chThdSleepMilliseconds(500);
    }
}

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

    (void)arg;
    chRegSetThreadName("blinker");
    while (true) {
        systime_t time;

        time = USBD1.state == USB_ACTIVE ? 250 : 1000;
        palClearLine(LINE_LED1);
        chThdSleepMilliseconds(time);
        palSetLine(LINE_LED1);
        chThdSleepMilliseconds(time);
    }
}

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

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(&USBD1);
    chThdSleepMilliseconds(1500);
    usbStart(&USBD1, &usbcfg);
    usbConnectBus(&USBD1);

    /*
     * Starting threads.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
    chThdCreateStatic(waReader, sizeof(waReader), NORMALPRIO, Reader, NULL);

    thread_t *writer_tp = NULL; 

    while (true) 
    {
        if ( writer_tp == NULL 
#ifndef CONTINUOUS_WRITE
            && palReadLine(LINE_BUTTON)
#endif
            )
        {
            writer_tp = chThdCreateStatic(waWriter, sizeof(waWriter), NORMALPRIO, Writer, NULL);
        }

        chThdSleepMilliseconds(1000);
    }
}
