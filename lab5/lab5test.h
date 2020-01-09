#ifndef _LCOM_LAB5_TEST_H_
#define _LCOM_LAB5_TEST_H_

#include <lcom/lcf.h>

/* LAB5 TEST FUNCTIONS */
int test_video_init(uint16_t *mode, uint8_t *delay);

int test_video_rectangle(uint16_t mode, uint16_t x, uint16_t y,
		uint16_t width, uint16_t height, uint32_t color);

int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);

int test_video_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

int test_video_xpm(xpm_map_t* xpm, uint16_t x, uint16_t y);

int test_video_move(xpm_map_t* xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
		int16_t speed, uint8_t fr_rate);

#endif	// _LCOM_LAB5_TEST_H_
