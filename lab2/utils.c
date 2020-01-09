#include <lcom/lcf.h>

#include <stdint.h>


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
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


int(util_get_MSB)(uint16_t val, uint8_t *msb) {
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


int (util_sys_inb)(int port, uint8_t *value) {
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
