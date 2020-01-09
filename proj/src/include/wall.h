/** @file wall.h */
#ifndef __WALL_H__
#define __WALL_H__

#include <stdlib.h>

#include "object.h"

/** @addtogroup object_grp
 * @{
 */

/** @enum WALL_TYPE
 *  Type of the wall (defines the collision directions)
 */
typedef enum WALL_TYPE {
  VERT_WALL,  /**< Vertical wall */
  HORIZ_WALL, /**< Horizontal wall */
  RECT_WALL   /**< Rectangular wall */
} wall_type;

/** @struct WALL_T
 *  Wall object
 */
typedef struct WALL_T
{
  /** @name Members used for internal identification and object handling. */
  /*@{*/
  const Object_Vtable_t* vtable; /**< Wall's vtable. */
  Object_t* obj;                 /**< Wall's base object. */
  /*@}*/

  /** @name Other members. */
  /*@{*/
  uint8_t length; /**< Wall's length. */
  uint8_t height; /**< Wall's height. */
  wall_type type; /**< Wall's type. */
  /*@}*/
} Wall_t;

/**
 * @brief Creates a new Wall_t object.
 *
 * @param x       X coordinate of the new wall.
 * @param y       Y coordinate of the new wall.
 * @param length  Length of the new wall.
 * @param height  Width of the new wall.
 * @param sprite  Sprite of the new wall.
 * @param type    Type of the new wall.
 *
 * @return  NULL, on wall instanciation failure,\n
 *          pointer to the instanciatiated wall, otherwise.
 */
Wall_t* new_wall(float x,
                 float y,
                 uint8_t length,
                 uint8_t height,
                 Sprite_t* sprite,
                 wall_type type);

/**@}*/

#endif // __WALL_H__
