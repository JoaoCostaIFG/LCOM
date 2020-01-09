/** @file vg_utils.h */
#ifndef __VG_UTILS_H__
#define __VG_UTILS_H__

#include <lcom/lcf.h>
#include <stdint.h>

/** @addtogroup	vg_grp
 * @{
 */

#pragma pack(1)
/** @struct VBEINFOBLOCK_T
 * Struct used to read and store a graphics controller info. */
typedef struct VBEINFOBLOCK_T
{
  uint8_t VbeSignature[4];      /**< @brief	db - "VESA"	 */
  uint16_t VbeVersion;          /**< @brief	dw - 0x0200	 */
  phys_bytes OemStringPtr;      /**< @brief	dd - ?		 */
  uint8_t Capabilities[4];      /**< @brief	db - 4 dup (?)	 */
  phys_bytes VideoModePtr;      /**< @brief	dd - ?		 */
  uint16_t TotalMemory;         /**< @brief	dw - ?		 */
  uint16_t OemSoftwareRev;      /**< @brief	dw - ?		 */
  phys_bytes OemVendorNamePtr;  /**< @brief	dd - ?		 */
  phys_bytes OemProductNamePtr; /**< @brief	dd - ?		 */
  phys_bytes OemProductRevPtr;  /**< @brief	dd - ?		 */
  uint8_t Reserved[222];        /**< @brief	db - 222 dup (?) */
  uint8_t OemData[256];         /**< @brief	db - 256 dup (?) */
} VbeInfoBlock_t;
#pragma options align = reset

/**
 * @brief	Give the application privileges over a selected low memory
 *region.
 *
 * @param base	The base/starting address of the region.
 * @param size	The size of the region in bytes.
 *
 * @return
 * @return	0, on success\n
 *		      			1, otherwise.
 */
int privctl(phys_bytes base, phys_bytes size);

/* VBE FUNC 0x00 */
/**
 * @brief	Get the graphics controller information.
 *
 * @param info		Output: struct where the info was saved.
 * @param base_ptr	Output: value of pointer to decode pointers in struct.
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_get_ctrlinfo(VbeInfoBlock_t* info, uint32_t* base_ptr);

/* VBE FUNC 0x0			1 */
/**
 * @brief	Get the controller information for a given mode.
 *
 * @param mode	Mode to get the information of.
 * @param info	Output: struct where the info was saved.
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_get_modeinfo(uint16_t mode, vbe_mode_info_t* info);

/* VBE FUNC 0x02 */
/**
 * @brief	Set the given super VGA graphics mode.
 *
 * @param mode	The mode to set.
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_set_mode(uint16_t mode);

/* VBE FUNC 0x06 */
/**
 * @brief	Get the scanline information for the currently set graphics mode
 *and settings.
 *
 * @param info	Output: array with 3 elements (allocs the memory for them), in
 *this order:\n [0] - bytes per scanline\n [			1] - actual
 *pixels per scanline (truncate to nearest complete pixel)\n [2] - maximum
 *number of scanlines.
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_get_scanline_info(uint32_t* info);

/**
 * @brief	Set the length of the scanlines (in pixels).
 * @note	If the desired value is not achievable, the next larger value
 *will be used.
 *
 * @param psize	Input: Scanline size (in pixels) to set.\n
 *		Output:	Number need to sum pointer to get to next scanline
 *(bytes per scanline).\n
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_set_scanline_psize(uint32_t* psize);

/* VBE FUNC 0x07 */
/**
 * @brief	Get display start pixel and scanline.
 *
 * @param info	Output: array with 2 elements (allocs the memory for them), in
 *this order:\n [0] - first pixel in scanline\n [			1] -
 *first displayed scanline.
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_get_display_start(uint32_t* info);

/**
 * @brief	Set display start pixel and scanline.
 *
 * @param ipixel	Display start pixel.
 * @param iscanline	Display start scanline.
 * @param vsync Use vsync
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_set_display_start(uint32_t ipixel, uint32_t iscanline, bool vsync);

/* VBE FUNC 0x08 */
/**
 * @brief	Get DAC palette format for the current graphics mode.
 *
 * @return	-1, on failure\n
 *		      DAC palette format, otherwise.
 */
int vbe_get_dac_format();

/**
 * @brief	Set DAC pallete width to a given one.
 * @note	If the display controller is not capable of selecting the
 *desired width, the next lower valid value will be selected.
 *
 * @param width	Palette width to attempt to set (in bits).
 *
 * @return	0, if the set value is the desired one\n
 *					1, otherwise.
 */
int vbe_set_dac_format(uint8_t width);

/* VBE FUNC 0x09 */
/**
 * @brief	Set a new color palette for packed pixel graphics modes.
 * @warning	Doesn't check if the given parameters make sense (e.g.:
 *start_ind + ncolors <= 255).\n That won't cause an error, just undefined
 *behavior.
 *
 * @param palette	Array of the new colors to set.
 * @param ncolors	Number of colors to replace.
 * @param start_ind	Index of the first color to replace [0, 255].
 *
 * @return	0, on success\n
 *					1, otherwise.
 */
int vbe_set_colorpalette(uint32_t* palette, uint8_t ncolors, uint8_t start_ind);

/** @} */

#endif // __VG_UTILS_H__
