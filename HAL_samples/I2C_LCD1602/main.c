#include <ch.h>
#include <hal.h>

#include <chprintf.h>
#include <stdint.h>
#include <stdlib.h>


/*****************************************/
/********** I2C Configuration ************/
/*****************************************/

#define SCL_LINE    PAL_LINE( GPIOB, 8 )
#define DATA_LINE   PAL_LINE( GPIOB, 9 )

static I2CDriver    *i2cDriver  = &I2CD1;

static const I2CConfig i2c1conf = {
         STM32_TIMINGR_PRESC(15U)  |
         STM32_TIMINGR_SCLDEL(9U)  | STM32_TIMINGR_SDADEL(9U) |
         STM32_TIMINGR_SCLH(21U)   | STM32_TIMINGR_SCLL(24U),
         0,                         // CR1
         0                          // CR2
};

/*****************************************/

uint8_t port_lcd    = 0;

#define e_set()     lcdWriteByte( port_lcd |= 0x04 )  // set E in 1
#define e_reset()   lcdWriteByte( port_lcd &= ~0x04 ) // set E in 0
#define rs_set()    lcdWriteByte( port_lcd |= 0x01 )  // set RS in 1
#define rs_reset()  lcdWriteByte( port_lcd &= ~0x01 ) // set RS in 0
#define set_led()   lcdWriteByte( port_lcd |= 0x08 )  // turn on backlight
#define set_write() lcdWriteByte( port_lcd &= ~0x02 ) // writing into lcd-memory


uint8_t buf[1] = {0};

/*
 * @brief   Send information to LCD via I2C (aka send command)
 */
void lcdWriteByte( uint8_t byte )
{

    buf[0] = byte;

    msg_t msg = i2cMasterTransmitTimeout( i2cDriver, (uint16_t)0x27, buf, 1, NULL, 0, 1000 );

    if( msg == MSG_OK ) // just to be sure that the address of LCD is correct
    {
        palSetLine( LINE_LED3 );
    }
}

/*
 * @brief   Send 4 significant bits
 */
void lcdSendHalfByte( uint8_t byte )
{
    byte <<= 4;         // send 4 high bits (only that 4 bits are important)
    e_set();
    chThdSleepMicroseconds( 50 );
    lcdWriteByte( port_lcd|byte );
    e_reset();
    chThdSleepMicroseconds( 50 );
}

/*
 * @brief   Send 8 significant bits (1 byte)
 */
void lcdSendByte( uint8_t byte, uint8_t mode )
{
    if( mode == 0 ) rs_reset();
    else rs_set();

    uint8_t hc = 0;
    hc = byte >> 4;
    lcdSendHalfByte( hc );
    lcdSendHalfByte( byte );
}

/*
 * @brief   Clear LCD
 */
void lcdClear(void)
{
    lcdSendByte( 0x01, 0 );
    chThdSleepMicroseconds( 1500 );
}

/*
 * @brief   Set the position of cursor
 * @note    Work with 2 rows only
 *          The initial position is ( 0, 0 )
 */
void lcdSetCursorPos( uint8_t x, uint8_t y )
{
    uint8_t command = (y == 0) ? 0x80 : 0xc0;
    command |= x;
    lcdSendByte( command, 0);
}

/*
 * @brief   Show 1 character on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendChar( uint8_t x, uint8_t y, char ch )
{
    lcdSetCursorPos( x, y );
    lcdSendByte( ch, 1 );
}

/*
 * @brief   Show string on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendString( uint8_t x, uint8_t y, char* str )
{
    uint8_t i = 0;
    lcdSetCursorPos( x, y );
    while( str[i] != 0 )
    {
        lcdSendByte( str[i], 1 );
        i++;
    }
}

/*
 * @brief   Show signed integer number on display
 * @note    The initial position of cursor is ( 0, 0 )
 */
void lcdSendNumber( uint8_t x, uint8_t y, int16_t num )
{
    char buff[16];                  // the width of LCD is 16 cells

    itoa( num, buff, 10 );          // 10 -> decimal radix
    lcdSendString( x, y, buff );
}

/*
 * @brief   Initialization of LCD-display
 */
void lcdInit( void )
{
    chThdSleepMilliseconds( 20 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMilliseconds( 4 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMicroseconds( 100 );
    lcdSendHalfByte( 0x03 );
    chThdSleepMilliseconds( 1 );
    lcdSendHalfByte( 0x02 );
    chThdSleepMilliseconds( 1 );

    lcdSendByte( 0x28, 0 );         // 4 bit mode (DL = 0), 2 lines (N = 1)
    chThdSleepMilliseconds( 1 );
    lcdSendByte( 0x0C, 0 );         // show image on LCD (D = 1)
    chThdSleepMilliseconds( 1 );
    lcdSendByte( 0x01, 0 );         //  clean the screen
    chThdSleepMilliseconds(2);
    lcdSendByte(0x06, 0);           // cursor will move to the right
    chThdSleepMilliseconds(1);

    set_led();                      // turn on LED
    set_write();
}

int main(void)
{
    chSysInit();
    halInit();

    palSetLineMode( SCL_LINE,  PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN  );
    palSetLineMode( DATA_LINE, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN );

    i2cStart(i2cDriver, &i2c1conf);

    lcdInit( );
    lcdClear( );

    int16_t number = -777;

    while (true)
    {
        lcdSendChar( 15, 1, '!' );
        lcdSendString( 0, 0, "Big brother" );

        lcdSendNumber( 12, 0, number );
        lcdSendString( 0, 1, "is watching you" );

        chThdSleepMilliseconds( 1000 );
    }
}
