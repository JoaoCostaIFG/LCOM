#include "include/vg_utils.h"
#include "include/err_utils.h"
#include "include/utils.h"
#include "include/vg_def.h"

int
privctl(phys_bytes base, phys_bytes size)
{
  /* set privileges for our process to access low memory */
  struct minix_mem_range mr;
  mr.mr_base  = base;
  mr.mr_limit = mr.mr_base + size;

  int privctl_answer;
  if (VBE_OK != (privctl_answer = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    printf("sys_privctl (ADD_MEM) failed: %d\n", privctl_answer);
    return 1;
  }

  return 0;
}

/* VBE FUNC 0x00 */
int
vbe_get_ctrlinfo(VbeInfoBlock_t* info, uint32_t* base_ptr)
{
  /* alloc memory in low memory for info struct
           because it needs to be accessible both in real and protected mode */
  mmap_t buf;
  if (lm_alloc(sizeof(VbeInfoBlock_t), &buf) == NULL) {
    printf("lm_alloc: memory allocation failed\n");
    return 1;
  }

  /* Initialize VbeSignature = "VBE2" */
  memcpy(((VbeInfoBlock_t*)buf.virt)->VbeSignature, VBE2SIGN, sizeof(VBE2SIGN));

  /* get mode info */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_GET_CTRL_FUNC;
  reg86.es    = PB2BASE(buf.phys);
  reg86.di    = PB2OFF(buf.phys);

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    printf("sys_int86: failed\n");
    lm_free(&buf);
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    printf("sys_int86: Couldn't get controller info\n");
    lm_free(&buf);
    return 1;
  }

  if (memcmp(((VbeInfoBlock_t*)buf.virt)->VbeSignature,
             VESASIGN,
             sizeof(VESASIGN))) {
    printf("memcmp: VESA signature verification failed\n");
    return 1;
  }

  /* copy info to our struct */
  *info = *((VbeInfoBlock_t*)buf.virt);
  /* get base address for far pointer calculations */
  *base_ptr = (uint32_t)buf.virt - buf.phys;

  /* free memory */
  if (!lm_free(&buf)) // failing to free memory is not treated as an error
    printf("WARNING: lm_free: memory deallocation failed!");

  return 0;
}

/* VBE FUNC 0x01 */
int
vbe_get_modeinfo(uint16_t mode, vbe_mode_info_t* info)
{
  /* alloc memory in low memory for info struct
           because it needs to be accessible both in real and protected mode */
  mmap_t buf;
  if (lm_alloc(sizeof(vbe_mode_info_t), &buf) == NULL) { // alloc 256 bytes
    printf("lm_alloc: memory allocation failed\n");
    return 1;
  }

  /* get mode info */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_GET_MODE_FUNC;
  reg86.es    = PB2BASE(buf.phys);
  reg86.di    = PB2OFF(buf.phys);
  reg86.cx    = mode;

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    printf("sys_int86: failed\n");
    lm_free(&buf);
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    printf("sys_int86: VBE couldn't get mode 0x%X information", mode);
    lm_free(&buf);
    return 1;
  }

  /* free memory */
  *info = *((vbe_mode_info_t*)buf.virt);
  if (!lm_free(&buf)) // failing to free memory is not treated as an error
    printf("WARNING: lm_free: memory deallocation failed!");

  return 0;
}

/* VBE FUNC 0x02 */
int
vbe_set_mode(uint16_t mode)
{
  /* set mode */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_SET_MODE_FUNC;
  reg86.bx    = (mode | VBE_SET_LIN_FRMBUF);

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    printf("sys_int86: failed\n");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    printf("sys_int86: VBE couldn't set mode 0x%X", mode);
    return 1;
  }

  return 0;
}

/* VBE FUNC 0x06 */
int
vbe_get_scanline_info(uint32_t* info)
{
  /* get curr scanline info */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_SCANLINE_FUNC;
  reg86.bl    = VBE_GET_SCAN_INFO_OP;

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: Couldn't get scanline info");
    return 1;
  }

  /* allocate space for the information and save it */
  info = (uint32_t*)malloc(sizeof(int) * 3);
  if (!info) {
    warn("%s: memory allocation failed", __func__);
    return 1;
  }

  info[0] = reg86.bx; // bytes per scanline
  info[1] =
    reg86.cx; // actual pixels per scanline (truncate to nearest complete pixel)
  info[2] = reg86.dx; // maximum number of scanlines

  return 0;
}

