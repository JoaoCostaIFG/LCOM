#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/err_utils.h"
#include "include/vg.h"
#include "include/vg_def.h"
#include "include/vg_utils.h"

#define RGB8TO6(x) ((63 * x) / 255)

/* PRIVATE */
/* VG CLASS DATA MEMBERS */
static void* show_buff;  /* Process' address where VRAM is mapped */
static void* write_buff; /* Process' address where VRAM is mapped (2nd buff) */
static unsigned
  scanline_pix; /* Size of the current logical scan line in bytes */
static phys_bytes physbaseptr; /* mode base pointer */
static unsigned h_res;         /* Horizontal resolution in pixels */
static unsigned v_res;         /* Vertical resolution in pixels */
static unsigned bitspixel;     /* Number of VRAM bits per pixel */
static unsigned bytespixel;    /* Number of VRAM bytes per pixel */
static unsigned vram_size;     /* Total syze of vram */
static uint8_t
  memory_model;    /* memory color mode (packed pixel, direct, etc...) */
static bool vsync; /* whether ot not to use vsync */
/* END VG CLASS DATA MEMBERS */

static bool
is_2nd_buff(void)
{
  /* return ao primeiro if true */
  return (show_buff > write_buff);
}

static int
vg_alloc_2nd_buff(void)
{
  /* specify that the 2nd buffer has double the current mode resolution */
  scanline_pix = h_res * 2;

  if (vbe_set_scanline_psize(&scanline_pix)) {
    write_buff = NULL;
    return 1;
  }

  /* start at the second buffer */
  write_buff = (void*)((uint8_t*)show_buff + h_res);

  return 0;
}

static int
vg_save_mode_info(uint16_t mode)
{
  /* get vbe mode info */
  vbe_mode_info_t info;
  if (vbe_get_modeinfo(mode, &info))
    return 1;

  h_res       = info.XResolution;
  v_res       = info.YResolution;
  bitspixel   = info.BitsPerPixel;
  physbaseptr = info.PhysBasePtr;

  /* packed pixel or direct mode */
  memory_model = info.MemoryModel;

  bytespixel = (bitspixel + 7) >> 3; // rounding up the byte count
  vram_size  = bytespixel * h_res * v_res;

  return 0;
}

/* PUBLIC */
/* VG GETTERS */
unsigned
get_h_res(void)
{
  return h_res;
}
unsigned
get_v_res(void)
{
  return v_res;
}
unsigned
get_bitspixel(void)
{
  return bitspixel;
}
unsigned
get_bytespixel(void)
{
  return bytespixel;
}
uint8_t
get_memory_model(void)
{
  return memory_model;
}
/* END VG GETTERS */

/* VG SETTERS */
void
vg_clear(void)
{
  uint8_t* reset_ptr = (uint8_t*)write_buff;
  for (size_t i = v_res; i > 0; --i) {
    memset(reset_ptr, 0, h_res);
    reset_ptr += scanline_pix;
  }
}

void
vg_clear_all(void)
{
  if (is_2nd_buff())
    memset(write_buff, 0, vram_size * 2);
  else
    memset(show_buff, 0, vram_size * 2);
}

int
set_truecolor(void)
{
  if (vbe_set_dac_format(TRUE_COLOR_BITS)) {
    warn("%s: Failed setting truecolor mode", __func__);
    return 1;
  }

  return 0;
}

int
set_defaultdac(void)
{
  if (vbe_set_dac_format(DFLT_DAC_BITS)) {
    warn("%s: Failed setting defaults dac size: %d\n", __func__, DFLT_DAC_BITS);
    return 1;
  }

  return 0;
}

