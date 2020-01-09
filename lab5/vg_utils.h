#ifndef _LCOM_VG_UTILS_H_
#define _LCOM_VG_UTILS_H_

#include <lcom/lcf.h>

#pragma pack(1)
typedef struct {
	uint8_t		VbeSignature[4];	/* db - "VESA"		*/
	uint16_t	VbeVersion;		/* dw - 0x0200		*/
	phys_bytes	OemStringPtr;		/* dd - ?		*/
	uint8_t		Capabilities[4];	/* db - 4 dup (?)	*/
	phys_bytes	VideoModePtr;		/* dd - ?		*/
	uint16_t	TotalMemory;		/* dw - ?		*/
	uint16_t	OemSoftwareRev;		/* dw - ?		*/
	phys_bytes	OemVendorNamePtr;	/* dd - ?		*/
	phys_bytes	OemProductNamePtr;	/* dd - ?		*/
	phys_bytes	OemProductRevPtr;	/* dd - ?		*/
	uint8_t		Reserved[222];		/* db - 222 dup (?)	*/
	uint8_t		OemData[256];		/* db - 256 dup (?)	*/
} VbeInfoBlock_t;
#pragma options align = reset

int privctl(phys_bytes base, phys_bytes size);

int vbe_get_modeinfo(uint16_t mode, vbe_mode_info_t *info);

int vbe_set_mode(uint16_t mode);

int vbe_get_ctrlinfo(VbeInfoBlock_t *info, uint32_t *base_ptr);

#endif	// _LCOM_VG_UTILS_H_
