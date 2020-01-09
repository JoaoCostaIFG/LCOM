/** @file bmp.h */
#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>

#include "vg.h"

/** @addtogroup	sprite_grp
 * @{
 */

#define BMP_SIGN 0x4D42 /**< @brief BM - little endian bitmap file signature */

#pragma pack(1)
/** @struct BMPFILEHEADER_T
 *  struct for the BMP file format headers
 */
typedef struct BMPFILEHEADER_T
{
  uint16_t Signature;
  uint32_t FileSize;
  uint16_t Reserved[2];
  uint32_t PixelArrayOff;
} BMPFileHeader_t;

/** @struct BMPV5HEADER_T
 *  struct for the BMP image information headers
 */
typedef struct BMPV5HEADER_T
{
  uint32_t DIBHeaderSize;
  uint32_t Width;
  int Height; /* height can be negative (representing the order in which the
                 pixel array is represented) */
  uint16_t Planes;
  uint16_t BitsPerPixel;
  uint32_t Compression;
  uint32_t ImageSize;
  uint32_t XPixPerMeter;
  uint32_t YPixPerMeter;
  uint32_t ColorTableSize;
  uint32_t ImportantColorCount;
  uint32_t RedChannelMask;
  uint32_t GreenChannelMask;
  uint32_t BlueChannelMask;
  uint32_t AlphaChannelMask;
  uint32_t ColorSpaceType;
  uint32_t ColorSpaceEndpoints;
  uint32_t GammaRedChannel;
  uint32_t GammaBlueChannel;
  uint32_t GammaGreenChannel;
  uint32_t Intent;
  uint32_t ICCProfileData;
  uint32_t ICCProfileSize;
  uint32_t Reserved;
} BMPV5Header_t;
#pragma options align = reset

/**
 * @brief	Reads a BMP file, specified by its path and saves its
 *information in a given Sprite_t struct.
 *
 * @param file_name	Path (and name) of the BMP file to read the information
 *from.
 * @param sprite	Struct to save the read information to.
 *
 * @return	0, on success,\n
 *		1, otherwise.
 */
int new_sprite_bmp(const char* const file_name, Sprite_t* sprite);

/**
 * @brief Shear a sprite in the the horizontal direction by a given amount.
 *
 * @param ori_sprite  Sprite to shear.
 * @param shear       Amount to shear.
 *
 * @return  Sheared sprite, on success\n
 *          NULL, otherwise
 */
Sprite_t* shearX_sprite(Sprite_t* ori_sprite, float shear);

/**
 * @brief Shear a sprite in the the vertical direction by a given amount.
 *
 * @param ori_sprite  Sprite to shear.
 * @param shear       Amount to shear.
 *
 * @return  Sheared sprite, on success\n
 *          NULL, otherwise
 */
Sprite_t* shearY_sprite(Sprite_t* ori_sprite, float shear);

/**
 * @brief Creates a new sprite object containing a copy of a given sprite
 * rotated by a given angle.
 *
 * @param ori_sprite  Original sprite to rotate.
 * @param angle Angle to rotate by (in radians).
 *
 * @return  A sprite object containing the rotated sprite, on success\n
 *          NULL, otherwise.
 */
Sprite_t* rotate_sprite(Sprite_t* ori_sprite, float angle);

/**
 * @brief Creates a new sprite object containing a copy of a given sprite
 * rotated by a given number of 90 degree turns.
 *
 * @param ori_sprite  Original sprite to rotate.
 * @param num_turns Number of 90 degree turns to do.
 *
 * @return  A sprite object containing the rotated sprite, on success\n
 *          NULL, otherwise.
 */
Sprite_t* rotate_sprite_intPI(Sprite_t* ori_sprite, int8_t num_turns);

/**
 * @brief Copy a given sprite.
 *
 * @param orig  Sprite to copy.
 *
 * @return  A copy of the given sprite, on success\n
 *          NULL, otherwise.
 */
Sprite_t* sprite_cpy(Sprite_t* orig);

/** @} */

#endif // __BMP_H__
