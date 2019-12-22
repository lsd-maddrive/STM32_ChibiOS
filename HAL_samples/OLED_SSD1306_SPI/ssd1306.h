#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stddef.h>
#include <hal.h>
#include <ch.h>

#include "oled_fonts.h"

#define SSD1306_MIRROR_VERT     FALSE
#define SSD1306_MIRROR_HORIZ    FALSE
#define SSD1306_INVERSE_COLOR   FALSE

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

// Enumeration for screen colors
typedef enum {
    color_black = 0x00, // Black color, no pixel
    color_white = 0x01  // Pixel is set. Color depends on OLED
} ssd1306_color_t;

typedef struct {

    void (*reset)(void);
    void (*delay)(uint32_t ms);

    void (*send_data)(void *custom_data, void *buffer, size_t buffer_size);
    void (*send_cmd)(void *custom_data, uint8_t cmd);

    /* This field is passed to send_data/send_cmd funcs */
    void *custom_data;

} display_config_t;

// Procedure definitions
void ssd1306_Init(display_config_t *cfg);
void ssd1306_Fill(ssd1306_color_t color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, ssd1306_color_t color);
char ssd1306_WriteChar(char ch, DisplayFont_t Font, ssd1306_color_t color);
char ssd1306_WriteString(char* str, DisplayFont_t Font, ssd1306_color_t color);
void ssd1306_set_cursor_pos_px(uint8_t x, uint8_t y);

#endif // __SSD1306_H__
