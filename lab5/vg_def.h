#ifndef _LCOM_VG_DEF_H_
#define _LCOM_VG_DEF_H_

#include <lcom/lcf.h>


#define GPUINTNO		0x10
#define MiB			BIT(20)  // 1024 ^ 2
#define GPUPAGESIZE		64  //kB
/* 12 is (8 + 4) - address came shifted to left (address is 20 bits) */
#define LIN2VIRT(base, far)	(base +(\
				((far & 0xFFFF0000) >> 12) +\
				(far & 0xFFFF)));
#define	VESASIGN		"VESA"
#define	VBE2SIGN		"VBE2"

/* modes */
#define VBE_SET_LIN_FRMBUF BIT(14)
#define VBE_TEXT_MODE	   0x00
#define VBE_PACKED_PIXEL   0x04
#define VBE_DIRECT_COLOR   0x06

/* functions */
#define VBE_FUNCTION	  0x4F
#define VBE_GET_CTRL_FUNC 0x00
#define VBE_GET_MODE_FUNC 0x01
#define VBE_SET_MODE_FUNC 0x02

/* sys_int86 return values */
#define VBE_OK		0x00
#define VBE_FSUP	0x4F
#define VBE_FFAIL	0x01
#define VBE_FNOTSUP	0x02
#define VBE_FINVAL	0x03

#endif  // _LCOM_VG_DEF_H_
