/** @file menu.h */
#ifndef __MENU_H__
#define __MENU_H__

#include "object.h"

/** @addtogroup object_grp
 * @{
 */

#define MENU_TITLE_ID  0 /**< Menu title screen ID */
#define MENU_SINGLE_ID 1 /**< Singleplayer mode button id */
#define MENU_MULTIP_ID 2 /**< Multiplayer mode button id */
#define MENU_EXIT_ID   3 /**< Quit button id */
#define MENU_LOAD_ID   4 /**< Load button id */

#define MENU_SINGLE_X_POS 100 /**< Singleplayer mode button X pos */
#define MENU_SINGLE_Y_POS 100 /**< Singleplayer mode button Y pos */
#define MENU_MULTIP_X_POS 100 /**< Multiplayer mode button X pos */
#define MENU_MULTIP_Y_POS 0   /**< Multiplayer mdoe button Y pos */
#define MENU_EXIT_X_POS   100 /**< Quit burron X pos */
#define MENU_EXIT_Y_POS   100 /**< Quit button Y pos */
#define MENU_TITLE_SCR_X  0   /**< Title X pos */
#define MENU_TITLE_SCR_Y  100 /**< Title Y pos */

/** @struct MENU_T
 * Struct that manages the state of a instanciated food.
 */
typedef struct MENU_T
{
  const Object_Vtable_t* vtable; /**< Virtual table of the object 'class'. */
  Object_t* obj;                 /**< Menu's base object. */
} Menu_t;

/**
 * @brief Creates a new Menu_t.
 *
 * @param x       Starting horizontal position of the menu.
 * @param y       Starting vertical position of the menu.
 * @param sprite  Sprite of the menu.
 * @param ID		  Identifier of the menu.
 *
 * @return  A pointer to the new created Menu_t object.
 */
Menu_t* new_menu(float x, float y, Sprite_t* sprite, size_t ID);

/**@}*/

#endif // __MENU_H__
