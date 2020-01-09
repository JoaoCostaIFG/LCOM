#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stdint.h>

/** @defgroup util_grp	Utils */

/** @addtogroup	util_grp
 *
 * @brief	Commonly/general functions, inline functions and macros used all
 * over the project.
 *
 * @{
 */

/** @brief converts seconds to microseconds. */
#define SEC2MICRO(x) (x * 1000000)

/** @brief converts decimal to BCD (8 bit numbers only). */
#define DEC2BCD(dec) (((dec / 10) << 4) | (dec % 10))

/** @brief converts BCD to decimal (8 bit numbers only). */
#define BCD2DEC(BCD) (((BCD >> 4) * 10) + (BCD & 0xF))

/* INTERRUPT (UN)SUBSCRIBE */
/**
 * @brief	Subscribes interrupts for a given irq_line.
 *
 * @param bit_no	Input of the chosen bit in the interrupt array for this
 *interrupt.\n Output of the hook_id given back by the kernel.
 * @param irq_line	Irq_line to subscribe the interrupts from.
 * @param is_exclusive	Indicates whether the interrupts should be subscribed
 *exclusively
 *
 * @return	0, on success,\n
 *		      1, otherwise.
 */
int subscribe_int(int* bit_no, int irq_line, bool is_exclusive);

/**
 * @brief	Unsubscribes interrupts for a given hook_id.
 *
 * @param hook_id	Hook_id (given by the kernel) that refers to the
 *interrupts we want to unsub.
 *
 * @return	0, on success,\n
 *		      1, otherwise.
 */
int unsubscribe_int(int* hook_id);

/* UTILITIES */
/**
 * @brief	Get the least significant byte of a 2 byte integer.
 *
 * @param val	Integer to get the LSB from.
 *
 * @return	Byte containing the LSB of the given value.
 */
uint8_t get_LSB(uint16_t val);

/**
 * @brief	Get the most significant byte of a 2 byte integer.
 *
 * @param val	Integer to get the MSB from.
 *
 * @return	Byte containing the MSB of the given value.
 */
uint8_t get_MSB(uint16_t val);

/**
 * @brief	Get a input from a given port.
 *
 * @param port	Port to get input from.
 * @param value	Byte read (casted from uint32_t).
 *
 * @return	0, on success\n
 *		1, otherwise.
 */
int util_sys_inb(int port, uint8_t* value);

/**
 * @brief	Joins a 9 bits signed integer into a 16 bit signed integer.
 *
 * @param msb_bit	Most significant bit of the 9 bit integer.
 * @param lsb_bit	Least significant byte of the 9 bit integer.
 *
 * @return	The joined 16 bit signed integer.
 */
int16_t ninebit2sixteen(bool msb_bit, uint8_t lsb_bit);

float scalar_prodf(const float* const vec1,
                   const float* const vec2,
                   uint32_t n);

int scalar_prodi(const float* const vec1, const char* const vec2, uint32_t n);

/* INLINE FUNCTIONS */
/**
 * @brief	Calculates the slope defined by a x variation and a y variation.
 *
 * @param x	Variation in x of the slope.
 * @param y	Variation in y of the slope.
 *
 * @return	0 if the given x is 0,\n
 *		the value of the calculated slope, otherwise
 */
inline float
slope_calc(const int x, const int y)
{
  return x != 0 ? (float)y / x : 0;
}

/* don't feel like including <math.h> just for this */
/**
 * @brief	Calculates the absolute value of a double/float.
 *
 * @param x	Double/float to get the abs of.
 *
 * @return	The abs of x.
 */
inline double
fmabs(const double x)
{
  return x < 0 ? -x : x;
}

/**
 * @brief	Skip the given number of bytes in a file stream.
 * @note	Stops if EOF is reached.
 *
 * @param fp		File stream to be skipped.
 * @param num_bytes	Number of bytes to skip.
 */
inline void
fskip(FILE* fp, uint32_t num_bytes)
{
  for (size_t i = 0; i < num_bytes && !feof(fp); ++i)
    fgetc(fp);
}

/* skip a file stream until the byte, search_byte, is found or EOF is reached */
/**
 * @brief	Skips bytes in a file stream until a given byte is found
 * (inclusive).
 * @note	Stops if EOF is reached.
 *
 * @param fp		File stream to be skipped.
 * @param search_byte	Byte we're searching for.
 */
inline void
fskip_until(FILE* fp, char search_byte)
{
  while (!feof(fp))
    if (fgetc(fp) == search_byte)
      break;
}

/** @} */

#endif /* __UTILS_H__ */
