/** @file game_opts.h */
#ifndef __GAME_OPTS_H__
#define __GAME_OPTS_H__

#include <string.h>

/** @defgroup game_grp Game state/config */

/** @addtogroup game_grp
 *
 * @brief  Code and constants related to the game configurations and its state
 * handling.
 *
 * @{
 */

/* game */

/** @enum gamestate_t
 *  Defines the current game state */
typedef enum gamestate_t {
  SINGLE, /**< Singleplayer */
  MULT1,  /**< Multiplayer (player 1) */
  MULT2,  /**< Multiplayer (player 2) */
  MENUST  /**< In a menu */
} gamestate;

/* RESOURCES PATH */
#define PATH_MAXSIZE 256 /**< Max size of file paths in the program */
/** Default resources folder path */
#define DFLT_RESPATH "/home/lcom/labs/proj/src/resources/"
/** Current resources path in the game */
extern char respath[PATH_MAXSIZE];

/**
 * @brief   Get full path to given file from current resources path.
 * @param   file  File to get the path off.
 * @return  Full path to the file.
 */
inline static char*
make_path(const char* const file)
{
  static char tempstr[PATH_MAXSIZE];

  strcpy(tempstr, respath);
  strcat(tempstr, file);

  return tempstr;
}

/** @brief Set to something between [0, 256] (the higher, the safer) */
#define YOUGOTEPILEPSY  1
#define DFLT_VIDEO_MODE 0x107 /**< @brief Default video mode */
/** @brief Default color palette location */
#define DFLT_PALLETE_FILE "/color_palette"
#define DFLT_BKG          0 /**< @brief Default background color */
#define DFLT_TRANSP       1 /**< @brief Default transparency color */

/* menus */
/** @brief  Singleplayer mode button sprite. */
#define MENU_START_SINGLE_PATH "/singleplayer_button.bmp"
/** @brief  Multiplayer mode button sprite. */
#define MENU_START_MULTIP_PATH "/multiplayer_button.bmp"
/** @brief  Quit button sprite. */
#define MENU_EXIT_PATH "/quit_button.bmp"
/** @brief  Title sprite. */
#define MENU_TITLE_PATH "/title_screen.bmp"
/** @brief  Loading sprite. */
#define MENU_LOAD_PATH "/login_screen.bmp"

/* cursor */
#define CURSORPATH "/cursor.bmp" /**< @brief Cursor sprite file */

/* walls */
#define WALLSEGMENT "/seg.bmp"    /**< @brief Wall segment sprite file */
#define WALLCORNER  "/corner.bmp" /**< @brief Wall corner sprite file */

/* food */
#define FOODPATH "/food.bmp" /**< @brief Food sprite file */

/* animations */
#define SKA_H_ANIMCYCLE   1 /**< @brief Number of skane head sprites */
#define SKA_B_ANIMCYCLE   1 /**< @brief Number of skane body sprites */
#define SKA_T_ANIMCYCLE   1 /**< @brief Number of skane tail sprites */
#define MIS_ANIMCYCLE     1 /**< @brief Number of skane missle sprites */
#define FOOD_ANIMCYCLE    1 /**< @brief Number of skane food sprite */
#define ENE_ANIMCYCLE     5 /**< @brief Number of skane enemy sprite */
#define ENE_ANIMCYCLE_T   3 /**< @brief Frames between enemy sprite changes */
#define ENE_ATK_ANIMCYCLE 2 /**< @brief Number of enemy attack sprites */
/** @brief Frames between enemy attack sprite changes */
#define ENE_ATK_ANIMCYCLE_T 10

/* skane 1 sprites */
/** @brief Skane 1 head sprite */
#define SKA1_HEADPATH "/skane1/skane1Head.bmp"
/** @brief Skane 1 body sprite */
#define SKA1_BODYPATH "/skane1/skane1Body.bmp"
/** @brief Skane 1 tail sprite */
#define SKA1_TAILPATH "/skane1/skane1Tail.bmp"
/** @brief Skane 1 missle sprite */
#define SKA1_MISPATH "/skane1/skane1Missle.bmp"
/** @brief Skane 1 enemy sprite 1 */
#define SKA1_ENEPATH "/skane1/skane1Ene.bmp"
/** @brief Skane 1 enemy sprite 2 */
#define SKA1_ENEPATH_2 "/skane1/skane1Ene_2.bmp"
/** @brief Skane 1 enemy sprite 3 */
#define SKA1_ENEPATH_3 "/skane1/skane1Ene_3.bmp"
/** @brief Skane 1 enemy sprite 4 */
#define SKA1_ENEPATH_4 "/skane1/skane1Ene_4.bmp"
/** @brief Skane 1 attack sprite */
#define SKA1_ENEPATH_ATK "/skane1/skane1Ene_Atk.bmp"
/** @brief Skane 1 spawner sprite */
#define SKA1_SPAWNER "/skane1/skane1Spawner.bmp"

