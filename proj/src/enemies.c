#include <math.h>

#include "include/enemies.h"
#include "include/err_utils.h"
#include "include/obj_handle.h"
#include "include/food.h"

/* VIRTUAL FUNCTIONS */
static void
printEnemy(void* enem)
{
  Enemy_t* enemy = (Enemy_t*)enem;
  enemy->obj->vtable->print(enemy->obj);
  warn("HEALTH: %u DAMAGE: %u SKANE_ID: %d",
       enemy->health,
       enemy->damage,
       enemy->ska->obj->identifier.id);
}

static void
updateEnemyPos(void* enem)
{
  Enemy_t* enemy = (Enemy_t*)enem;

  /* get movement direction */
  if (!enemy->is_attacking) {
    float del_x = enemy->ska->obj->x + enemy->ska->obj->sprite.Width / 2 -
                  (enemy->obj->x + enemy->obj->sprite.Width / 2);
    float del_y = enemy->ska->obj->y + enemy->ska->obj->sprite.Height / 2 -
                  (enemy->obj->y + enemy->obj->sprite.Height / 2);

    if (del_x || del_y) {
      float norm          = sqrt(del_x * del_x + del_y * del_y);
      enemy->obj->speed_x = (float)del_x / norm * enemy->speed;
      enemy->obj->speed_y = (float)del_y / norm * enemy->speed;

      enemy->obj->vtable->updatePos(enemy->obj);
    }
  }

  if (enemy->is_attacking)
    --enemy->curr_attack;
  if (enemy->curr_attack == 0)
    enemy->is_attacking = false;
}

static void
renderEnemy(void* enem)
{
  Enemy_t* e = (Enemy_t*)enem;

  /* cycle animation */
  if (e->is_attacking) {
    e->obj->anim_cnt = 0;

    if (e->curr_attack > ENE_ATK_ANIMCYCLE_T) // start attacking
      e->obj->sprite = e->ska->ska_sprt.ene_sprite[4];
    else // finish attack animation
      e->obj->sprite = e->ska->ska_sprt.ene_sprite[0];
  }
  else {
    switch (e->obj->anim_cnt) {
      case 0:
        e->obj->sprite = e->ska->ska_sprt.ene_sprite[0];
        break;
      case ENE_ANIMCYCLE_T:
      case 5 * ENE_ANIMCYCLE_T:
        e->obj->sprite = e->ska->ska_sprt.ene_sprite[1];
        break;
      case 2 * ENE_ANIMCYCLE_T:
      case 4 * ENE_ANIMCYCLE_T:
        e->obj->sprite = e->ska->ska_sprt.ene_sprite[2];
        break;
      case 3 * ENE_ANIMCYCLE_T:
        e->obj->sprite = e->ska->ska_sprt.ene_sprite[3];
        break;
      default:
        break;
    }

    /* reset anim_cnt */
    ++e->obj->anim_cnt;
    if (e->obj->anim_cnt == 6 * ENE_ANIMCYCLE_T)
      e->obj->anim_cnt = 0;
  }

  e->obj->vtable->draw(e->obj);
}

static void
destroyEnemy(void* enem)
{
  Enemy_t* e = (Enemy_t*)enem;
  e->obj->vtable->destroy(e->obj);
  free(e);
}

static void
updateCollisionEnemy(void* enem, vector* col_matrix)
{
  Enemy_t* e      = (Enemy_t*)enem;
  e->collided_ene = false; // reset collision with allies state

  e->obj->vtable->updateCollision(e, col_matrix);
}

const static Object_Vtable_t enemy_vtable = { .draw      = renderEnemy,
                                              .print     = printEnemy,
                                              .updatePos = updateEnemyPos,
                                              .destroy   = destroyEnemy,
                                              .updateCollision =
                                                updateCollisionEnemy };

/* PUBLIC FUNCTIONS */
Enemy_t*
new_enemy(float x,
          float y,
          float speed,
          uint8_t damage,
          uint8_t health,
          uint8_t nourishment,
          unsigned attack_delay,
          Skane_t* ska)
{
  static size_t id = 1;
  Enemy_t* enemy   = (Enemy_t*)malloc(sizeof(Enemy_t));
  if (!enemy)
    return NULL;

  Object_t* obj = new_object(0, 0, x, y, &ska->ska_sprt.ene_sprite[0]);
  if (!obj) {
    free(enemy);
    return NULL;
  }

  /* enemy basic stats */
  enemy->health       = health;
  enemy->damage       = damage;
  enemy->nourishment  = nourishment;
  enemy->ska          = ska; // target skane (enemy)

  enemy->collided_ene = true; // let them attempt to move out of their group

  /* make enemies start on attack animation */
  enemy->speed        = speed;
  enemy->attack_delay = attack_delay;
  enemy->is_attacking = true;
  enemy->curr_attack  = enemy->attack_delay;

  /* identification */
  obj->identifier.type = ENEMY;
  obj->identifier.id   = id;
  ++id;
  enemy->obj    = obj;
  enemy->vtable = &enemy_vtable;
  return enemy;
}

void
enemy_take_damage(Enemy_t* enemy, uint8_t damage)
{
  if (damage >= enemy->health) {
    Food_t* f = new_food(enemy->obj->x,
                         enemy->obj->y,
                         enemy->nourishment,
                         &enemy->ska->ska_sprt.f_sprite);
    if (f)
      add_object(f, FOOD);
    else
      warn("Could not spawn food from this enemy");

    enemy->obj->identifier.id = 0;
    return;
  }

  enemy->health -= damage;
}