int
set_color_palette_file(const char* const filename)
{
  FILE* fp;
  if ((fp = fopen(filename, "rb")) == NULL) {
    warn("%s: Couldn't open the PALETTE file: %s", __func__, filename);
    return 1;
  }

  uint8_t palette_size  = fgetc(fp); // number of colors
  uint32_t* new_palette = (uint32_t*)malloc(sizeof(int) * palette_size);
  if (new_palette == NULL) {
    warn("%s: Memory allocation for new palette failed", __func__);
    return 1;
  }

  /* read palette */
  uint8_t first_color_ind = fgetc(fp); // first color index
  if (vbe_get_dac_format() == TRUE_COLOR_BITS) {
    for (size_t i = 0; i < palette_size; ++i) {
      // TODO ERROR VERIFICATION BY EMPTY CHAR
      new_palette[i] = (fgetc(fp) << 16) + (fgetc(fp) << 8) + (fgetc(fp));
    }
  }
  else {
    /* converts 8 bit RBG to 6 bit RBG */
    for (size_t i = 0; i < palette_size; ++i) {
      // TODO ERROR VERIFICATION BY EMPTY CHAR
      new_palette[i] = (RGB8TO6(fgetc(fp)) << 16) + (RGB8TO6(fgetc(fp)) << 8) +
                       (RGB8TO6(fgetc(fp)));
    }
  }

  fclose(fp);

  if (vbe_set_colorpalette(new_palette, palette_size, first_color_ind)) {
    free(new_palette);
    warn("%s: palette setting failed", __func__);
    return 1;
  }
  free(new_palette);

  return 0;
}

int
set_random_color_palette(uint8_t palette_size, uint8_t first_color_ind)
{
  /* alloc palette */
  uint32_t* new_palette = (uint32_t*)malloc(sizeof(int) * palette_size);
  if (new_palette == NULL) {
    warn("%s: Memory allocation for new random palette failed", __func__);
    return 1;
  }

  /* randomize palette */
  for (size_t i = 0; i < palette_size; ++i) {
    /* new_palette[i] = rand() % 2147483647; */
    new_palette[i] = rand() % INT32_MAX;
  }

  if (vbe_set_colorpalette(new_palette, palette_size, first_color_ind)) {
    free(new_palette);
    warn("%s: random palette setting failed", __func__);
    return 1;
  }
  free(new_palette);

  return 0;
}
/* END VG SETTERS */

/* OTHER PUBLIC FUNCTIONS */
void
next_buff(void)
{
  /* let vga know about the switch */
  if (is_2nd_buff()) { // return to initial state
    if (vbe_set_display_start(0, 0, vsync))
      die("olha falhei\n");
  }
  else {
    if (vbe_set_display_start(h_res, 0, vsync))
      die("olha falhei\n");
  }

  /* switch video pointers */
  void* temp_video = show_buff;
  show_buff        = write_buff;
  write_buff       = temp_video;

  // vg_show2write();
  vg_clear();
}

void*
vginit(uint16_t mode, bool v_sync)
{
  /* get and save given mod info */
  if (vg_save_mode_info(mode))
    return NULL;

  /* set privileges for the mode buffer (2 buffers) */
  if (privctl(physbaseptr, vram_size * 2)) {
    warn("%s: video memory privilege setting failed", __func__);
    return NULL;
  }

  /* memory mapping and set video memory pointer (2 buffers) */
  show_buff = vm_map_phys(SELF, (void*)physbaseptr, vram_size * 2);
  if (show_buff == MAP_FAILED) {
    warn("%s: video memory map failed", __func__);
    return NULL;
  }

  /* set mode */
  if (vbe_set_mode(mode)) {
    warn("%s: can't get given mode (0x%X) info", __func__, mode);
    return NULL;
  }

  /* alloc second buffer */
  if (vg_alloc_2nd_buff()) {
    warn("%s: second buffer allocation failed", __func__);
    return NULL;
  }

  /* set vsync info */
  vsync = v_sync;

  /* return pointer to buffer (in virtual memory) */
  return show_buff;
}

