#include <lcom/lcf.h>

#include "i8254.h"
#include "i8042.h"
#include "kbc_utils.h"
#include "timer_utils.h"
#include "utils.h"
#include "vg.h"
#include "vg_def.h"
#include "vg_utils.h"

/* VG BUFFER CLASS LIKE DEFINITIONS */
static void *video_mem;		 /* Process (virtual) address to which VRAM is mapped */
static phys_bytes physbaseptr;	 /* mode base pointer */
static unsigned h_res;		 /* Horizontal resolution in pixels */
static unsigned v_res;		 /* Vertical resolution in pixels */
static unsigned bitspixel;	 /* Number of VRAM bits per pixel */
static unsigned bytespixel;	 /* Number of VRAM bytes per pixel */
static unsigned vram_size;	 /* Total syze of vram */
static uint8_t RedScreeMask;
static uint8_t RedLSB;
static uint8_t GreenScreeMask;
static uint8_t GreenLSB;
static uint8_t BlueScreeMask;
static uint8_t BlueLSB;
static uint8_t memory_model;	 /* memory color mode (packed pixel, direct, text, etc...) */
/* END VG BUFFER CLASS LIKE DEFINITIONS */


/* VG BUFFER CLASS GETTERS */
unsigned get_h_res() { return h_res; }
unsigned get_v_res() { return v_res; }
unsigned get_bitspixel() { return bitspixel; }
unsigned get_bytespixel() { return bytespixel; }
uint8_t get_RedScreenMask() { return RedScreeMask; }
uint8_t get_RedLSB() { return RedLSB; }
uint8_t get_GreenScreenMask() { return GreenScreeMask; }
uint8_t get_GreenLSB() { return GreenLSB; }
uint8_t get_BlueScreenMask() { return BlueScreeMask; }
uint8_t get_BlueLSB() { return BlueLSB; }
uint8_t get_memory_model() { return memory_model; }
/* END VG BUFFER CLASS GETTERS */


int
vg_save_mode_info(uint16_t mode)
{
	/* get vbe mode info */
	vbe_mode_info_t info;
	if (vbe_get_modeinfo(mode, &info)) {
		printf("Can't get given mode (0x%X) info\n", mode);
		return 1;
	}

	h_res = info.XResolution;
	v_res = info.YResolution;
	bitspixel = info.BitsPerPixel;
	physbaseptr = info.PhysBasePtr;

	/* rgb info */
	RedScreeMask = info.RedMaskSize;
	RedLSB = info.RedFieldPosition;

	GreenScreeMask = info.GreenMaskSize;
	GreenLSB = info.GreenFieldPosition;

	BlueScreeMask = info.BlueMaskSize;
	BlueLSB = info.BlueFieldPosition;

	/* packed pixel or direct mode */
	memory_model = info.MemoryModel;

	bytespixel = (bitspixel + 7) >> 3;  // rounding up the byte count
	vram_size = bytespixel * h_res * v_res;

	return 0;
}


int
vg_show_ctrl_info()
{
	/* #SegFaultIsMyWaifu */
	/* get controller info */
	VbeInfoBlock_t info_block;
	uint32_t base_ptr;
	if (vbe_get_ctrlinfo(&info_block, &base_ptr)) {
		printf("vbe_get_ctrlinfo: Couldn't get video controller info\n");
		return 1;
	}

	/* get the relevant controller info fields */
	vg_vbe_contr_info_t info;

	/* NORMAL FIELDS */
	/* signature: "VESA" */
	memcpy(info.VBESignature, info_block.VbeSignature, sizeof(info_block.VbeSignature));
	/* vbe version: 0x0200 */
	memcpy(info.VBEVersion, &info_block.VbeVersion, sizeof(info_block.VbeVersion));
	/* total memory */
	info.TotalMemory = info_block.TotalMemory * GPUPAGESIZE;

	/* FAR POINTER FIELDS */
	info.OEMString = (char*) LIN2VIRT(base_ptr, info_block.OemStringPtr);
	info.VideoModeList = (uint16_t*) LIN2VIRT(base_ptr, info_block.VideoModePtr);
	info.OEMVendorNamePtr = (char*) LIN2VIRT(base_ptr, info_block.OemVendorNamePtr);
	info.OEMProductNamePtr = (char*) LIN2VIRT(base_ptr, info_block.OemProductNamePtr);
	info.OEMProductRevPtr = (char*) LIN2VIRT(base_ptr, info_block.OemProductRevPtr);

	/* display the controller info */
	if (vg_display_vbe_contr_info(&info)) {
		printf("vg_display_vbe_contr_info: Couldn't display the video controller info\n");
		return 1;
	}

	return 0;
}


