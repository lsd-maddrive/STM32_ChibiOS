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

#include "chprintf.h"

#include "lwipthread.h"
#include "web/web.h"

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg)
{
    (void)arg;
    chRegSetThreadName( "blinker" );
    while ( true )
    {
        palToggleLine( LINE_LED1 );
        chThdSleepMilliseconds( 1000 );
    }
}


#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include <string.h>

static void tcpecho(void)
{
    struct netconn *conn, *newconn;
    err_t err, accept_err;
    struct netbuf* buf;
    void* data;
    u16_t len;
    err_t recv_err;

    /* Create a new connection identifier. */
    conn = netconn_new(NETCONN_TCP);
    if (conn != NULL)
    {
        /* Bind connection to well known port number 800. */
        err = netconn_bind(conn, NULL, 800);

        if (err == ERR_OK)
        {
            /* Tell connection to go into listening mode. */
            netconn_listen(conn);

            while (1)
            {
                /* Grab new connection. */
                accept_err = netconn_accept(conn, &newconn);

                /* Process the new connection. */
                if (accept_err == ERR_OK)
                {
                    while ((recv_err = netconn_recv(newconn, &buf)) == ERR_OK)
                    {
                        do
                        {
                            netbuf_data(buf, &data, &len);
                            netconn_write(newconn, data, len, NETCONN_COPY);

                        } while (netbuf_next(buf) >= 0);

                        netbuf_delete(buf);
                    }

                    /* Close connection and discard connection identifier. */
                    netconn_close(newconn);
                    netconn_delete(newconn);
                }
            }
        }
        else
        {
            netconn_delete(newconn);
        }
    }
    else
    {
        printf("can not create TCP netconn");
    }
}

/*
 * Application entry point.
 */
int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     * - lwIP subsystem initialization using the default configuration.
     */
    halInit();
    chSysInit();
    lwipInit(NULL);

    /*
     * Creates the blinker thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /*
     * Creates the HTTP thread (it changes priority internally).
     */
//    chThdCreateStatic(wa_http_server, sizeof(wa_http_server), NORMALPRIO + 1,
//                                        http_server, NULL);

    tcpecho();
    
    while (true)
    {
        
        chThdSleepSeconds( 1 );
    }
}
