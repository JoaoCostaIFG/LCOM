#include <lcom/lcf.h>

#include "utils.h"
#include "vg_def.h"
#include "vg_utils.h"


int
privctl(phys_bytes base, phys_bytes size)
{
	/* set privileges for our process to access low memory */
	struct minix_mem_range mr;
	mr.mr_base = base;
	mr.mr_limit = mr.mr_base + size;

	int privctl_answer;
	if (VBE_OK != (privctl_answer = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
		printf("sys_privctl (ADD_MEM) failed: %d\n", privctl_answer);
		return 1;
	}

	return 0;
}


int
(vbe_get_modeinfo)(uint16_t mode, vbe_mode_info_t *info)
{
	/* set privileges for the first MiB of low memory */
	/*if (privctl(0, MiB))*/  // not needed anymore
		/*return 1;*/

	/* alloc memory in low memory for info struct
	   because it needs to be accessible both in real and protected mode */
	mmap_t buf;
	if (lm_alloc(sizeof(vbe_mode_info_t), &buf) == NULL) {  // alloc 256 bytes
		printf("lm_alloc: memory allocation failed\n");
		return 1;
	}


	/* get mode info */
	reg86_t reg86;
	memset(&reg86, 0, sizeof(reg86));  // fill struct with zeros

	reg86.intno = GPUINTNO;
	reg86.ah = VBE_FUNCTION;
	reg86.al = VBE_GET_MODE_FUNC;
	reg86.es = PB2BASE(buf.phys);
	reg86.di = PB2OFF(buf.phys);
	reg86.cx = mode;

	/* setting mode and error handling */
	if (sys_int86(&reg86) != VBE_OK) {
		printf("sys_int86(): failed\n");
		lm_free(&buf);
		return 1;
	}
	else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
		printf("sys_int86(): VBE couldn't get mode 0x%X information", mode);
		lm_free(&buf);
		return 1;
	}

	/* free memory */
	*info = *((vbe_mode_info_t*) buf.virt);
	if (!lm_free(&buf))
		printf("WARNING: lm_free: memory deallocation failed!");

	return 0;
}


int
vbe_set_mode(uint16_t mode)
{
	/* set privileges for the first MiB of low memory */
	if (privctl(0, MiB))
		return 1;

	/* set mode */
	reg86_t reg86;
	memset(&reg86, 0, sizeof(reg86));  // fill struct with zeros

	reg86.intno = GPUINTNO;
	reg86.ah = VBE_FUNCTION;
	reg86.al = VBE_SET_MODE_FUNC;
	reg86.bx = (mode | VBE_SET_LIN_FRMBUF);

	/* setting mode and error handling */
	if (sys_int86(&reg86) != VBE_OK) {
		printf("sys_int86(): failed\n");
		return 1;
	}
	else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
		printf("sys_int86(): VBE couldn't set mode 0x%X", mode);
		return 1;
	}

	return 0;
}


int
vbe_get_ctrlinfo(VbeInfoBlock_t *info, uint32_t *base_ptr)
{
	/* set privileges for the first MiB of low memory */
	if (privctl(0, MiB))
		return 1;

	/* alloc memory in low memory for info struct
	   because it needs to be accessible both in real and protected mode */
	mmap_t buf;
	if (lm_alloc(sizeof(VbeInfoBlock_t), &buf) == NULL) {
		printf("lm_alloc: memory allocation failed\n");
		return 1;
	}

	/* Initialize VbeSignature = "VBE2" */
	memcpy(((VbeInfoBlock_t*) buf.virt)->VbeSignature, VBE2SIGN, sizeof(VBE2SIGN));

	/* get mode info */
	reg86_t reg86;
	memset(&reg86, 0, sizeof(reg86));  // fill struct with zeros

	reg86.intno = GPUINTNO;
	reg86.ah = VBE_FUNCTION;
	reg86.al = VBE_GET_CTRL_FUNC;
	reg86.es = PB2BASE(buf.phys);
	reg86.di = PB2OFF(buf.phys);

	/* setting mode and error handling */
	if (sys_int86(&reg86) != VBE_OK) {
		printf("sys_int86(): failed\n");
		lm_free(&buf);
		return 1;
	}
	else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
		printf("sys_int86(): Couldn't get controller info\n");
		lm_free(&buf);
		return 1;
	}

	if (memcmp(((VbeInfoBlock_t*) buf.virt)->VbeSignature, VESASIGN, sizeof(VESASIGN))) {
		printf("memcmp: VESA signature verification failed\n");
		return 1;
	}

	/* copy info to our struct */
	*info = *((VbeInfoBlock_t*) buf.virt);
	/* get base address for far pointer calculations */
	*base_ptr = (uint32_t) buf.virt - buf.phys;

	/* free memory */
	if (!lm_free(&buf))
		printf("WARNING: lm_free: memory deallocation failed!");

	return 0;
}