void*
(vg_init)(uint16_t mode)
{
	/* get given mod info */
	if (vg_save_mode_info(mode))
		return NULL;

	/* set privileges for the mode buffer */
	if (privctl(physbaseptr, vram_size))
		return NULL;

	/* memory mapping and video memory pointer */
	video_mem = vm_map_phys(SELF, (void *)physbaseptr, vram_size);
	if (video_mem == MAP_FAILED) {
		printf("Video memory map failed\n");
		return NULL;
	}

	/* set mode */
	if (vbe_set_mode(mode))
		return NULL;

	/* return pointer to buffer (in virtual memory) */
	return video_mem;
}


void
vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
	/* calculate starting video memory writting position */
	uint8_t* pixel_pointer = (uint8_t*) video_mem + (y * h_res + x) * bytespixel;

	for (size_t j = 0; j < len && (j + x) < h_res; ++j) {
		memcpy(pixel_pointer, &color, bytespixel);
		pixel_pointer += bytespixel;
	}

	/* old code */
	/*uint8_t* color_part;*/
	/*color_part = (uint8_t*) &color;*/
	/*for (size_t col_num = 0; col_num < bytespixel; ++col_num)*/
		/**(pixel_pointer++) = *(color_part++);*/

	/**(pixel_pointer + (y * h_res + (x + j)) * bytespixel + col_num) = color_part;*/
}


void
vg_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	/* draws a rectangle at the given position */
	/* check if the rectangle fits in the given mode resolution */
	for (size_t i = 0; i < height && (i + y) < v_res; ++i)
		vg_draw_line(x, y + i, width, color);
}


int
vg_load_sprite(xpm_map_t* xpm, xpm_image_t* img)
{
	/* get sprite info and load it to memory */
	if (xpm_load(*xpm, XPM_INDEXED, img) == NULL) {
		printf("xpm_load: Failed to load xpm\n");
		return 1;
	}

	return 0;
}


void
vg_draw_sprite(const xpm_image_t *img, uint16_t x, uint16_t y)
{
	/* draw a xpm sprite starting at pos (x, y) */
	if (!img || !img->bytes) {
		printf("vg_draw_sprite: Unitialized pointers\n");
		return;
	}
	else if (x >= h_res || y >= v_res)  // stop if there will be nothing to draw
		return;

	/* initialize video memory pointer at correct position */
	uint8_t *pixel_pointer = (uint8_t*) video_mem + (y * h_res + x) * bytespixel;
	uint8_t *sprite = img->bytes;  // sprite data location

	/* checks to not draw out of the screen */
	size_t h_lim = 0;
	for (size_t i = 0; i < img->height && (i + y) < v_res; ++i) {
		if ((h_lim = h_res - x) > img->width)
			h_lim = img->width;

		memcpy(pixel_pointer, sprite, h_lim * bytespixel);
		pixel_pointer += h_res * bytespixel;  // skip to next line
		sprite += img->width;  // skip to the next line
	}

	/* old code */
	/*for (j = 0; j < img->width && (j + x) < h_res; ++j)*/
		/**(pixel_pointer++) = *(sprite++);*/
}



