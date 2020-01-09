/** @file skane.h */
#ifndef __SKANE_H__
#define __SKANE_H__

#include <stdint.h>

#include "game_opts.h"
#include "missile.h"
#include "object.h"
#include "vector.h"

/** @addtogroup object_grp
 * @{
 */

#define DFLT_C_SIZE 30

/** @enum SKANE_DIREC
 *  Direction that the skane will move on the next frame (state of the skane).
 */
typedef enum SKANE_DIREC {
  E    = 2,  /**< East direction */
  N    = -3, /**< North direction */
  W    = -2, /**< West direction */
  S    = 3,  /**< South direction */
  NE   = -1, /**< North-East direction */
  NW   = -5, /**< North-West direction */
  SE   = 5,  /**< South-East direction */
  SW   = 1,  /**< South-West direction */
  STOP = 0   /**< Indicates the skane didn't move */
} direc;

/** @struct SKANE_SEGMENT
 *  Segment of a skane.
 */
typedef struct SKANE_SEGMENT
{
  size_t len; /**< Length of the skane's segment. */
  direc dir;  /**< Direction of the skane's segment. */
} seg;

/** @struct SKA_SPRT_T
 *  Group of sprites relating to a skane.
 */
typedef struct SKA_SPRT_T
{
  Sprite_t h_sprite; /**< Skane's head sprite. */
  Sprite_t b_sprite; /**< Skane's body sprite. */
  Sprite_t t_sprite; /**< Skane's tail sprite. */
  Sprite_t m_sprite; /**< Skane's missle sprite. */
  Sprite_t f_sprite; /**< Skane's enemies dropped food sprite. */
  Sprite_t ene_sprite[ENE_ANIMCYCLE]; /**< Skane's enemy sprites. */
} ska_sprt_t;

/** @struct SKANE_BODY_T
 *  Skane body object (for identification in collisions).
 */
typedef struct SKANE_BODY_T
{
  const Object_Vtable_t* vtable; /**< Body's object vtable */
  Object_t* obj;                 /**< Body's base object */
  void* ska;                     /**< Body's skane */
} Skane_Body_t;

/** @struct enemy_diff_t
 *  Skane's enemy current extra stats
 */
typedef struct enemy_diff_t
{
  float gsize;  /**< @brief Enemy group size */
  float es;     /**< @brief Enemy extra speed */
  float shots;  /**< @brief Lower shoot cooldown */
  float shotup; /**< @brief Up projectile speed */
} enemy_diff;

/** @struct SKANE_T
 *  Skane object.
 */
typedef struct SKANE_T
{
  /** @name Members used for internal identification and object handling. */
  /*@{*/
  const Object_Vtable_t* vtable; /**< Skane's object vtable */
  Object_t* obj;                 /**< Skane's base object */
  Skane_Body_t* ska_body;        /**< Skane's body tag */
  /*@}*/

  /** @name Sprite related members. */
  /*@{*/
  ska_sprt_t ska_sprt; /**< Skane's sprites */
  direc draw_direc;    /**< Skane current sprite direction */
  uint8_t cell_size;   /**< Skane's cell size (segment width) */
  /*@}*/

  /** @name Skane state members. */
  /*@{*/
  direc curr_state;      /**< Skane current movement direction */
  direc collision_direc; /**< Skane's collision direction */
  vector* directions;    /**< Skane's body components */
  bool changed_direc;    /**< If set, skane changed direction */
  float t_x, t_y;        /**< Skane's current tail position */
  uint8_t fire_cd;       /**< Skane's current shot cooldown */
  enemy_diff* ediff;     /**< Skane's enemies difficulty scaling */
  /*@}*/

  /** @name Other skane members. */
  /*@{*/
  uint32_t health;           /**< Skane's current health (current length) */
  float s;                   /**< Skane's base movement speed */
  uint16_t damage;           /**< Skane's missle damage */
  float mis_offset;          /**< Skane's missle shooting offset */
  uint16_t offsetx, offsety; /**< Skane head draw offset */
  uint8_t has_col_skane; /**< Controls if skane is colliding with other skane */
  /*@}*/
} Skane_t;

/**
 * @brief Creates a new Skane_t.
 *
 * @param x         Starting horizontal position of the skane.
 * @param y         Starting vertical position of the skane.
 * @param speed     Speed of the skane.
 * @param health    Starting health of the skane.
 * @param damage    Damage of the skane's missle.
 * @param ska_sprt  Struct with all the sprites relating to the skane.
 *
 * @return  A pointer to the new created Skane_t, on success,\n
 *          NULL, otherwise.
 */
Skane_t* new_skane(float x,
                   float y,
                   float speed,
                   uint8_t health,
                   uint16_t damage,
                   ska_sprt_t* ska_sprt);

/**
 * @brief Move skane based on given input.
 *
 * @param ska         Skane to move.
 * @param input_array Input array to get movement from.
 */
void skane_mov(Skane_t* ska, input_array_t input_array);

/**
 * @brief Make skane shoot a missle to the given
 * @note  Skane takes 1 damage when shooting
 *
 * @param ska Skane that will shoot the missle.
 * @param fx  X component of the point that defines the shooting direction.
 * @param fy  Y component of the point that defines the shooting direction.
 *
 * @return  NULL, on missle instanciation failure,\n
 *          pointer to the instanciatiated, otherwise.
 */
Missle_t* fire_missle(Skane_t* ska, float fx, float fy);

/**
 * @brief	Increases the size and health of a given skane by a given
 * amount.
 *
 * @param ska	        Skane that ate something.
 * @param nourishment	Amount of nourishment eaten.
 */
void skane_nom(Skane_t* ska, uint16_t nourishment);

/**
 * @brief Inflicts damage to a given skane.
 * @warning Destroys the game if the skane's health drops bellow 1.
 *
 * @param ska     Skane that took damage.
 * @param damage  Amount of damage taken.
 */
void skane_take_damage(Skane_t* ska, uint8_t damage);

/**
 * @brief Return whether a given Skane is currently able to shoot.
 *
 * @param ska Skane to check.
 *
 * @return  true, if Skane can shoot now\n
 *          false, otherwise.
 */
bool skane_can_shoot(Skane_t* ska);

/**
 * @brief Return whether a given Skane shot this frame.
 *
 * @param ska Skane to check.
 *
 * @return  true, if Skane can shot this frame\n
 *          false, otherwise.
 */
bool skane_just_shot(Skane_t* ska);

/**
 * @brief Scales or resets the current skane difficulty stats.
 *
 * @param ska   Skane to wokr with.
 * @param reset Whether to reset the difficulty or not.
 */
void skane_diff(Skane_t* ska, bool reset);

/**@}*/

#endif // __SKANE_H__
