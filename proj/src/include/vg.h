/** @file vg.h */
#ifndef __VG_H__
#define __VG_H__

#include <stdbool.h>
#include <stdint.h>

/** @addtogroup	vg_grp
 * @{
 */

/** @brief	Struct that saves the information of a sprite. */
typedef struct
{
  uint32_t Width;  /**< @brief Horizontal length of the sprite, in pixels. */
  uint32_t Height; /**< @brief Vertical length of the sprite, in pixels. */
  /**
   * @brief	Color value for every pixel, in sequence,
   *		from left to right, top to bottom of the image.
   * @details	A pixel can be represented by 1 or more bytes, sequentially,
   *depending on the graphics mode at which the read image file was encoded.
   */
  uint8_t* Data;
} Sprite_t;

/* VG GETTERS */
/** @brief	Returns the x resolution of the current graphics mode. */
unsigned get_h_res(void);
/** @brief	Returns the y resolution of the current graphics mode. */
unsigned get_v_res(void);
/** @brief	Returns the number of bits per pixel of the current graphics
 * mode. */
unsigned get_bitspixel(void);
/** @brief	Returns the number of bytes per pixel of the current graphics
 * mode. */
unsigned get_bytespixel(void);
/** @brief	Returns a byte that identifies the current memory model. */
uint8_t get_memory_model(void);
/* END VG GETTERS */

/* VG SETTERS */
/** @brief	Clears the buffer that's currently being used for drawing. */
void vg_clear(void);

/** @brief	Clears all buffers currently alloced by the program. */
void vg_clear_all(void);

/**@brief	Set true color mode for the currently set video graphics mode.
 * @return	0, on success\n
 *		1, otherwise.
 */
int set_truecolor(void);

/**@brief	Set default pallete width (6 bit) for the current set (super)VGA
 *mode.
 * @return	0, on success\n
 *		1, otherwise.
 */
int set_defaultdac(void);

/**@brief	Read and set a new color palette from a given file path.
 *
 * @param filename	Path the file to read.
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int set_color_palette_file(const char* const filename);

/**
 * @brief Set a new random color palette.
 *
 * @param palette_size    Number of colors to generate.
 * @param first_color_ind First color index.
 *
 * @return  0, on success\n
 *          1, otherwise.
 */
int set_random_color_palette(uint8_t palette_size, uint8_t first_color_ind);
/* END VG SETTERS */

/* OTHER PUBLIC FUNCTIONS */
/** @brief	Switchs to next frame buffer (in case there is more than one).
 */
void next_buff(void);
/**@brief	Sets the super VGA graphics mode to a given mode.
 * @note	Makes use of double buffering. This means the used vram is,
 *		*at least* double the amount specified in the set mode.
 *
 * @param mode    Hexadecimal integer that identifies the super VGA graphics
 *mode to set.
 * @param v_sync  Should we use vsync on the video mode.
 *
 * @return	Pointer to allocated video memory, on success\n
 *		      NULL, otherwise.
 */
void* vginit(uint16_t mode, bool v_sync);

/**@brief	Shows the VGA controller information.
 * @return	0, on success\n
 *		1, otherwise.
 */
int vg_show_ctrl_info(void);

/* DRAWING SIMPLE OBJECTS FUNCTIONS */
/**@brief	Draw a horizontal line on the current video memory buffer.
 * @note	All distances are measured in pixels.
 *
 * @param x	x coordinate to start the line at.
 * @param y	y coordinate to start the line at.
 * @param color	Color of the line.
 * @param len	Length of the line, measured horizontaly.
 */
void draw_line(uint16_t x, uint16_t y, uint32_t color, uint16_t len);

/**@brief	Draw a rectangle on the current video memory buffer.
 * @note	All distances are measured in pixels.
 *
 * @param x		x coordinate of the top-left rectangle corner.
 * @param y		y coordinate of the top-left rectangle corner.
 * @param width		Width of the rectangle (measured horizontaly).
 * @param height	Height of the rectangle (measured horizontaly).
 * @param color		Color of the rectangle.
 */
void draw_rect(uint16_t x,
               uint16_t y,
               uint16_t width,
               uint16_t height,
               uint32_t color);

/* SPRITE DRAWING */
/**	@addtogroup	sprite_grp
 *
 *	@brief	Code related to the reading and handling of image/sprite
 *			information from files, and the parsing, saving and use
 *of it.
 *
 *	@{
 */

/**@brief	Copies a given sprite to a buffer at the specified coordinates.
 * @note	Uses double buffering.
 *
 * @param sprite	Pointer to sprite to copy the information from.
 * @param x		Number of pixels from the left margin at which the copy
 * will be placed.
 * @param y		Number of pixels from the top margin at which the copy
 * will be placed.
 */
