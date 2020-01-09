/** @file vg_def.h */
#ifndef __VG_DEF_H__
#define __VG_DEF_H__

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

/** @defgroup vg_grp	Video Graphics */

/** @defgroup sprite_grp	Sprites */

/** @addtogroup	vg_grp
 *
 * @brief	Code and constants related to the configuration and display of
 * video information.
 *
 * @{
 */

// typedef unsigned long phys_bytes;

#define GPUINTNO    0x10
#define MiB         BIT(20) // 1024 ^ 2
#define GPUPAGESIZE 64      // kB

/* 12 is (8 + 4) - address came shifted to left (address is 20 bits) */
#define LIN2VIRT(base, far) \
  (base + (((far & 0xFFFF0000) >> 12) + (far & 0xFFFF)));

#define VESASIGN "VESA"
#define VBE2SIGN "VBE2"

/* modes info */
#define VBE_SET_LIN_FRMBUF BIT(14)
#define VBE_TEXT_MODE      0x00
#define VBE_PACKED_PIXEL   0x04
#define VBE_DIRECT_COLOR   0x06

/* functions */
#define VBE_FUNCTION 0x4F

#define VBE_GET_CTRL_FUNC 0x00

#define VBE_GET_MODE_FUNC 0x01

#define VBE_SET_MODE_FUNC 0x02

#define VBE_SCANLINE_FUNC    0x06
#define VBE_SET_SCAN_PIX_OP  0x00
#define VBE_GET_SCAN_INFO_OP 0x01

#define VBE_DISP_START_FUNC    0x07
#define VBE_SET_START_OP       0x00
#define VBE_SET_START_OP_VSYNC 0x80
#define VBE_GET_START_OP       0x01

#define VBE_DAC_FUNC    0x08
#define VBE_SET_DAC_OP  0x00
#define VBE_GET_DAC_OP  0x01
#define TRUE_COLOR_BITS 8
#define DFLT_DAC_BITS   6

#define VBE_PALETTE_FUNC      0x09
#define VBE_SET_PALT_OP       0x00
#define VBE_GET_PALT_OP       0x01
#define VBE_SET_PALT_OP_VSYNC 0x80

/* sys_int86 return values */
#define VBE_OK      0x00
#define VBE_FSUP    0x4F
#define VBE_FFAIL   0x01
#define VBE_FNOTSUP 0x02
#define VBE_FINVAL  0x03

/** @} */

#endif // __VG_DEF_H__