int
vg_show_ctrl_info(void)
{
  /* #SegFaultIsMyWaifu */
  /* get controller info */
  VbeInfoBlock_t info_block;
  uint32_t base_ptr;
  if (vbe_get_ctrlinfo(&info_block, &base_ptr)) {
    warn("%s: Couldn't get video controller info", __func__);
    return 1;
  }

  /* get the relevant controller info fields */
  vg_vbe_contr_info_t info;

  /* NORMAL FIELDS */
  /* signature: "VESA" */
  memcpy(info.VBESignature,
         info_block.VbeSignature,
         sizeof(info_block.VbeSignature));
  /* vbe version: 0x0200 */
  memcpy(
    info.VBEVersion, &info_block.VbeVersion, sizeof(info_block.VbeVersion));
  /* total memory */
  info.TotalMemory = info_block.TotalMemory * GPUPAGESIZE;

  /* FAR POINTER FIELDS */
  info.OEMString     = (char*)LIN2VIRT(base_ptr, info_block.OemStringPtr);
  info.VideoModeList = (uint16_t*)LIN2VIRT(base_ptr, info_block.VideoModePtr);
  info.OEMVendorNamePtr =
    (char*)LIN2VIRT(base_ptr, info_block.OemVendorNamePtr);
  info.OEMProductNamePtr =
    (char*)LIN2VIRT(base_ptr, info_block.OemProductNamePtr);
  info.OEMProductRevPtr =
    (char*)LIN2VIRT(base_ptr, info_block.OemProductRevPtr);

  /* display the controller info */
  if (vg_display_vbe_contr_info(&info)) {
    warn("%s: Couldn't display the video controller info", __func__);
    return 1;
  }

  return 0;
}

/* DRAWING SIMPLE OBJECTS FUNCTIONS */
void
draw_line(uint16_t x, uint16_t y, uint32_t color, uint16_t len)
{
  /* Draw a line at the given coordinates on the screen with
   * the specified size and color.
   * Checks if the line fits in the given mode resolution
   */

  /* calculate starting video memory writting position */
  uint8_t* pixel_pointer =
    (uint8_t*)write_buff + (y * scanline_pix + x) * bytespixel;

  size_t h_lim; // part of the line that isn't drawn
  if ((h_lim = h_res - x) > len)
    h_lim = len;

  for (size_t j = 0; j < h_lim; ++j) {
    memcpy(pixel_pointer, &color, bytespixel);
    pixel_pointer += bytespixel;
  }
}

void
draw_rect(uint16_t x,
          uint16_t y,
          uint16_t width,
          uint16_t height,
          uint32_t color)
{
  /* Draws a rectangle at the given position.
   * Checks if the rectangle fits in the given mode resolution.
   */

  if (x >= h_res || y >= v_res) // no need to verify y outside loop because
                                // outside loop verifies it
    return;                     // nothing to draw

  size_t v_lim; // part of the line that isn't drawn
  if ((v_lim = v_res - y) > height)
    v_lim = height;

  for (size_t i = 0; i < v_lim; ++i)
    draw_line(x, y + i, color, width);
}

/* SPRITE DRAWING */
void
draw_sprite_notransp(Sprite_t* sprite, const uint16_t x, const uint16_t y)
{
  /* Draws a prite at the given coordinates */

  /* check if the given sprite data is ok */
  if (!sprite || !sprite->Data) {
    warn("%s: Unitialized pointers", __func__);
    return;
  }

  /* stop if there will be nothing to draw (already finished) */
  if (x >= h_res || y >= v_res)
    return;

  /* initialize video memory pointer at the correct position for writting */
  uint8_t* pixel_pointer =
    (uint8_t*)write_buff + (y * scanline_pix + x) * bytespixel;

  /* draws while checking to not draw out of the screen resolution */
  uint8_t* sprite_ptr = sprite->Data; // get sprite data location

  /* calculate limits */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_res - y) > sprite->Height)
    v_lim = sprite->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_res - x) > sprite->Width)
    h_lim = sprite->Width;

  for (size_t i = 0; i < v_lim; ++i) {
    memcpy(pixel_pointer,
           sprite_ptr,
           h_lim * bytespixel); // copy data to given video memory
    /* skip pointers to the next line */
    pixel_pointer += (scanline_pix * bytespixel);
    sprite_ptr += sprite->Width;
  }
}

