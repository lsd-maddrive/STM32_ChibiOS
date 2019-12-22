#include "ssd1306.h"

// Screenbuffer
static uint8_t framebuffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static bool inverted = false;

static uint16_t screen_x_pos_px;
static uint16_t screen_y_pos_px;

display_config_t    cfg_;

static void write_command(uint8_t cmd) {
    cfg_.send_cmd(cfg_.custom_data, cmd);
}

static void write_data(uint8_t *buffer, size_t buffer_sz) {
    cfg_.send_data(cfg_.custom_data, buffer, buffer_sz);
}

// Initialize the oled screen
void ssd1306_Init(display_config_t *cfg) {

    cfg_ = *cfg;

	// Reset OLED
	cfg_.reset();

    // Wait for the screen to boot
    cfg_.delay(100);

    // Init OLED
    write_command(0xAE); //display off

    write_command(0x20); //Set Memory Addressing Mode
    write_command(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#if SSD1306_MIRROR_VERT
    write_command(0xC0); // Mirror vertically
#else
    write_command(0xC8); //Set COM Output Scan Direction
#endif

    write_command(0x00); //---set low column address
    write_command(0x10); //---set high column address

    write_command(0x40); //--set start line address - CHECK

    write_command(0x81); //--set contrast control register - CHECK
    write_command(0xFF);

#if SSD1306_MIRROR_HORIZ
    write_command(0xA0); // Mirror horizontally
#else
    write_command(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#if SSD1306_INVERSE_COLOR
    write_command(0xA7); //--set inverse color
#else
    write_command(0xA6); //--set normal color
#endif

    write_command(0xA8); //--set multiplex ratio(1 to 64) - CHECK
    write_command(0x3F); //

    write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    write_command(0xD3); //-set display offset - CHECK
    write_command(0x00); //-not offset

    write_command(0xD5); //--set display clock divide ratio/oscillator frequency
    write_command(0xF0); //--set divide ratio

    write_command(0xD9); //--set pre-charge period
    write_command(0x22); //

    write_command(0xDA); //--set com pins hardware configuration - CHECK
    write_command(0x12);

    write_command(0xDB); //--set vcomh
    write_command(0x20); //0x20,0.77xVcc

    write_command(0x8D); //--set DC-DC enable
    write_command(0x14); //
    write_command(0xAF); //--turn on SSD1306 panel

    // Clear screen
    ssd1306_Fill(color_black);

    // Flush buffer to screen
    ssd1306_UpdateScreen();

    // Set default values for screen object
    screen_x_pos_px = 0;
    screen_y_pos_px = 0;
}

// Fill the whole screen with the given color
void ssd1306_Fill(ssd1306_color_t color) {
    /* Set memory */
    uint32_t i;

    for(i = 0; i < sizeof(framebuffer); i++) {
        framebuffer[i] = (color == color_black) ? 0x00 : 0xFF;
    }
}

// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        write_command(0xB0 + i);
        write_command(0x00);
        write_command(0x10);
        write_data(&framebuffer[SSD1306_WIDTH*i], SSD1306_WIDTH);
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void ssd1306_DrawPixel(uint8_t x, uint8_t y, ssd1306_color_t color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if( inverted ) {
        color = (ssd1306_color_t)!color;
    }

    // Draw in the right color
    if(color == color_white) {
        framebuffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        framebuffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => color_black or color_white
char ssd1306_WriteChar(char ch, DisplayFont_t font, ssd1306_color_t color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (screen_x_pos_px + font.width) ||
        SSD1306_HEIGHT < (screen_y_pos_px + font.height))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(screen_x_pos_px + j, (screen_y_pos_px + i), (ssd1306_color_t) color);
            } else {
                ssd1306_DrawPixel(screen_x_pos_px + j, (screen_y_pos_px + i), (ssd1306_color_t)!color);
            }
        }
    }

    // The current space is now taken
    screen_x_pos_px += font.width;

    // Return written char for validation
    return ch;
}

static int8_t utf8_2_rus_table(char *val) {
    int8_t result = -1;

    if ( val[0] == 0xD0 ) {
        // if (n == 0x81) { n = 0xA8; break; }

        if (val[1] >= 0x90 && val[1] <= 0xBF)
            result = val[1] + 0x30;
    } else if ( val[0] == 0xD1 ) {
        // if (n == 0x91) { n = 0xB8; break; }

        if (val[1] >= 0x80 && val[1] <= 0x8F)
            result = val[1] + 0x70;
    }

    return result - 192;    // Shift to get index of table
}

static char ssd1306_write_rus_char(char *ch, DisplayFont_t font, ssd1306_color_t color) {
    uint32_t i, b, j;

    int8_t rus_table_idx = utf8_2_rus_table(ch);

    // Check if character is valid (63 - amount of symbols)
    if (rus_table_idx < 0 || rus_table_idx > 63)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (screen_x_pos_px + font.width) ||
        SSD1306_HEIGHT < (screen_y_pos_px + font.height))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < font.height; i++) {
        b = font.data[(rus_table_idx) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(screen_x_pos_px + j, (screen_y_pos_px + i), (ssd1306_color_t) color);
            } else {
                ssd1306_DrawPixel(screen_x_pos_px + j, (screen_y_pos_px + i), (ssd1306_color_t)!color);
            }
        }
    }

    // The current space is now taken
    screen_x_pos_px += font.width;

    // Return written char for validation
    return *ch;
}

// Write full string to screenbuffer
char ssd1306_WriteString(char* str, DisplayFont_t font, ssd1306_color_t color) {
    // Write until null-byte
    while (*str) {
        if (*str > 126) {
            if (ssd1306_write_rus_char(str, font, color) != *str) {
                // Char could not be written
                return *(str+1);
            }
            // Skip twice as we have two bytes
            str++;
        }
        else {
            if (ssd1306_WriteChar(*str, font, color) != *str) {
                // Char could not be written
                return *str;
            }
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

// Position the cursor
void ssd1306_set_cursor_pos_px(uint8_t x, uint8_t y) {
    screen_x_pos_px = x;
    screen_y_pos_px = y;
}
