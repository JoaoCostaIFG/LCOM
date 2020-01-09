#include <lcom/lcf.h>

#include "i8042.h"
#include "utils.h"


int
(util_get_LSB)(uint16_t val, uint8_t *lsb) {
	/*
	 * Returns the LSB of a 2 byte integer.
	 */

	/* check for NULL/uninitialized pointer */
	if (!lsb)
		return 1;

	/* Least Significant Bits Mask */
	uint8_t const LSB_BITS = (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
	*lsb = val & LSB_BITS;

	return 0;
}


int
(util_get_MSB)(uint16_t val, uint8_t *msb) {
	/*
	 * Returns the MSB of a 2 byte integer.
	 */

	/* check for NULL/uninitialized pointer */
	if (!msb)
		return 1;

	/* Most Significant Bits Mask */
	uint16_t const MSB_BITS = (BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8));
	val &= MSB_BITS;
	*msb = val >> 8;

	return 0;
}


int
(util_sys_inb)(int port, uint8_t *value) {
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
	*value = (uint8_t) byte;

	return 0;
}


int
subscribe_int(int *bit_no, int irq_line, bool is_exclusive)
{
	/*
	 * Subscribes irq_line interrupts to a given bit_no.
	 */

	/* check which polivy to use */
	int policy = IRQ_REENABLE;
	if (is_exclusive)
		policy |= IRQ_EXCLUSIVE;

	/* check if input for 'sys_irqsetpolicy' was given */
	if (!bit_no ||
    	    irq_line < 0)
		return 1;

	/* subscribes keyboard to 'bit_no' */
	if (sys_irqsetpolicy(irq_line, policy, bit_no))
		return 1;

	return 0;
}


int
unsubscribe_int(int *hook_id)
{
	if (sys_irqrmpolicy(hook_id))
		return 1;

	return 0;
}


int16_t
ninebit2sixteen(bool msb_bit, uint8_t lsb_bit)
{
	int16_t ret = lsb_bit;
	if (msb_bit) {
		int16_t const MSB_BITS = (BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8));
		ret |= MSB_BITS;
	}

	return ret;
}


float
slope_calc(int x, int y)
{
	if (x != 0)
		return (float) y / x;
	else
		return 0;
}


float
fabs(float x)
{
	return x < 0 ? -x : x;
}