void
draw_sprite(Sprite_t* sprite,
            const uint16_t x,
            const uint16_t y,
            const uint32_t transp)
{
  /* draws a sprite at the given coordinates */

  /* check if the given sprite data is ok */
  if (!sprite || !sprite->Data) {
    warn("%s: Unitialized pointers", __func__);
    return;
  }

  /* stop if there will be nothing to draw (already finished) */
  if (x >= h_res || y >= v_res)
    return;

  /* initialize video memory pointer at the correct position for writting */
  uint8_t* pixel_pointer =
    (uint8_t*)write_buff + (y * scanline_pix + x) * bytespixel;

  /* get the sprite data pointer and how long horizontal lines are */
  uint8_t* sprite_ptr = sprite->Data; // get sprite data location

  /* calculate limits */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_res - y) > sprite->Height)
    v_lim = sprite->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_res - x) > sprite->Width)
    h_lim = sprite->Width;

  /* how much to skip pointers on line change */
  size_t pixel_ptr_skip  = (scanline_pix - h_lim) * bytespixel;
  size_t sprite_ptr_skip = (sprite->Width - h_lim) * bytespixel;

  /* draws while checking to not draw out of the screen resolution */
  for (size_t i = y; i < (sprite->Height + y) && i < v_res; ++i) {
    for (size_t j = 0; j < h_lim; ++j) {
      /* copy data to given video memory, if it is not a transparent pixel */
      if (memcmp(sprite_ptr, &transp, bytespixel))
        memcpy(pixel_pointer, sprite_ptr, bytespixel);

      /* advance pointers to next pixel */
      pixel_pointer += bytespixel;
      sprite_ptr += bytespixel;
    }

    pixel_pointer += pixel_ptr_skip;
    /* skip sprite data that was going to be drawn outside the screen */
    sprite_ptr += sprite_ptr_skip;
  }
}

void
draw_sprite_ovr(Sprite_t* sprite,
                const uint16_t x,
                const uint16_t y,
                const uint32_t transp,
                const uint32_t bkg)
{
  /* draws a sprite at the given coordinates */

  /* check if the given sprite data is ok */
  if (!sprite || !sprite->Data) {
    warn("%s: Unitialized pointers", __func__);
    return;
  }

  /* stop if there will be nothing to draw (already finished) */
  if (x >= h_res || y >= v_res)
    return;

  /* initialize video memory pointer at the correct position for writting */
  uint8_t* pixel_pointer =
    (uint8_t*)write_buff + (y * scanline_pix + x) * bytespixel;

  /* get the sprite data pointer and how long horizontal lines are */
  uint8_t* sprite_ptr = sprite->Data; // get sprite data location

  /* calculate limits */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_res - y) > sprite->Height)
    v_lim = sprite->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_res - x) > sprite->Width)
    h_lim = sprite->Width;

  /* how much to skip pointers on line change */
  size_t pixel_ptr_skip  = (scanline_pix - h_lim) * bytespixel;
  size_t sprite_ptr_skip = (sprite->Width - h_lim) * bytespixel;

  /* draws while checking to not draw out of the screen resolution */
  for (size_t i = y; i < (sprite->Height + y) && i < v_res; ++i) {
    for (size_t j = 0; j < h_lim; ++j) {
      /* copy data to given video memory, if it is not a transparent pixel */
      if (memcmp(sprite_ptr, &transp, bytespixel))
        memcpy(pixel_pointer, sprite_ptr, bytespixel);
      else
        memcpy(pixel_pointer, &bkg, bytespixel);

      /* advance pointers to next pixel */
      pixel_pointer += bytespixel;
      sprite_ptr += bytespixel;
    }

    pixel_pointer += pixel_ptr_skip;
    /* skip sprite data that was going to be drawn outside the screen */
    sprite_ptr += sprite_ptr_skip;
  }
}

