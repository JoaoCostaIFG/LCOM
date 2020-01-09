#include <lcom/lcf.h>

#include "include/kbc_utils.h"
#include "include/kbd.h"
#include "include/utils.h"

/* GLOBAL VARIABLES */
/** @brief	The latest packet read from the keyboard. */
static uint8_t kout_buf = 0;
/** @brief	Indicates whether the latest packet read from the keyboard is
 * valid. */
static bool kbyte_valid = false;

/* INTERRUPT HANDLER */
void
kbd_ih(void)
{
  /* get current byte on the output buffer */
  if (!check_status_read(false)) { // check for errors (is not mouse data)
    kbyte_valid = true;
    util_sys_inb(KBC_IO_BUF, &kout_buf);
  }
  else
    kbyte_valid = false;
}

/* GETTERS */
uint8_t
get_kout_buf(void)
{
  return kout_buf;
}

bool
get_kbyte_valid(void)
{
  return kbyte_valid;
}

int
kbd_restore_conf(void)
{
  /* *looks at cucs* Ai Jesus, que bixo mais feio ?¿! */
  if (kbc_restore_dflt_conf())
    return 1;

  return 0;
}

int
kbd_scancode(bool* make, uint8_t* bytes)
{
  /* if the scancode is a 2 byte scancode, save first byte in array
   * and next time this function is called, save next byte
   */
  static uint8_t size = 0;

  if (kout_buf == SECONDBYTE) { // first byte of 2 byte scancode
    bytes[1] = kout_buf;
    size     = 1;

    return 1;
  }

  if (size == 0) {
    bytes[1] = 0;
    bytes[0] = kout_buf;
  }
  else { // size == 1
    bytes[0] = kout_buf;
  }
  size = 0;

  /* check if it isn't a make code */
  *make = true;
  if (kout_buf & BIT(7))
    *make = false;

  return 0;
}
