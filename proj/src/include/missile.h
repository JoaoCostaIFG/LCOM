/** @file missile.h */
#ifndef __MISSILE_H__
#define __MISSILE_H__

#include <stdlib.h>

#include "object.h"

/** @addtogroup object_grp
 * @{
 */

/** @struct MISSLE_T
 *  Struct that manages the state of a missle.
 */
typedef struct MISSLE_T
{
  const Object_Vtable_t* vtable; /**< Virtual table of the object 'class'. */
  Object_t* obj;                 /**< Pointer to the Object_t of the missle. */
  uint16_t damage;               /**< Damage of the missle. */
  uint8_t my_ska;                /**< Id of the skane that shot the missle. */
} Missle_t;

/**
 * @brief   Creates a new Missle_t.
 *
 * @param speed_x Horizontal speed of the missle.
 * @param speed_y Vertical speed of the missle.
 * @param x       Starting horizontal position of the missle.
 * @param y       Starting vertical position of the missle.
 * @param sprite  Sprite of the missle.
 * @param damage  Damage of the shot.
 * @param my_ska  Id of the skane that shot the missle.
 *
 * @return  A pointer to the new created Missle_t.
 */
Missle_t* new_missle(float speed_x,
                     float speed_y,
                     float x,
                     float y,
                     Sprite_t* sprite,
                     uint16_t damage,
                     uint8_t my_ska);

/**@}*/

#endif // __MISSILE_H__