/* INDEXED MODES SPECIALIZATION */
void
draw_rect_i(uint16_t x,
            uint16_t y,
            uint16_t width,
            uint16_t height,
            uint8_t color)
{
  /* Draws a rectangle at the given position.
   * Checks if the rectangle fits in the given mode resolution.
   */

  if (x >= h_res || y >= v_res)
    return; // nothing to draw

  size_t v_lim; // lines that aren't drawn
  if ((v_lim = v_res - y) > height)
    v_lim = height;

  size_t h_lim; // part of the line that isn't drawn
  if ((h_lim = h_res - x) > width)
    h_lim = width;

  /* calculate starting video memory writting position */
  uint8_t* pixel_pointer = (uint8_t*)write_buff + (y * scanline_pix + x);

  for (; v_lim; --v_lim) {
    for (size_t j = h_lim; j; --j)
      pixel_pointer[j - 1] = color;

    pixel_pointer += scanline_pix;
  }
}

void
draw_sprite_i(Sprite_t* sprite,
              const uint16_t x,
              const uint16_t y,
              const uint8_t transp)
{
  /* draws a sprite at the given coordinates */
  /* stop if there will be nothing to draw (already finished) */
  if (x >= h_res || y >= v_res)
    return;

  /* check if the given sprite data is ok */
  if (!sprite || !sprite->Data) {
    warn("%s: Unitialized pointers", __func__);
    return;
  }

  /* initialize video memory pointer at the correct position for writting */
  uint8_t* pixel_pointer = (uint8_t*)write_buff + (y * scanline_pix + x);

  /* get the sprite data pointer and how long horizontal lines are */
  uint8_t* sprite_ptr = sprite->Data; // get sprite data location

  /* calculate limits */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_res - y) > sprite->Height)
    v_lim = sprite->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_res - x) > sprite->Width)
    h_lim = sprite->Width;

  /* draws while checking to not draw out of the screen resolution */
  for (; v_lim; --v_lim) {
    for (size_t j = h_lim; j; --j) {
      /* copy data to given video memory, if it is not a transparent pixel */
      if (sprite_ptr[j - 1] != transp)
        pixel_pointer[j - 1] = sprite_ptr[j - 1];
    }

    pixel_pointer += scanline_pix;
    /* skip sprite data that was going to be drawn outside the screen */
    sprite_ptr += sprite->Width;
  }
}

void
draw_sprite_ovr_i(Sprite_t* sprite,
                  const uint16_t x,
                  const uint16_t y,
                  const uint8_t transp,
                  const uint8_t bkg)
{
  /* draws a sprite at the given coordinates */
  /* stop if there will be nothing to draw (already finished) */
  if (x >= h_res || y >= v_res)
    return;

  /* check if the given sprite data is ok */
  if (!sprite || !sprite->Data) {
    warn("%s: Unitialized pointers", __func__);
    return;
  }

  /* initialize video memory pointer at the correct position for writting */
  uint8_t* pixel_pointer = (uint8_t*)write_buff + (y * scanline_pix + x);

  /* get the sprite data pointer and how long horizontal lines are */
  uint8_t* sprite_ptr = sprite->Data; // get sprite data location

  /* calculate limits */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_res - y) > sprite->Height)
    v_lim = sprite->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_res - x) > sprite->Width)
    h_lim = sprite->Width;

  /* draws while checking to not draw out of the screen resolution */
  for (; v_lim; --v_lim) {
    for (size_t j = h_lim; j; --j) {
      /* copy data to given video memory, if it is not a transparent pixel */
      if (sprite_ptr[j - 1] != transp)
        pixel_pointer[j - 1] = sprite_ptr[j - 1];
      else
        pixel_pointer[j - 1] = bkg;
    }

    pixel_pointer += scanline_pix;
    /* skip sprite data that was going to be drawn outside the screen */
    sprite_ptr += sprite->Width;
  }
}
