/** @file obj_handle.h */
#ifndef __OBJ_HANDLE_H__
#define __OBJ_HANDLE_H__

#include <stdint.h>

#include "game_opts.h"
#include "menu.h"
#include "missile.h"

/** @addtogroup game_grp
 * @{
 */

/** Render all objects, in the objects matrix, on screen. */
void render_objects(void);

/** Calculate new positions of all objects, in the objects matrix. */
void calc_objs_pos(void);

/** Empty out the collision matrix. */
void clear_collision_matrix(void);

/** Update the collision matrix for all the objects. */
void update_objs_collisions(void);

/** Debug function that draws the collision matrix on screen. */
void debug_collisions(void);

/**
 * @brief Destroy objects that have 0 as id (marked for clean-up).
 * @return  0, in case no Skane object was destroyed\n
 *          1, in case at least one Skane Object was destroyed.
 */
int garbage_collector(void);

/** Destroys all the objects in the object matrix. */
void destroy_all_objects(void);

/** Allocate the object matrix. */
void alloc_obj_matrix(void);

/** Allocate the collision matrix. */
void alloc_collison_matrix(void);

/**
 * @brief Adds a given object the object vector.
 *
 * @param object_to_be_added  Pointer to the object to be added-
 * @param layer               Layer to add the object to.
 */
int add_object(void* object_to_be_added, int layer);

/**
 * @brief	Removes an object with a given id and type from the game objects
 * matrix
 *
 * @param object_id		Id of the object to delete
 * @param object_type	Type of the object to delete
 */
void remove_object(int object_id, int object_type);

/** Frees and dereferences menu pointers if defined. */
void delete_menus(void);

/** Renders the main menu buttons. */
void draw_menus(void);

/** Renders the loading screen menu. */
void draw_loading_menu(void);

/* INSTANTIATION */
/**
 * @brief Instantiate Skanes.
 * @param gamest  The current game state.
 */
void inst_skane(gamestate gamest);

/**
 * @brief Spawn a group of enemies.
 * @param gamest  The current game state.
 */
void spawn_enemy(gamestate gamest);

/**
 * @brief Spawn a group of allies.
 * @param grp_size  Number of allies to spawn.
 * @param gamest    The current game state.
 */
void spawn_allies(uint8_t grp_size, gamestate gamest);

/**
 * @brief Spawn map walls and enemy spawners.
 * @param gamest  The current game state.
 */
void create_map(gamestate gamest);

/** Instantiate cursor. */
void inst_cursor(void);

/** Instantiate menus. */
void inst_menus(void);

/* GETTERS/SETTERS */
/**
 * @brief Set player's Skane state (given the current game input).
 * @param input_array Game input.
 */
void ska1_mov(input_array_t input_array);

/**
 * @brief Set opponents' Skane state.
 * @param new_state State to set.
 */
void set_ska2_state(int8_t new_state);

/**
 * @brief Attempt to shoot a missle (player's Skane).
 * @return  0, on success\n
 *          1, on failure.
 */
int ska1_fire_missle(void);

/**
 * @brief Shoot a missle from second player's Skane to a given point.
 *
 * @param x X coordinate to shoot at.
 * @param y Y coordinate to shoot at.
 *
 * @return  0, on success\n
 *          1, on failure.
 */
int ska2_fire_missle(float x, float y);

/**
 * @brief   Get current cursor X coordinate.
 * @return  The cursor X coordinate.
 */
float get_cursor_x(void);

/**
 * @brief   Get current cursor Y coordinate.
 * @return  The cursor Y coordinate.
 */
float get_cursor_y(void);

/**
 * @brief   Get current cursor's center X coordinate.
 * @return  The cursor's center X coordinate.
 */
float get_cursor_center_x(void);

/**
 * @brief   Get current cursor's center Y coordinate.
 * @return  The cursor's center Y coordinate.
 */
float get_cursor_center_y(void);

/** Draw current cursor on screen */
void draw_cursor(void);

/**
 * @brief Move cursor by given amount.
 * @note  Negative values move in the opposite direction.
 *
 * @param x Amount of pixels to move in the horizontal
 * @param y Amount of pixels to move in the vertical
 */
void update_cursor(int16_t x, int16_t y);

/**
 * @brief Get the x coordinate of a given menu
 * @param menu	Pointer to Menu_t to the given menu
 * @return  The x coordinate of the given menu
 */
float get_menu_x(Menu_t* menu);

/**
 * @brief Get the y coordinate of a given menu
 * @param menu	Pointer to Menu_t to the given menu
 * @return	The y coordinate of the given menu
 */
float get_menu_y(Menu_t* menu);

/**
 * @brief Gets the title menu
 * @return Pointer to the title menu
 */
Menu_t* get_title_menu(void);

/**
 * @brief Gets the exit menu
 * @return Pointer to the exit menu
 */
Menu_t* get_exit_menu(void);

/**
 * @brief Gets the singleplayer menu
 * @return Pointer to the singleplayer menu
 */
Menu_t* get_sing_menu(void);

/**
 * @brief Gets the multiplayer menu
 * @return Pointer to the multiplayer menu
 */
Menu_t* get_mult_menu(void);

/* SYNC */
/**
 * @brief   Transmit over serial the new Skane information.
 * @note    Transmits missle shots and state changes.
 * @return  1, in case no information was sent (no new info)\n
 *          0, otherwise.
 */
int transmit_skane_info(void);

/**@}*/

#endif // __OBJ_HANDLE_H__