void draw_sprite_notransp(Sprite_t* sprite, const uint16_t x, const uint16_t y);

/**@brief	Copies a given sprite to a buffer at the specified coordinates
 * (skipping transparent pixels).
 * @note	Uses double buffering.
 *
 * @param sprite	Sprite to copy the information from.
 * @param x		Number of pixels from the left margin at which the copy
 * will be placed.
 * @param y		Number of pixels from the top margin at which the copy
 * will be placed.
 * @param transp	Color to skip (considered transparent).
 */
void draw_sprite(Sprite_t* sprite,
                 const uint16_t x,
                 const uint16_t y,
                 const uint32_t transp);

/**@brief	Copies a given sprite to a buffer at the specified coordinates
 *		(coloring transparent pixels to a given color (usually the
 *background color)).
 * @note	Uses double buffering.
 *
 * @param sprite	Sprite to copy the information from.
 * @param x		Number of pixels from the left margin at which the copy
 *will be placed.
 * @param y		Number of pixels from the top margin at which the copy
 *will be placed.
 * @param transp	Color to change (considered transparent).
 * @param bkg		Color to change to (usually the background color).
 */
void draw_sprite_ovr(Sprite_t* sprite,
                     const uint16_t x,
                     const uint16_t y,
                     const uint32_t transp,
                     const uint32_t bkg);

/** Draw rectangle (indexed mode only) function like macro */
#define DRAW_RECT(x, y, w, h, c) draw_rect_i(x, y, w, h, (uint8_t)c)

/**@brief	Draw a rectangle on the current video memory buffer.
 * @note	All distances are measured in pixels. Optimized version for
 * packed pixel modes only.
 *
 * @param x		x coordinate of the top-left rectangle corner.
 * @param y		y coordinate of the top-left rectangle corner.
 * @param width		Width of the rectangle (measured horizontaly).
 * @param height	Height of the rectangle (measured horizontaly).
 * @param color		Color of the rectangle.
 */
void draw_rect_i(uint16_t x,
                 uint16_t y,
                 uint16_t width,
                 uint16_t height,
                 uint8_t color);

/** Draw sprite (indexed mode only) function like macro */
#define DRAW_SPRITE(s, x, y, t) draw_sprite_i(s, x, y, (uint8_t)t)

/**@brief	Copies a given sprite to a buffer at the specified coordinates
 * (skipping transparent pixels).
 * @note	Uses double buffering. Optimized version for packed pixel modes
 * only.
 *
 * @param sprite	Sprite to copy the information from.
 * @param x		Number of pixels from the left margin at which the copy
 * will be placed.
 * @param y		Number of pixels from the top margin at which the copy
 * will be placed.
 * @param transp	Color to skip (considered transparent).
 */
void draw_sprite_i(Sprite_t* sprite,
                   const uint16_t x,
                   const uint16_t y,
                   const uint8_t transp);

/** Draw sprite (indexed mode only), changing transparent to given color,
 * function like macro */
#define DRAW_SPRITE_OVR(s, x, y, t, b) \
  draw_sprite_ovr_i(s, x, y, (uint8_t)t, (uint8_t)b)

/**@brief	Copies a given sprite to a buffer at the specified coordinates
 *		(coloring transparent pixels to a given color (usually the
 *background color)).
 * @note	Uses double buffering. Optimized version for packed pixel modes
 *only.
 *
 * @param sprite	Sprite to copy the information from.
 * @param x		Number of pixels from the left margin at which the copy
 *will be placed.
 * @param y		Number of pixels from the top margin at which the copy
 *will be placed.
 * @param transp	Color to change (considered transparent).
 * @param bkg		Color to change to (usually the background color).
 */
void draw_sprite_ovr_i(Sprite_t* sprite,
                       const uint16_t x,
                       const uint16_t y,
                       const uint8_t transp,
                       const uint8_t bkg);
/**	@} */
/**@}*/

/* DIVIDE COLOR INTO ITS R, G AND B COMPONENTS */
inline uint8_t
r_color(uint32_t color, uint8_t RedLSB)
{ // get red field from color
  return color >> RedLSB;
}

inline uint8_t
g_color(uint32_t color, uint8_t RedLSB, uint8_t GreenLSB)
{ // get green field from color
  return (color << (31 - RedLSB)) >> (31 - RedLSB + GreenLSB);
}

inline uint8_t
b_color(uint32_t color, uint8_t RedLSB, uint8_t GreenLSB, uint8_t BlueLSB)
{ // get blue field from color
  return (color << (31 - GreenLSB)) >> (31 - GreenLSB + BlueLSB);
}

#endif // __VG_H__
