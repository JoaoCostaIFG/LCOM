/** @file ev_disp.h */
#ifndef __EV_DISP_H__
#define __EV_DISP_H__

#include <stdint.h>

/** @addtogroup game_grp
 * @{
 */

/* GAME FUNCTIONS */
/** @brief	Initializes interrupts and video mode */
void init(void);

/** @brief	Unsubscribes interrupts and resets to text mode. Exits
 * afterwards. */
void quit(void);

/**
 * @brief Goes to main menu screen independant of the game state
 */
void exit_to_main_menu(void);

/* OBJECT FUNCTIONS */

/* CONFIG FUNCTIONS */
/**
 * @brief Puts the game in multiplayer mode.
 * @note Default is no multiplayer.
 * @warning Must be set before calling the game init function.
 */
void set_multiplayer(void);

/**
 * @brief Sets a game resources folder path.
 *
 * @note Default is "home/lcom/labs/proj/src/resources/".
 * @warning Must be set before calling the game init function.
 *
 * @param path  Path to set.
 */
void set_resourcepath(const char* const path);

/**
 * @brief Set a video mode (default 0x107)
 *
 * @note Default is 0x107.
 * @warning Must be set before calling the game init function.
 *
 * @param mode  Super-VGA video mode to set.
 */
void set_videomode(const uint16_t mode);

/**@}*/

#endif // __EV_DISP_H__
