/** @file enemies.h */
#ifndef __ENEMIES_H__
#define __ENEMIES_H__

#include <stdint.h>

#include "object.h"
#include "skane.h"

/** @addtogroup object_grp
 * @{
 */

/** @struct ENEMY_T
 *  Struct that represents an enemy
 */
typedef struct ENEMY_T
{
  /** @name Members used for internal identification and object handling. */
  /*@{*/
  const Object_Vtable_t* vtable; /**< Enemy's vtable */
  Object_t* obj;                 /**< Enemy's base object */
  /*@}*/

  /** @name Animation related members. */
  /*@{*/
  bool collided_ene; /**< Set if the enemy collided with an ally this frame */
  bool is_attacking; /**< Set if in the middle of an attack */
  unsigned curr_attack;  /**< How long until the attack ends (in frames) */
  unsigned attack_delay; /**< Enemy's attack animation length */
  /*@}*/

  /** @name Other enemy members. */
  /*@{*/
  uint8_t health;      /**< Enemy's current health */
  float speed;         /**< Enemy's base movement speed */
  uint8_t damage;      /**< Enemy's attack damage */
  uint8_t nourishment; /**< Enemy's food drop nourishment */
  Skane_t* ska;        /**< This enemy's target skane */
  /*@}*/

} Enemy_t;

/**
 * @brief Creates a new enemy
 *
 * @param x	            X coordinate of the new enemy
 * @param y             Y coordinate of the new enemy
 * @param speed		      Speed of the new enemy
 * @param damage	      Damage of the new enemy
 * @param health        Enemy health
 * @param nourishment   The amount of nourishment of the food produced by this
 * enemy
 * @param attack_delay  Delay between attacks (in frames)
 * @param ska		        Skane that the enemy will follow and attack
 *
 * @return  Pointer the created enemy, on success,\n
 *          NULL, otherwise.
 */
Enemy_t* new_enemy(float x,
                   float y,
                   float speed,
                   uint8_t damage,
                   uint8_t health,
                   uint8_t nourishment,
                   unsigned attack_delay,
                   Skane_t* ska);

/**
 * @brief   Inflicts a given damage to a given enemy.
 * @warning Destroys enemy if enemy health drops bellow 0.
 *
 * @param enemy   Enemy that took damage.
 * @param damage  How much damage the enemy took.
 */
void enemy_take_damage(Enemy_t* enemy, uint8_t damage);

/**@}*/

#endif // __ENEMIES_H__
