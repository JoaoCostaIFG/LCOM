#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/bmp.h"
#include "include/err_utils.h"
#include "include/utils.h"

static inline float
applymtr(const float* const mtr_row, const float* const point)
{
  return (mtr_row[0] * point[0] + mtr_row[1] * point[1]);
}

static int
load_bmp(FILE* fp, Sprite_t* sprite)
{
  /* read BMP file header */
  BMPFileHeader_t file_header;
  fread((char*)&file_header, sizeof(BMPFileHeader_t), 1, fp);
  if (file_header.Signature != BMP_SIGN) {
    warn("%s: the file isn't a supported BMP format", __func__);
    return 1;
  }

  /* read BMP image header */
  BMPV5Header_t header;
  fread((char*)&header, sizeof(BMPV5Header_t), 1, fp); // TODO
  if (header.BitsPerPixel != 8) {
    // anything different from 8 bit indexed mode will be undifined behaviour
    warn("%s: the file isn't in a 8 bit indexed mode enconding.", __func__);
  }
  if (header.Compression) {
    warn("%s: can't parse compressed BMP files.", __func__);
    return 1;
  }

  /* skip to pixel array (ignore color table) */
  fseek(fp, file_header.PixelArrayOff, SEEK_SET);

  /* read pixel array */
  uint8_t bytespixel = (header.BitsPerPixel + 7) / 8; // pixel size in bytes
  // size of a row of pixel data in bytes (possibly with padding)
  uint32_t row_size = (header.BitsPerPixel * header.Width + 31) / 32 * 4;
  // padding size in byte (if any)
  uint8_t padding_size = row_size - (header.BitsPerPixel * header.Width) / 8;

  /* allocate memory for data array */
  sprite->Data = (uint8_t*)malloc(sizeof(char) * header.Width *
                                  abs(header.Height) * bytespixel);
  if (sprite->Data == NULL) {
    warn("%s: BMP pixel array memory allocation failed: %d bytes.",
         __func__,
         header.Height * abs(header.Height) * bytespixel);
    return 1;
  }
  else {
    /* save sprite width and height */
    sprite->Width  = header.Width;
    sprite->Height = abs(header.Height);
  }

  /* store pixel array (skipping padding, if any) */
  if (header.Height >= 0) {
    uint8_t* sprite_ptr = sprite->Data + sprite->Height * sprite->Width;
    for (size_t i = 0; i < sprite->Height; ++i) {
      for (size_t x = sprite->Width; x > 0; --x) {
        *(sprite_ptr - x) = (uint8_t)fgetc(fp);
      }

      sprite_ptr -= sprite->Width;
      fskip(fp, padding_size); // skip padding
    }
  }
  else {
    /* negative height means "reverse order": start from top to bottom */
    uint8_t* sprite_ptr = sprite->Data;
    for (size_t i = 0; i < sprite->Height; ++i) {
      for (size_t x = 0; x < sprite->Width; ++x) {
        *(sprite_ptr++) = (uint8_t)fgetc(fp);
      }

      fskip(fp, padding_size); // skip padding
    }
  }

  return 0;
}

int
new_sprite_bmp(const char* const file_name, Sprite_t* sprite)
{
  /* Read a BMP from a given file path */

  /* open the file */
  FILE* fp = fopen(file_name, "rb");
  if (!fp) {
    // warn("%s: Couldn't open the BMP file: %s.", __func__, file_name);
    return 1;
  }

  /* load the bmp */
  if (load_bmp(fp, sprite)) {
    warn("%s:", __func__);
    fclose(fp);
    return 1;
  }

  /* close the file */
  fclose(fp);

  return 0;
}

