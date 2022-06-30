#ifndef INCLUDES_UART_DEBUG_H_
#define INCLUDES_UART_DEBUG_H_

#include "ch.h"
#include "hal.h"
#include <chprintf.h>

void debug_stream_init( void );
void dbgprintf( const char* format, ... );

#endif
