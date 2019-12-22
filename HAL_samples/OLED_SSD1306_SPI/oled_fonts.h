#include <stdint.h>

#ifndef __OLED_FONTS_H__
#define __OLED_FONTS_H__

typedef struct {
	uint8_t width;    /*!< Font width in pixels */
	uint8_t height;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} DisplayFont_t;


extern DisplayFont_t Font_5x8rus;
extern DisplayFont_t Font_7x10;
extern DisplayFont_t Font_11x18;
extern DisplayFont_t Font_16x26;

#endif // __OLED_FONTS_H__
