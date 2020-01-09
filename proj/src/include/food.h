/** @file food.h */
#ifndef __FOOD_H__
#define __FOOD_H__

#include <stdlib.h>

#include "object.h"

/** @addtogroup object_grp
 * @{
 */

/** @struct FOOD_T
 * Struct that manages the state of an instantiated food object.
 */
typedef struct FOOD_T
{
  const Object_Vtable_t* vtable; /**< Virtual table of the object 'class'. */
  Object_t* obj;                 /**< Food's base object. */
  /** Health that the food, when consumed, will replenish. */
  u_int16_t nourishment;
} Food_t;

/**
 * @brief Creates a new Food_t.
 *
 * @param x           Starting horizontal position of the food.
 * @param y           Starting vertical position of the food.
 * @param nourishment Health of the food.
 * @param sprite      Sprite of the food.
 *
 * @return  A pointer to the new created Food object.
 */
Food_t* new_food(float x, float y, uint16_t nourishment, Sprite_t* sprite);

/**@}*/

#endif // __FOOD_H__
