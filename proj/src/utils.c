#include <lcom/lcf.h>
#include <stdlib.h>

#include "include/utils.h"

/* INTERRUPT (UN)SUBSCRIBE */
int
subscribe_int(int* bit_no, int irq_line, bool is_exclusive)
{
  /*
   * Subscribes irq_line interrupts to a given bit_no.
   */

  /* check which polivy to use */
  int policy = IRQ_REENABLE;
  if (is_exclusive)
    policy |= IRQ_EXCLUSIVE;

  /* check if input for 'sys_irqsetpolicy' was given */
  if (!bit_no || irq_line < 0)
    return 1;

  /* subscribes keyboard to 'bit_no' */
  return (sys_irqsetpolicy(irq_line, policy, bit_no));
}

int
unsubscribe_int(int* hook_id)
{
  return (sys_irqrmpolicy(hook_id));
}

/* UTILITIES */
uint8_t
get_LSB(uint16_t val)
{
  /*
   * Returns the LSB of a 2 byte integer.
   */

  /* Least Significant Bits Mask */
  static uint8_t const LSB_BITS =
    (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
  return val & LSB_BITS;
}

uint8_t
get_MSB(uint16_t val)
{
  /*
   * Returns the MSB of a 2 byte integer.
   */

  /* Most Significant Bits Mask */
  static uint16_t const MSB_BITS = (BIT(15) | BIT(14) | BIT(13) | BIT(12) |
                                    BIT(11) | BIT(10) | BIT(9) | BIT(8));
  return (val & MSB_BITS) >> 8;
}

int
util_sys_inb(int port, uint8_t* value)
{
  /*
   * Invokes sys_inb() system call but reads the value into a uint8_t variable.
   */

  /* check for NULL/uninitialized pointer */
  if (!value)
    return 1;

  /* We save the value in our 'byte' variable */
  uint32_t byte;
  if (sys_inb(port, &byte))
    return 1;

  /* Cast 'byte' to uint8_t ('value' type) */
  *value = (uint8_t)byte;

  return 0;
}

int16_t
ninebit2sixteen(bool msb_bit, uint8_t lsb_bit)
{
  int16_t ret = lsb_bit;
  if (msb_bit) {
    static int16_t const MSB_BITS = (BIT(15) | BIT(14) | BIT(13) | BIT(12) |
                                     BIT(11) | BIT(10) | BIT(9) | BIT(8));
    ret |= MSB_BITS;
  }

  return ret;
}

float
scalar_prodf(const float* const vec1, const float* const vec2, uint32_t n)
{
  float result = 0;
  for (size_t i = 0; i < n; ++i)
    result += (vec1[i] * vec2[i]);

  return result;
}

int
scalar_prodi(const float* const vec1, const char* const vec2, uint32_t n)
{
  int result = 0;
  for (size_t i = 0; i < n; ++i)
    result += (int)(vec1[i] * vec2[i]);

  return result;
}
