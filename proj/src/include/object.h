/** @file object.h */
#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include <stdbool.h>
#include <stdlib.h>

#include "game_opts.h"
#include "vector.h"
#include "vg.h"

/** @defgroup object_grp Game objects group */

/** @addtogroup object_grp
 *
 * @brief  Code and constants related to the game objects.
 *
 * @{
 */

/** @brief  Scancode hashing function */
#define INPUT_HASH(bytes) ((bytes[0] & MAKECODE) + bytes[1])

/** @enum INP
 *  Keys/mouse inputs recognized by the game.
 */
typedef enum INP {
  lmb       = 511,         /**< Left mouse button. */
  rmb       = 512,         /**< Right mouse button. */
  mmb       = 513,         /**< Middle mouse button. */
  w         = 0x11,        /**< W key. */
  a         = 0x1E,        /**< A key. */
  s         = 0x1F,        /**< S key. */
  d         = 0x20,        /**< D key. */
  ESC       = 0x01,        /**< ESC key. */
  up_arr    = 0xE0 + 0x48, /**< Up arrow key. */
  left_arr  = 0xE0 + 0x4B, /**< Left arrow key. */
  right_arr = 0xE0 + 0x4D, /**< Right arrow key. */
  down_arr  = 0xE0 + 0x50, /**< Down arrow key. */
} input;

/** @brief  Array that can store all the currently pressed keyboard keys. */
typedef bool input_array_t[514];

/* CLASS DEFINITION */
/** @enum OBJ_TYPE
 *  Types that an object can take on the game
 */
typedef enum OBJ_TYPE {
  WALL       = 0, /**< Wall type */
  SKANE      = 1, /**< Skane type */
  FOOD       = 2, /**< Food type */
  ENEMY      = 3, /**< Enemy type */
  MISSILE    = 4, /**< Missle type */
  SKANE_BODY = 5, /**< Skane body type */
  CURSOR     = 6, /**< Cursor type */
  NOT_SET    = 7, /**< @brief Undefined type (should be the highest tag set) */
  MENU       = 8  /**< @brief Menu type always (above NOT_SET)*/
} obj_type;

/** @struct OBJECT_IDENTIFIER_T
 *  Type used to identify and tag objects.
 */
typedef struct OBJECT_IDENTIFIER_T
{
  uint8_t id;    /**< ID of the object. */
  obj_type type; /**< Type of the object. */
} Object_Identifier_t;

/** @struct OBJECT_VTABLE_T
 *  C++ like virtual table implementation (for polymorphism)
 *  @note	All classes derived from object must have a vtable as the first
 *  member of their struct (for casting purposes))
 */
typedef struct OBJECT_VTABLE_T
{
  void (*draw)(void*);  /**< Draws the object on screen. */
  void (*print)(void*); /**< Prints all the information of a given object. */
  void (*updatePos)(void*); /**< Updates the position of a given object. */
  void (*destroy)(void*);   /**< Destructor of the object. */
  /** Update collision matrix of the object. */
  void (*updateCollision)(void*, vector*);
} Object_Vtable_t;

/** @struct OBJECT_T
 *  Struct that manages the state of an object.
 */
typedef struct OBJECT_T
{
  /** @name Members used for internal identification and object handling. */
  /*@{*/
  const Object_Vtable_t* vtable;  /**< Virtual table of the object 'class'. */
  Object_Identifier_t identifier; /**< Type of the object. */
  /*@}*/

  /** @name Position related parameters. */
  /*@{*/
  float speed_x; /**< Horizontal velocity of the object. */
  float speed_y; /**< Vertical velocity of the object. */
  float x;       /**< Current horizontal position of the object. */
  float y;       /**< Current vertical position of the object. */
  /*@}*/

  /** @name Sprite related parameters. */
  /*@{*/
  size_t anim_cnt;  /**< Current pos of the object's animation cycle. */
  int transparency; /**< Transparency of the object. */
  Sprite_t sprite;  /**< Sprite of the object. */
  /*@}*/
} Object_t;

/** @struct DERIVED_OBJ_T
 *  Derived object type.
 */
typedef struct DERIVED_OBJ_T
{
  Object_Vtable_t* vtable; /**< Derived object vtable */
  Object_t* obj;           /**< Derived object's base object */
} Derived_obj_t;

/* OBJECT FUNCTIONS */
/**
 * @brief       Creates a new Object_t.
 * @note        Normally this function should only be used by derived 'classes'
 * of object (An Object is considered an abstract class).
 *
 * @param speed_x       Horizontal speed of the object.
 * @param speed_y       Vertical speed of the object.
 * @param x             Starting horizontal position of the object.
 * @param y             Starting vertical position of the object.
 * @param sprite        Sprite of the object.
 *
 * @return      A pointer to the new created Object_t.
 */
Object_t* new_object(float speed_x,
                     float speed_y,
                     float x,
                     float y,
                     Sprite_t* sprite);

/**
 * @brief	Updates the object of an area delimited by a given x, y, height
 *and width (basically a rectangle) of a given collision matrix.
 * @note Any type of collision due to object overlab is handled by calling the
 *collision_dispatcher.
 *
 * @param obj         Object to be refered to in the selected points of the
 *matrix
 * @param col_matrix  The collision matrix to update
 * @param x				    Starting X coordinate of the
 *rectangle to update
 * @param y				    Starting Y coordinate of the
 *rectangle point to update
 * @param height			Height of the rectangle
 * @param width				Width of the rectangle
 * @param already_collided_objs Pointer to the vector of collided objects.
 */
void updateCollisionMatrixRect(void* obj,
                               vector* col_matrix,
                               uint16_t x,
                               uint16_t y,
                               uint16_t width,
                               uint16_t height,
                               vector* already_collided_objs);

/**
 * @brief	Updates the object of an area delimited by a given x, y, height
 * and width (basically a rectangle) of a given collision matrix.
 * Ignores all pixels that are transparent in the given sprite.
 * @note Any type of collision due to object overlab is handled by calling the
 * collision_dispatcher.
 *
 * @param obj         Object to be refered to in the selected points of the
 *matrix
 * @param col_matrix  The collision matrix to update
 * @param x				    Starting X coordinate of the
 *rectangle to update
 * @param y				    Starting Y coordinate of the
 *rectangle point to update
 * @param spr         Sprite of the object to work with (get width and height)
 * @param already_collided_objs Pointer to the vector of collided objects.
 */
void updateCollisionMatrix(void* obj,
                           vector* col_matrix,
                           uint16_t x,
                           uint16_t y,
                           Sprite_t* spr,
                           vector* already_collided_objs);

/* VIRTUAL FUNCTIONS */

/** @brief      Wrapper that calls the function that prints all of the
 * information of a given *Object_t*. */
void print(void*);

/** @brief      Wrapper that calls the function that renders an *Object_t*
 * object. */
void draw(void*);

/** @brief      Wrapper that calls the function that updates the position of an
 * *Object_t* object. */
void updatePos(void* obj);

void destroy(void* obj);

void updateCollisions(void* obj, vector* col_matrix);

/**@}*/

#endif // __OBJECTS_H__
