#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

/**
 * @brief   Initialize serial port
 */
void debug_stream_init(void);

/**
* @brief    Print
*/
void dbgprintf( const char* format, ... );

void comm_dbgprintf_info(const char *format, ...);
void comm_dbgprintf_warning(const char *format, ...);
void comm_dbgprintf_error(const char *format, ...);

#endif