int
vbe_set_scanline_psize(uint32_t* psize)
{
  /* set new scan line size, in pixels */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_SCANLINE_FUNC;
  reg86.bl    = VBE_SET_SCAN_PIX_OP;
  reg86.cx    = *psize; // size (in pixels) to set

  /* set scanline size and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: Couldn't set scanline size");
    return 1;
  }

  /* number we need to sum to video mem pointer to get next scanline */
  *psize = reg86.bx;
  return 0;
}

/* VBE FUNC 0x07 */
int
vbe_get_display_start(uint32_t* info)
{
  /* get current display start info */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_DISP_START_FUNC;
  reg86.bl    = VBE_GET_START_OP;

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: Couldn't get scanline info");
    return 1;
  }

  /* allocate space for the information and save it */
  info = (uint32_t*)malloc(sizeof(int) * 2);
  if (!info) {
    warn("%s: memory allocation failed", __func__);
    return 1;
  }

  info[0] = reg86.cx; // first pixel in scanline
  info[1] = reg86.dx; // first displayed scanline

  return 0;
}

int
vbe_set_display_start(uint32_t ipixel, uint32_t iscanline, bool vsync)
{
  /* set new scan line size, in pixels */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_DISP_START_FUNC;
  reg86.bh    = 0x00; // reserved (must be 0)
  reg86.bl    = vsync ? VBE_SET_START_OP_VSYNC : VBE_GET_START_OP;
  reg86.cx    = ipixel;    // first pixel in scanline
  reg86.dx    = iscanline; // first scanline

  /* set scanline size and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: Couldn't set scanline size");
    return 1;
  }

  return 0;
}

/* VBE FUNC 0x08 */
int
vbe_get_dac_format()
{
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_DAC_FUNC;
  reg86.bl    = VBE_GET_DAC_OP;

  /* getting info and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return -1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: VBE function 0x08 - set/get dac palette failed");
    return -1;
  }

  return reg86.bh;
}

/* VBE FUNC 0x08 */
int
vbe_set_dac_format(uint8_t width)
{
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_DAC_FUNC;
  reg86.bl    = VBE_SET_DAC_OP;
  reg86.bh    = width; // dac palette size to set (in bits)

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: VBE function 0x08 - set dac palette failed");
    return 1;
  }

  /* success if current num of bits is the same as the one tried */
  return !(reg86.bh == width);
}

/* VBE FUNC 0x09 */
int
vbe_set_colorpalette(uint32_t* palette, uint8_t ncolors, uint8_t start_ind)
{
  if (!palette) {
    warn("%s: given palette is an uninitialized pointer", __func__);
    return 1;
  }

  /* alloc memory in low memory for info struct
           because it needs to be accessible both in real and protected mode */
  mmap_t buf;
  if (lm_alloc(sizeof(int) * ncolors, &buf) == NULL) {
    warn("lm_alloc: memory allocation failed");
    return 1;
  }

  /* copy pallete to low level memory */
  memcpy((uint32_t*)buf.virt, palette, sizeof(int) * ncolors);

  /* get mode info */
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); // fill struct with zeros

  reg86.intno = GPUINTNO;
  reg86.ah    = VBE_FUNCTION;
  reg86.al    = VBE_PALETTE_FUNC;
  reg86.bl    = VBE_SET_PALT_OP;
  reg86.cx    = ncolors;   // number of regs to update (max 256)
  reg86.dx    = start_ind; // first to update
  reg86.es    = PB2BASE(buf.phys);
  reg86.di    = PB2OFF(buf.phys);

  /* setting mode and error handling */
  if (sys_int86(&reg86) != VBE_OK) {
    warn("sys_int86: failed");
    lm_free(&buf);
    return 1;
  }
  else if (reg86.ah != VBE_OK || reg86.al != VBE_FSUP) {
    warn("sys_int86: Couldn't set new palette info");
    lm_free(&buf);
    return 1;
  }

  /* free memory */
  if (!lm_free(&buf)) // failing to free memory is not treated as an error
    warn("lm_free: memory deallocation failed");

  return 0;
}
