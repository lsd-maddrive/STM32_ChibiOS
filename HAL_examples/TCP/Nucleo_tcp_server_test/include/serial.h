#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

#include <hal.h>
#include <chprintf.h>

/**
 * @brief   Initialize serial port
 */
void debug_stream_init(void);

/**
* @brief    Print
*/
void dbgprintf( const char* format, ... );

#endif
