#include <hal.h>
#include <stdio.h>
#include <chprintf.h>

#include "serial.h"

/*============================================================================*/
/* LINE CONFIGURATION                                                         */
/*============================================================================*/
#define SerialTX PAL_LINE(GPIOD, 8)
#define SerialRX PAL_LINE(GPIOD, 9)

static SerialDriver *debug_serial = &SD3;
static BaseSequentialStream *debug_stream = NULL;

static const SerialConfig sd_st_cfg = {
    .speed = 115200,
    .cr1 = 0,
    .cr2 = 0,
    .cr3 = 0
};

/**
 * @brief   Initialize serial port
 */
void debug_stream_init(void)
{
    sdStart(debug_serial, &sd_st_cfg);
    palSetLineMode(SerialRX, PAL_MODE_ALTERNATE(7));
    palSetLineMode(SerialTX, PAL_MODE_ALTERNATE(7));

    debug_stream = (BaseSequentialStream *)debug_serial;
}

/**
 * @brief    Print
 */
void dbgprintf(const char *format, ...)
{
    if (!debug_stream)
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}



/* Sending a debug line with info. */
void comm_dbgprintf_info(const char *format, ...)
{
    if (!debug_stream)
        return;

    char buffer[64];
    sprintf(buffer, "INF: %s\r\n", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

/* Sending a debug line with warning. */
void comm_dbgprintf_warning(const char *format, ...)
{
    if (!debug_stream)
        return;

    char buffer[64];
    sprintf(buffer, "WRN: %s\r\n", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

/* Sending a debug line with error. */
void comm_dbgprintf_error(const char *format, ...)
{
    if (!debug_stream)
        return;

    char buffer[64];
    sprintf(buffer, "ERR: %s\r\n", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

