#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "object.h"

/** @addtogroup object_grp
 * @{
 */

/** @struct CURSOR_T
 *  Struct that manages the state of a cursor.
 */
typedef struct CURSOR_T
{
  /** @name  base object inherited members */
  /*@{*/
  /** Virtual table of the cursor 'class'. */
  const Object_Vtable_t* vtable;
  /** Pointer to the Object_t of the cursor. */
  Object_t* obj;
  /*@}*/

  /** @name  parameters */
  /*@{*/
  /** Offset until the center of the cursor on screen in X. */
  float coff_x;
  /** Offset until the center of the cursor on screen in Y. */
  float coff_y;
  /*@}*/
} Cursor_t;

/**
 * @brief	Creates a new cursor
 *
 * @param sprite	Sprite that the new cursor will have
 *
 * @return	Returns a pointer to the newly made cursor
 */
Cursor_t* new_cursor(Sprite_t* sprite);

/**
 * @brief	Updates the position of a given cursor and destroys the previous
 * sprite of it
 *
 * @param delta_x	Horizontal offset to add
 * @param delta_y	Vertical offset to add
 * @param cursor	Pointer to Cursor_t to be updated
 */
void updateCursor(Cursor_t* cursor, int16_t delta_x, int16_t delta_y);

/**
 * @brief Get current X position of the center of the cursor (in pixels).
 *
 * @param cursor  Cursor to get the position from.
 *
 * @return A float representing the X position of the cursor on screen.
 */
float get_center_x(Cursor_t* cursor);

/**
 * @brief Get current Y position of the center of the cursor (in pixels).
 *
 * @param cursor  Cursor to get the position from.
 *
 * @return A float representing the Y position of the cursor on screen.
 */
float get_center_y(Cursor_t* cursor);

/**@}*/

#endif // __CURSOR_H__
