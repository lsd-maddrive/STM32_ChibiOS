#include <ch.h>
#include <hal.h>

#include <chprintf.h>

/*
 * Setup:
 * halconf.h -  Enable HAL_USE_SERIAL
 *              To change the size of the buffer, change SERIAL_BUFFERS_SIZE
 * mcuconf.h -  Choose required serial STM32_SERIAL_USE_*
 */

/*
 * Serial configuration
 */
static const SerialConfig sdcfg = {
  .speed = 115200,          /* baudrate, directly number */
  .cr1 = 0,                 /* CR1 register, no need to set this */
  .cr2 = 0,                 /* CR2 register, no need to set this */
  /* RM says that USART_CR2_LINEN enables error detection,
     so this should work without this USART_CR2_LINEN */
  .cr3 = 0                  /* CR3 register, no need to set this */
  /*
   * if need to set one of registers use USART_CR1_*, USART_CR2_* or USART_CR3_*
   */
};

#define SERIAL_CHARACTER            0x00
#define SERIAL_BYTE_ARRAY           0x01
#define SERIAL_UINT32               0x02
#define SERIAL_STRING               0x03
#define SERIAL_FORMATTED_STRING     0x04
#define SERIAL_MATLAB               0x05

const uint8_t send_type = SERIAL_CHARACTER;

static THD_WORKING_AREA(waSender, 128);
static THD_FUNCTION(Sender, arg)
{
    arg = arg;      /* just to avoid warnings from compiler */
    while (true)
    {
        palToggleLine( LINE_LED1 );
        switch ( send_type )
        {
            case SERIAL_CHARACTER:
            {
                /* Send one byte / one character */
                sdPut( &SD6, 'a' );
                break;
            }
            case SERIAL_BYTE_ARRAY:
            {
                /* Send array of bytes */
                /*
                 * 0x30 == '0' (zero character)
                 * 0x41 == 'A'
                 * 0x42 == 'B'
                 * 0x0A == '\n' (newline)
                 */
                /* It okay to use codes and symbols as usually (char == uint8_t) */
                /* Last character is newline */
                const uint8_t array[] = {0x30, 0x41, 0x42, '\n'};
                /*
                 * Send 4 bytes (last arg) of array
                 * Result must be "0AB" as output, newline not shown
                 */
                sdWrite( &SD6, array, 4 );
                break;
            }
            case SERIAL_UINT32:
            {
                /*
                 * Okay, now send uint32_t as sequential of bytes (uint8_t)
                 */
                uint32_t bytes_comb = 0x0A304142;
                /*
                 * Result must be "0AB" - reason is sending order
                 * To send number as array we use cast:
                 * get variable address and use as pointer
                 * because any array == pointer to first element
                 * Last arg is 4 as uint32_t == uint8_t * 4
                 */
                sdWrite( &SD6, (uint8_t *)&bytes_comb, 4 );
                break;
            }
            case SERIAL_STRING:
            {
                /*
                 * Send string (string is an array) with sdWrite
                 */
                sdWrite( &SD6, (uint8_t *)"Hello\n", 6 );
                break;
            }
            case SERIAL_FORMATTED_STRING:
            {
                /*
                 * Send formatted string
                 * (BaseSequentialStream *) is used for chprintf function
                 * as function is abstract for any kind of sequential stream
                 * (it may be not only UART, but other serial interfaces
                 */
                const char *cool_string = "formatted string";
                chprintf( (BaseSequentialStream *)&SD6, "I can send %s =)\n", cool_string );
                break;
            }
            /* For this case there are MATLAB scripts in folder */
            case SERIAL_MATLAB:
            {
                /* Read one byte data with timeout 10 ms */
                msg_t msg = sdGetTimeout( &SD6, MS2ST( 10 ) );

                /*
                 * This condition work for any king of uint8_t message
                 * As msg_t == uint32_t and MSG_TIMEOUT < 0 and MSG_RESET < 0
                 *      then if something is wrong - msg == MSG_RESET
                 *      if timeout - msg == MSG_TIMEOUT
                 *      if we got message - msg >= 0
                 * Don`t forget, one byte is uint8_t but msg_t == uint32_t
                 *      you need to cast to get real number,
                 *      especially if you send negative numbers
                 * As -1 in int8_t equals 255 in uint32_t (msg_t)
                 * -1 == 0x000000ff     (uint32_t) - here left bit is not set - positive
                 * -1 == 0xff           (int8_t)   - here left bit is set - negative
                 */
                if ( msg >= 0 )
                {
                    /* Cast to real message */
                    int8_t byte = msg;

                    /* Gonna send 1 from MATLAB to send me number */
                    if ( byte == 1 )
                    {
                        /* Send number once */
                        uint16_t two_bytes_number = 10000;
                        sdWrite( &SD6, (uint8_t *)&two_bytes_number,
                                        sizeof( two_bytes_number ) );
                    } else if ( byte == 2 )
                    {
                        /* Send array of numbers */
                        uint32_t array_of_four_bytes_numbers[3] = {
                                400000, 100000, 20202020
                        };
                        /*
                         * sizeof() for array works only for static arrays,
                         * not passed through pointers
                         */
                        /*
                         * sizeof() - gives size of type in bytes
                         * for static arrays it works for whole array
                         */
                        sdWrite( &SD6, (uint8_t *)array_of_four_bytes_numbers,
                                        sizeof( array_of_four_bytes_numbers ) );
                    }
                }
                break;
            }
            default:
                palToggleLine( LINE_LED2 );
        }

        chThdSleepMilliseconds( 500 );
    }
}

int main(void)
{
    chSysInit();
    halInit();

    /* as 6th driver is used, use SD6 structure for driver functions */
    sdStart( &SD6, &sdcfg );
    /*
     * https://os.mbed.com/platforms/ST-Nucleo-F767ZI/
     * serial 6th driver is on PG_14, PG_9
     * alternate function is 8th (check datasheet)
     */
    palSetPadMode( GPIOG, 14, PAL_MODE_ALTERNATE(8) );  // TX = PG_14
    palSetPadMode( GPIOG, 9, PAL_MODE_ALTERNATE(8) );   // RX = PG_9

    /* create thread that sends just string */
    chThdCreateStatic( waSender, sizeof(waSender), NORMALPRIO, Sender, NULL );

    while (true)
    {
        chThdSleepSeconds( 1 );
    }
}
