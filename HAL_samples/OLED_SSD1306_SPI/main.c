#include <ch.h>
#include <hal.h>

#include <stdio.h>

#include "ssd1306.h"

static THD_WORKING_AREA(waThread, 128);
static THD_FUNCTION(Thread, arg)
{
    (void)arg;

    while (true)
    {
        palToggleLine(LINE_LED1);
        chThdSleepSeconds(1);
    }
}

#define SSD1306_DC_Line     PAL_LINE(GPIOG, 2)
#define SSD1306_Rst_Line    PAL_LINE(GPIOD, 2)

void display_delay(uint32_t ms)
{
    chThdSleepMilliseconds(ms);
}

void display_reset(void)
{
	palClearLine(SSD1306_Rst_Line);
    display_delay(20);
    palSetLine(SSD1306_Rst_Line);
	display_delay(20);
}

void display_send_data(void *driver, void *buffer, size_t buffer_size)
{
    SPIDriver *dr = driver;

    spiSelect(dr);
    palSetLine(SSD1306_DC_Line);
    spiExchange(dr, buffer_size, buffer, buffer);
    spiUnselect(dr);
}

void display_send_cmd(void *driver, uint8_t cmd)
{
    SPIDriver *dr = driver;

    spiSelect(dr);
    palClearLine(SSD1306_DC_Line);
    spiExchange(dr, 1, &cmd, &cmd);
    spiUnselect(dr);
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
     */
    halInit();
    chSysInit();

    chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL /* arg is NULL */);

    static const SPIConfig spicfg = {
        .end_cb = NULL,
        .ssport = GPIOG,
        .sspad  = 3,
        .cr1    = SPI_CR1_CPOL | SPI_CR1_BR_0,
        .cr2    = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
    };

    palSetLineMode(PAL_LINE(GPIOC, 10),
                    PAL_MODE_ALTERNATE(6) |
                    PAL_STM32_OSPEED_HIGHEST);                      /* SPI SCK   */
    palSetLineMode(PAL_LINE(GPIOC, 12),
                    PAL_MODE_ALTERNATE(6) |
                    PAL_STM32_OSPEED_HIGHEST);                      /* MOSI      */
    palSetLineMode(PAL_LINE(GPIOG, 3), PAL_MODE_OUTPUT_PUSHPULL);   /* CS        */
    palSetLineMode(SSD1306_Rst_Line, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(SSD1306_DC_Line, PAL_MODE_OUTPUT_PUSHPULL);

    spiStart(&SPID3, &spicfg);

    display_config_t    disp_cfg = {
        .reset = display_reset,
        .delay = display_delay,
        .send_data = display_send_data,
        .send_cmd = display_send_cmd,
        .custom_data = &SPID3
    };

    ssd1306_Init(&disp_cfg);

    while (true)
    {
        ssd1306_set_cursor_pos_px(2, 0);
        ssd1306_WriteString("Hello new OLED", Font_7x10, color_white);

        char str_buffer[256];
        // char *value = "А";
        // sprintf(str_buffer, "Letter: %d | %d", value[0], value[1]);

        ssd1306_set_cursor_pos_px(2, 22);
        char code;
        if ( (code = ssd1306_WriteString("ДАВЛЕНИЕ", Font_5x8rus, color_white)) != 0 )
        {
            ssd1306_set_cursor_pos_px(2, 32);
            sprintf(str_buffer, "Failed code: %d", code);
            ssd1306_WriteString(str_buffer, Font_7x10, color_white);
        }

        ssd1306_UpdateScreen();

        chThdSleepSeconds(1);
    }
}