int
vg_animate_sprite(xpm_map_t *xpm, uint16_t xi, uint16_t xf, uint16_t yi, uint16_t yf, int16_t speed, uint8_t fr_rate)
{
	/* INTERRUPT HANDLER VARIABLES */
	extern uint8_t g_out_buf;
	extern bool byte_valid;
	extern unsigned int g_timer_cnt;

	/* SUBSCRIBE INTERRUPTS */ //TODO SET FREQUENCY????
	/* subscribe kbd interrupts */
	int kbd_hook_id = KBD_IRQ;
	const int kbd_irq_sel = BIT(KBD_IRQ);
	if (subscribe_int(&kbd_hook_id, KBD_IRQ, true)) {  // subscribe the interupts in EXCLUSIVE mode
		printf("subscribe_int: Keyboard interrupt subscribing failed (in EXCLUSIVE mode)\n");
		return 1;
	}
	/* subscribe timer 0 interrupts */
	int timer0_hook_id = TIMER0_IRQ;
	const int timer0_irq_sel = BIT(TIMER0_IRQ);
	if (subscribe_int(&timer0_hook_id, TIMER0_IRQ, false)) {  // subscribe the interupts (not EXCLUSIVE)
		printf("subscribe_int: Timer 0 interrupt subscribing failed\n");
		return 1;
	}


	/* RECTIFY SPEED AND FRAME RATE */
	uint16_t fps = 0;
	if (speed < 0) {
		fps = -speed;
		speed = 1;
	}
	else
		fps = 60 / fr_rate;  // convert frequency to period
	if (xi > xf || yi > yf)  // movement with negative velocity
		speed *= -1;

	/* LOAD SPRITE */
	xpm_image_t img;
	if (vg_load_sprite(xpm, &img))
		return 1;

	/* DRAW ANIMATION */
	/* define direction variables */
	uint16_t delta_x = abs(xf - xi);
	uint16_t delta_y = abs(yf - yi);
	uint16_t abs_speed = abs(speed);

	vg_draw_sprite(&img, xi, yi);  // draw first sprite
	/* handle incoming packets */
	int ipc_status, r;
	message msg;
	while (g_out_buf != ESCBREAK) {
		/* get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)))
			printf("driver_receive failed with: %d", r);

		else if (is_ipc_notify(ipc_status) && (_ENDPOINT_P(msg.m_source) == HARDWARE)) {  // received notification
			if (msg.m_notify.interrupts & kbd_irq_sel) {  // subscribed interrupt
				if (!check_status_read(false))	// test if the byte was read correctly
					kbc_ih();  // call keyboard handler
			}
			else if (msg.m_notify.interrupts & timer0_irq_sel && (delta_x || delta_y)) {
				timer_int_handler();  // timer interrupt handler

				/* draw the next frame */
				if (g_timer_cnt >= fps) {
					if (delta_x >= delta_y) {
						if (delta_x < abs_speed) {
							vg_draw_rect(xi, yi, xf - xi, img.height, 0);
							xi = xf;
							delta_x = 0;
						}
						else {
							vg_draw_rect(xi, yi, speed, img.height, 0);
							xi += speed;
							delta_x -= abs_speed;
						}
					}
					else {
						if (delta_y < abs_speed) {
							vg_draw_rect(xi, yi, img.width, yf - yi, 0);
							yi = yf;
							delta_y = 0;
						}
						else {
							vg_draw_rect(xi, yi, img.width, speed, 0);
							yi += speed;
							delta_y -= abs_speed;
						}
					}

					vg_draw_sprite(&img, xi, yi);
					g_timer_cnt = 0;
				}
			}

		}
	}


	/* UNSUBSCRIBE INTERRUPTS */
	/* unsubscribe kbd interrupts */
	if (unsubscribe_int(&kbd_hook_id)) {
		printf("unsubscribe_int: Couldn't unsubscribe keyboard interrupts\n");
		return 1;
	}

	/* unsubscribe timer1 interrupts */
	if (unsubscribe_int(&timer0_hook_id)) {
		printf("unsubscribe_int: Couldn't unsubscribe timer 0 interrupts\n");
		return 1;
	}

	return 0;
}