Sprite_t*
shearX_sprite(Sprite_t* ori_sprite, float shear)
{
  Sprite_t* shear_sprite = (Sprite_t*)malloc(sizeof(Sprite_t));
  if (!shear_sprite) {
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* size of the rotated sprite is bigger */
  float size =
    sqrt(ceil(pow(ori_sprite->Height, 2)) + ceil(pow(ori_sprite->Width, 2))) *
    get_bytespixel();

  /* alloc space for the new, bigger, sprite data */
  uint8_t* rot_data = (uint8_t*)malloc(sizeof(uint8_t) * size * size);
  if (!rot_data) {
    free(shear_sprite);
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }
  shear_sprite->Width  = size;
  shear_sprite->Height = size;
  shear_sprite->Data   = rot_data;

  /* rotation matrix (rotation by shearing) */
  float rot_mtr[2][2] = { { 1, shear }, { 0, 1 } };
  float trans_point[2];

  /* variables used for temporary calculations */
  float old_x, old_y;
  float rot_trans = shear_sprite->Width / 2.0;
  float ori_trans = ori_sprite->Width / 2.0;

  for (int x = shear_sprite->Width - 1; x >= 0; --x) {
    for (int y = shear_sprite->Height - 1; y >= 0; --y) {
      trans_point[0] = x - rot_trans;
      trans_point[1] = y - rot_trans;

      /* apply transformation and translation */
      old_x = applymtr(rot_mtr[0], trans_point) + ori_trans;
      old_y = applymtr(rot_mtr[1], trans_point) + ori_trans;

      if (old_x >= 0 && old_y >= 0 && old_x < ori_sprite->Width &&
          old_y < ori_sprite->Height)
        shear_sprite->Data[x + y * shear_sprite->Width] =
          ori_sprite->Data[(int)(old_x) + (int)(old_y)*ori_sprite->Width];
      else // transparent pixel
        shear_sprite->Data[x + y * shear_sprite->Width] = 1;
    }
  }

  return shear_sprite;
}

Sprite_t*
shearY_sprite(Sprite_t* ori_sprite, float shear)
{
  Sprite_t* shear_sprite = (Sprite_t*)malloc(sizeof(Sprite_t));
  if (!shear_sprite) {
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* size of the rotated sprite is bigger */
  float size =
    sqrt(ceil(pow(ori_sprite->Height, 2)) + ceil(pow(ori_sprite->Width, 2))) *
    get_bytespixel();

  /* alloc space for the new, bigger, sprite data */
  uint8_t* rot_data = (uint8_t*)malloc(sizeof(uint8_t) * size * size);
  if (!rot_data) {
    free(shear_sprite);
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }
  shear_sprite->Width  = size;
  shear_sprite->Height = size;
  shear_sprite->Data   = rot_data;

  /* rotation matrix (rotation by shearing) */
  float rot_mtr[2][2] = { { 1, 0 }, { shear, 1 } };
  float trans_point[2];

  /* variables used for temporary calculations */
  float old_x, old_y;
  float rot_trans = shear_sprite->Width / 2.0;
  float ori_trans = ori_sprite->Width / 2.0;

  for (int x = shear_sprite->Width - 1; x >= 0; --x) {
    for (int y = shear_sprite->Height - 1; y >= 0; --y) {
      trans_point[0] = x - rot_trans;
      trans_point[1] = y - rot_trans;

      /* apply transformation and translation */
      old_x = applymtr(rot_mtr[0], trans_point) + ori_trans;
      old_y = applymtr(rot_mtr[1], trans_point) + ori_trans;

      if (old_x >= 0 && old_y >= 0 && old_x < ori_sprite->Width &&
          old_y < ori_sprite->Height)
        shear_sprite->Data[x + y * shear_sprite->Width] =
          ori_sprite->Data[(int)(old_x) + (int)(old_y)*ori_sprite->Width];
      else // transparent pixel
        shear_sprite->Data[x + y * shear_sprite->Width] = 1;
    }
  }

  return shear_sprite;
}

Sprite_t*
rotate_sprite(Sprite_t* ori_sprite, float angle)
{
  Sprite_t* rot_sprite = (Sprite_t*)malloc(sizeof(Sprite_t));
  if (!rot_sprite) {
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* size of the rotated sprite is bigger */
  float size =
    sqrt(ceil(pow(ori_sprite->Height, 2)) + ceil(pow(ori_sprite->Width, 2))) *
    get_bytespixel();

  /* alloc space for the new, bigger, sprite data */
  uint8_t* rot_data = (uint8_t*)malloc(sizeof(uint8_t) * size * size);
  if (!rot_data) {
    free(rot_sprite);
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }
  rot_sprite->Width  = size;
  rot_sprite->Height = size;
  rot_sprite->Data   = rot_data;

  /* rotation matrix (rotation by shearing) */
  /* float al = -tan(angle / 2), bt = sin(angle); */
  /* float albt          = al * bt; */
  /* float rot_mtr[2][2] = { { 1 + albt, al * (2 + albt) }, { bt, 1 + albt } };
   */

  /* normal rotation */
  float rot_mtr[2][2] = { { cos(angle), -sin(angle) },
                          { sin(angle), cos(angle) } };
  float trans_point[2];

  /* variables used for temporary calculations */
  float old_x, old_y;
  float rot_trans = rot_sprite->Width / 2.0;
  float ori_trans = ori_sprite->Width / 2.0;

  for (int x = rot_sprite->Width - 1; x >= 0; --x) {
    for (int y = rot_sprite->Height - 1; y >= 0; --y) {
      trans_point[0] = x - rot_trans;
      trans_point[1] = y - rot_trans;

      /* apply transformation and translation */
      old_x = applymtr(rot_mtr[0], trans_point) + ori_trans;
      old_y = applymtr(rot_mtr[1], trans_point) + ori_trans;

      if (old_x >= 0 && old_y >= 0 && old_x < ori_sprite->Width &&
          old_y < ori_sprite->Height)
        rot_sprite->Data[x + y * rot_sprite->Width] =
          ori_sprite->Data[(int)(old_x) + (int)(old_y)*ori_sprite->Width];
      else // transparent pixel
        rot_sprite->Data[x + y * rot_sprite->Width] = 1;
    }
  }

  return rot_sprite;
}

Sprite_t*
rotate_sprite_intPI(Sprite_t* ori_sprite, int8_t num_turns)
{
  Sprite_t* rot_sprite = (Sprite_t*)malloc(sizeof(Sprite_t));
  if (!rot_sprite) {
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* alloc space for the new, bigger, sprite data */
  uint8_t* rot_data =
    (uint8_t*)malloc(sizeof(uint8_t) * ori_sprite->Width * ori_sprite->Height);
  if (!rot_data) {
    free(rot_sprite);
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }
  rot_sprite->Width  = ori_sprite->Width;
  rot_sprite->Height = ori_sprite->Height;

  if (!num_turns) {
    *rot_data        = *ori_sprite->Data;
    rot_sprite->Data = rot_data;
    return rot_sprite;
  }
  rot_sprite->Data = rot_data;

  /* rotation matrix */
  float angle = M_PI_2 * num_turns;
  float co = cos(angle), si = sin(angle);
  float rot_mtr[2][2] = { { co, -si }, { si, co } };
  float trans_point[2];

  /* variables used for temporary calculations */
  int old_x, old_y;
  float rot_trans = rot_sprite->Width / 2.0;
  float ori_trans = ori_sprite->Width / 2.0;

  for (int x = rot_sprite->Width - 1; x >= 0; --x) {
    for (int y = rot_sprite->Height - 1; y >= 0; --y) {
      trans_point[0] = x - rot_trans;
      trans_point[1] = y - rot_trans;

      /* apply transformation and translation */
      old_x = round(applymtr(rot_mtr[0], trans_point) + ori_trans);
      old_y = round(applymtr(rot_mtr[1], trans_point) + ori_trans);

      if (old_x >= 0 && old_y >= 0 && old_x < (int)ori_sprite->Width &&
          old_y < (int)ori_sprite->Height)
        rot_sprite->Data[x + y * rot_sprite->Width] =
          ori_sprite->Data[(int)(old_x) + (int)(old_y)*ori_sprite->Width];
      else // transparent pixel
        rot_sprite->Data[x + y * rot_sprite->Width] = 1;
    }
  }

  return rot_sprite;
}

Sprite_t*
sprite_cpy(Sprite_t* orig)
{
  Sprite_t* cpy_sprite = (Sprite_t*)malloc(sizeof(Sprite_t));
  if (!cpy_sprite) {
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* alloc space for the new sprite Data */
  cpy_sprite->Data =
    (uint8_t*)malloc(sizeof(uint8_t) * orig->Width * orig->Height);
  if (!cpy_sprite->Data) {
    free(cpy_sprite);
    warn("%s: Bad Alloc", __func__);
    return NULL;
  }

  /* copy data */
  memcpy(
    cpy_sprite->Data, orig->Data, sizeof(uint8_t) * orig->Width * orig->Height);

  cpy_sprite->Width  = orig->Width;
  cpy_sprite->Height = orig->Height;

  return cpy_sprite;
}
