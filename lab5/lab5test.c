#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc_utils.h"
#include "utils.h"
#include "vg.h"
#include "vg_def.h"


/* TEST FUNCTIONS */
int
test_video_init(uint16_t *mode, uint8_t *delay)
{
	/* check pointers */
	if (!mode || !delay)
		return 1;

	if (vg_init(*mode) == NULL) {
		vg_exit();
		return 1;
	}

	/* wait for the specified delay (converts seconds to microseconds and then to ticks) */
	if (tickdelay(micros_to_ticks(SEC2MICRO(*delay)))) {  // nighty night
		vg_exit();
		return 1;
	}

	/* set text mode */
	if (vg_exit())
		return 1;

	return 0;
}


int
test_video_rectangle(uint16_t mode, uint16_t x, uint16_t y,
		uint16_t width, uint16_t height, uint32_t color)
{
	/* set video graphics mode */
	if (vg_init(mode) == NULL) {
		vg_exit();
		return 1;
	}

	/* draw the rectangle */
	vg_draw_rect(x, y, width, height, color);

	/* wait until the ESC is released by the user */
	if (wait_for_keypress(ESCBREAK)) { //TODO make packets (dual key)
		vg_exit();
		return 1;
	}

	/* set text mode */
	if (vg_exit())
		return 1;

	return 0;
}


int
vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step)
{
	/* draw pattern */
	uint32_t curr_color = first;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t rectangle_x = get_h_res() / no_rectangles;
	uint16_t rectangle_y = get_v_res() / no_rectangles;

	if (get_memory_model() == VBE_PACKED_PIXEL) {
		for (size_t i = 0; i < no_rectangles; ++i) {
			for (size_t j = 0; j < no_rectangles; ++j) {
				/* get new color */
				curr_color = (first + (i * no_rectangles + j) * step) % (1 << get_bitspixel());

				/* draw rectangle */
				vg_draw_rect(x, y, rectangle_x, rectangle_y, curr_color);
				x += rectangle_x;
			}

			/* new line of rectangles position */
			x = 0;
			y += rectangle_y;
		}
	}
	else if (get_memory_model() == VBE_DIRECT_COLOR) {
		uint8_t RedLSB = get_RedLSB();
		uint8_t GreenLSB = get_GreenLSB();
		uint8_t BlueLSB = get_BlueLSB();

		uint8_t r_first = r_color(first, RedLSB);  // get red field
		uint8_t g_first = g_color(first, RedLSB, GreenLSB);  // get green field
		uint8_t b_first = b_color(first, RedLSB, GreenLSB, BlueLSB);  // get blue field

		for (size_t i = 0; i < no_rectangles; ++i) {
			for (size_t j = 0; j < no_rectangles; ++j) {
				/* get new color */
				curr_color = (r_first + j * step) % (1 << get_RedScreenMask());  // add the new red field

				curr_color <<= (RedLSB - GreenLSB);  // shift enough bits to save the green field
				curr_color += (g_first + i * step) % (1 << get_GreenScreenMask());  // add the new green field

				curr_color <<= (GreenLSB - BlueLSB);  // shift enough bits to save the blue field
				curr_color += (b_first + (i + j) * step) % (1 << get_BlueScreenMask());  // add the new blue field
				curr_color <<= BlueLSB;  // make the bits start at the correct position

				/* draw rectangle */
				vg_draw_rect(x, y, rectangle_x, rectangle_y, curr_color);
				x += rectangle_x;
			}

			/* new line of rectangles position */
			x = 0;
			y += rectangle_y;
		}

	}
	else {
		printf("Can't draw in the current mode\n");
		return 1;
	}

	return 0;
}


int
test_video_pattern(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step)
{
	/* set video graphics mode */
	if (vg_init(mode) == NULL) {
		vg_exit();
		return 1;
	}

	/* draw pattern */
	if (vg_draw_pattern(no_rectangles, first, step)) {
		vg_exit();
		return 1;
	}

	/* wait until the ESC is released by the user */
	if (wait_for_keypress(ESCBREAK)) { //TODO make packets (dual key)
		vg_exit();
		return 1;
	}

	/* set text mode */
	if (vg_exit())
		return 1;

	return 0;
}


int
test_video_xpm(xpm_map_t* xpm, uint16_t x, uint16_t y)
{
	/* set video graphics mode to 0x105 */
	if (vg_init(0x105) == NULL) {
		vg_exit();
		return 1;
	}

	/* load sprite */
	xpm_image_t img;
	if (vg_load_sprite(xpm, &img))
		return 1;

	/* draw sprite */
	vg_draw_sprite(&img, x, y);

	/* wait until the ESC is released by the user */
	if (wait_for_keypress(ESCBREAK))  //TODO make packets (dual key)
		return 1;

	/* set text mode */
	if (vg_exit())
		return 1;

	return 0;
}


int
test_video_move(xpm_map_t* xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
		int16_t speed, uint8_t fr_rate)
{
	/* set video graphics mode to 0x105 */
	if (vg_init(0x105) == NULL) {
		vg_exit();
		return 1;
	}

	/* animate pixmap */
	/* wait until the ESC is released by the user */
	if (vg_animate_sprite(xpm, xi, xf, yi, yf, speed, fr_rate)) {
		vg_exit();
		return 1;
	}

	/* set text mode */
	if (vg_exit())
		return 1;

	return 0;
}
