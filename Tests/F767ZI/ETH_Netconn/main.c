#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"

#include "lwipthread.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include <string.h>

static uint8_t buffer[1024] =
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

SerialDriver            *debug_sd   = &SD2;
BaseSequentialStream    *debug_str  = (BaseSequentialStream *)&SD2;

#define printf(fmt, ...) chprintf(debug_str, fmt, ##__VA_ARGS__)

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0,
  .cr2 = 0,
  .cr3 = 0
};

err_t process_connection ( struct netconn *conn )
{
    struct netbuf   *buf;
    err_t recv_err;

    test_params_t   *input_params;
    uint16_t        params_sz;

    uint32_t        chunk_idx = 0;

    while ( (recv_err = netconn_recv( conn, &buf )) == ERR_OK )
    {
        // printf( "received new data\n" );

        // do
        // {

        netbuf_data( buf, (void **)&input_params, &params_sz );

        // printf( "data size: %d\n", params_sz );

        if ( params_sz != sizeof( test_params_t ) )
        {
            printf( "netbuf_data() failed\n" );
            return ERR_ARG;
        }

        if ( input_params->chunk_size == 0   || 
             input_params->chunk_size > 1024 ||
             input_params->chunk_count == 0 ||
             input_params->check_id != CONST_INPUT_PARAMS_CHECK_ID )
        {
            printf( "input_params check failed\n" );
        }

        printf( "Params: sz %d / cnt %d\n", input_params->chunk_size,
                                            input_params->chunk_count );

        palSetLine( LINE_LED1 );

        for ( uint32_t i = 0; i < input_params->chunk_count; i++ )
        {
            uint8_t flags = NETCONN_NOFLAG;

            // flags = i < input_params->chunk_count - 1 ? NETCONN_MORE : NETCONN_NOFLAG;

            // printf( "Send flags: 0x%x\n", flags );

            if ( (recv_err = netconn_write( conn, buffer, input_params->chunk_size, flags )) != ERR_OK )
            {
                printf( "netconn_write() err: %d\n", recv_err );
                break;
            }
        }

        palClearLine( LINE_LED1 );

        chunk_idx++;

        /* Not used here because we send 9 bytes request */
        // } while ( netbuf_next( buf ) >= 0 );

        // printf( "remove buffer\n" );

        netbuf_delete( buf );

        if ( recv_err != ERR_OK )
            return recv_err;
    }

    return ERR_OK;
}

/*
 * Application entry point.
 */
int main(void) 
{
    halInit();
    chSysInit();
    lwipInit(NULL);

    /* For Debug */
    sdStart( debug_sd, &sdcfg );
    palSetPadMode( GPIOD, 5, PAL_MODE_ALTERNATE(7) );
    palSetPadMode( GPIOD, 6, PAL_MODE_ALTERNATE(7) );

    struct netconn *conn;
    err_t err, accept_err;

    /* Create a new connection identifier. */
    conn = netconn_new( NETCONN_TCP );
    if ( conn == NULL )
    {
        printf( "netconn_new() failed\n" );
    }

    /* Bind connection to well known port number 80. */
    err = netconn_bind( conn, NULL, 80 );
    if ( err != ERR_OK )
    {
        printf( "netconn_bind() failed\n" );
    }

    netconn_listen( conn );
    
    printf( "Initialized\n" );

    while (true) 
    { 
        struct netconn *newconn;

        /* Grab new connection. */
        accept_err = netconn_accept( conn, &newconn );

        // printf( "New connection ------------\n" );

        /* Process the new connection. */
        if (accept_err != ERR_OK)
        {
            printf( "netconn_accept() failed\n" );
        }

        process_connection( newconn );

        /* Close connection and discard connection identifier. */
        netconn_close(newconn);
        netconn_delete(newconn);

        palToggleLine( LINE_LED2 );
    }
}