/* skane 2 sprites */
/** @brief Skane 2 head sprite */
#define SKA2_HEADPATH "/skane2/skane2Head.bmp"
/** @brief Skane 2 body sprite */
#define SKA2_BODYPATH "/skane2/skane2Body.bmp"
/** @brief Skane 2 tail sprite */
#define SKA2_TAILPATH "/skane2/skane2Tail.bmp"
/** @brief Skane 2 missle sprite */
#define SKA2_MISPATH "/skane2/skane2Missle.bmp"
/** @brief Skane 2 enemy sprite 1 */
#define SKA2_ENEPATH "/skane2/skane2Ene.bmp"
/** @brief Skane 2 enemy sprite 2 */
#define SKA2_ENEPATH_2 "/skane2/skane2Ene_2.bmp"
/** @brief Skane 2 enemy sprite 3 */
#define SKA2_ENEPATH_3 "/skane2/skane2Ene_3.bmp"
/** @brief Skane 2 enemy sprite 4 */
#define SKA2_ENEPATH_4 "/skane2/skane2Ene_4.bmp"
/** @brief Skane 2 attack sprite */
#define SKA2_ENEPATH_ATK "/skane2/skane2Ene_Atk.bmp"
/** @brief Skane 2 spawner sprite */
#define SKA2_SPAWNER "/skane2/skane2Spawner.bmp"

/* skane defaults */
#define SKA_X             100 /**< @brief Skane default start X location */
#define SKA_Y             50  /**< @brief Skane default start Y location */
#define SKA_X_2           100 /**< @brief Skane default start X location (2nd) */
#define SKA_Y_2           250 /**< @brief Skane default start Y location (2nd) */
#define SKA_S             5   /**< @brief Skane default start speed location */
#define SKA_HP            30  /**< @brief Skane default start health location */
#define SKA_SHOOTING_COST 1   /**< @brief Skane default shooting cost */
#define SKA1_SPAWN_DIREC  N   /**< @brief Skane 1 start spawn direction */
#define SKA2_SPAWN_DIREC  S   /**< @brief Skane 2 start spawn direction */

/* missle defaults */
#define MIS_SPEED 10 /**< @brief Missile speed. */
#define MIS_DMG   10 /**< @brief Missile damage. */
#define MIS_CD    30 /**< @brief Shotting cooldown (in frames). */

/* enemy defaults */
#define ENE_X            60 /**< @brief Enemy start X. */
#define ENE_Y            60 /**< @brief Enemy start Y. */
#define ENE_S            3  /**< @brief Enemy default speed. */
#define ENE_DMG          5  /**< @brief Enemy default damage. */
#define ENE_HP           30 /**< @brief Enemy default health. */
#define ENE_NOURISH      5  /**< @brief Enemy default food nourishment. */
#define ENE_ATKDELAY     30 /**< @brief Enemy default attack delay. */
#define ENEMY_SPAWN_RATE 10 /**< @brief Enemy spawn rate. */
#define ENE_MAX_GRPSIZE  3  /**< @brief Enemy spawn max group size. */
#define ENE_MIN_GRPSIZE  1  /**< @brief Enemy spawn min group size. */

/* difficulty scaling */
/** @brief Enemies' group size increase. */
#define ENE_GRP_SCALE 0.5f
/** @brief Enemies' group size limit (skane lvl up at this limit) */
#define ENE_GRP_LIM     (2 * ENE_MAX_GRPSIZE)
#define ENE_SPEED_SCALE 0.1f /**< @brief Speed enemies gain upon level up. */
#define MIS_UP_SCALE    1    /**< @brief Increases missle projectile speed. */
#define MIS_SPEED_SCALE 5    /**< @brief Lowers fire cooldown. */

/* death text */
#define DEATHTXT                                                               \
  "Look at them, they come to this place when they know they are not pure. "   \
  "Tenno use the keys, but they are mere trespassers. Only I, Vor, know the "  \
  "true power of the Void. I was cut in half, destroyed, but  through its "    \
  "Janus Key, the Void called to me. It brought me here and here I was "       \
  "reborn. We cannot blame these creatures, they are being led by a false "    \
  "prophet, an impostor who knows not the secrets of the Void. Behold the "    \
  "Tenno, come to scavenge and desecrate this sacred realm. My brothers, did " \
  "I not tell of this day? Did I not prophesize this moment? Now, I will "     \
  "stop them. Now I am changed, reborn through the energy of the Janus Key. "  \
  "Forever bound to the Void. Let it be known, if the Tenno want true "        \
  "salvation, they will lay down their arms, and wait for the baptism of my "  \
  "Janus key. It is time. I will teach these trespassers the redemptive "      \
  "power of my Janus key. They will learn its simple truth. The Tenno are "    \
  "lost, and they will resist. But I, Vor, will cleanse this place of their "  \
  "impurity."

/* uart packet's type */
#define SERIAL_SKA_MOV      0x01  /**< Skane movement packet type. */
#define SERIAL_SKA_MOV_S    1     /**< Skane movement packet size. */
#define SERIAL_SKA_MIS      0x02  /**< Skane missle packet type. */
#define SERIAL_SKA_MIS_S    2 * 4 /**< Skane missle packet size. */
#define SERIAL_ENE_SPA      0x03  /**< Enemy spawn packet type. */
#define SERIAL_ENE_SPA_S    1     /**< Empty spawn packet size. */
#define SERIAL_DEATH_PACK   0x0D  /**< Death packet. */
#define SERIAL_DEATH_PACK_S 0     /**< packet size. */
#define SERIAL_EMPTY_PACK   0x0E  /**< Empty packet. */
#define SERIAL_SYNC_PACK    0x0F  /**< End frame packet. */
#define SERIAL_SYNC_PACK_S  0     /**< End frame packet size. */
#define SERIAL_SYNC_PACK_HT 0x10  /**< End frame packet bit. */

/** How many sync tries per frame */
#define SERIAL_SYNC_TRIES 300000
/** How much delay (in microseconds) per try (not being used) */
#define SERIAL_SYNC_DELAY 2

/**@}*/

#endif //__GAME_OPTS_H__
