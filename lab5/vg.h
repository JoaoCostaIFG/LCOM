#ifndef _LCOM_VG_H_
#define _LCOM_VG_H_

#include <lcom/lcf.h>

/* VG BUFFER CLASS LIKE DEFINITIONS */
unsigned get_h_res();
unsigned get_v_res();
unsigned get_bitspixel();
unsigned get_bytespixel();
uint8_t get_RedScreenMask();
uint8_t get_RedLSB();
uint8_t get_GreenScreenMask();
uint8_t get_GreenLSB();
uint8_t get_BlueScreenMask();
uint8_t get_BlueLSB();
uint8_t get_memory_model();

/* INIT VG */
int vg_save_mode_info(uint16_t mode);

void* (vg_init)(uint16_t mode);

/* OTHER VG */
int vg_show_ctrl_info();

/* DRAW */
void vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

void vg_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int vg_load_sprite(xpm_map_t* xpm, xpm_image_t* img);

void vg_draw_sprite(const xpm_image_t *img, uint16_t x, uint16_t y);

int vg_animate_sprite(xpm_map_t* xpm, uint16_t xi, uint16_t xf, uint16_t yi, uint16_t yf, int16_t speed, uint8_t fr_rate);

/* DIVIDE COLOR INTO ITS R, G AND B PARTS */
inline uint8_t r_color(uint32_t color, uint8_t RedLSB) {  // get red field from color
	return color >> RedLSB;
}

inline uint8_t g_color(uint32_t color, uint8_t RedLSB, uint8_t GreenLSB) {  // get green field from color
	return (color << (31 - RedLSB)) >> (31 - RedLSB + GreenLSB);
}

inline uint8_t b_color(uint32_t color, uint8_t RedLSB, uint8_t GreenLSB, uint8_t BlueLSB) {  // get blue field from color
	return (color << (31 - GreenLSB)) >> (31 - GreenLSB + BlueLSB);
}

#endif	// _LCOM_VG_H_
