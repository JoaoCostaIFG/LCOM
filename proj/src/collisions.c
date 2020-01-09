#include "include/cursor.h"
#include "include/enemies.h"
#include "include/err_utils.h"
#include "include/food.h"
#include "include/missile.h"
#include "include/skane.h"
#include "include/wall.h"

static void
missile_and_wall_collision(Missle_t* missile, Wall_t* wall)
{
  missile->obj->identifier.id = 0;
  // remove_object(missile->obj->identifier.id, missile->obj->identifier.type);
}

static void
missle_and_enemy_collision(Missle_t* missile, Enemy_t* enemy)
{
  /* check if missle isn't shooting allies */
  if (missile->my_ska == enemy->ska->obj->identifier.id) {
    missile->obj->identifier.id = 0;
    enemy_take_damage(enemy, missile->damage);
  }
}

static void
missle_and_skabody_collision(Missle_t* missile, Skane_Body_t* ska_body)
{
  /* check if missle isn't shooting own skane */
  Skane_t* ska = (Skane_t*)ska_body->ska;
  if (ska->obj->identifier.id != missile->my_ska) {
    /* destroy missle (set for garbage collection) */
    missile->obj->identifier.id = 0;

    /* Enemy hits skane */
    skane_take_damage(ska_body->ska, missile->damage / 2);
  }
}

static void
enemy_and_wall_collision(Enemy_t* enemy, Wall_t* wall)
{
  if (wall->type == VERT_WALL) {
    if (enemy->obj->x > wall->obj->x) { // wall is at the left
      if (enemy->obj->x + enemy->obj->speed_x <
          wall->obj->x + wall->obj->sprite.Width)
        enemy->obj->x -= enemy->obj->speed_x;
    }
    else if (enemy->obj->x + enemy->obj->sprite.Width > wall->obj->x) {
      if (enemy->obj->x + enemy->obj->speed_x + enemy->obj->sprite.Width >
          wall->obj->x)
        enemy->obj->x -= enemy->obj->speed_x;
    }
  }
  else if (wall->type == HORIZ_WALL) {
    if (enemy->obj->y > wall->obj->y) { // wall is up
      if (enemy->obj->y + enemy->obj->speed_y <
          wall->obj->y + wall->obj->sprite.Height)
        enemy->obj->y -= enemy->obj->speed_y;
    }
    else if (enemy->obj->y + enemy->obj->sprite.Height > wall->obj->y) {
      if (enemy->obj->y + enemy->obj->speed_y + enemy->obj->sprite.Height >
          wall->obj->y)
        enemy->obj->y -= enemy->obj->speed_y;
    }
  }
}

static void
skane_and_enemy_collision(Skane_t* skane, Enemy_t* enemy)
{
  if (!enemy->is_attacking &&
      enemy->ska->obj->identifier.id == skane->obj->identifier.id) {
    enemy->is_attacking = true;
    enemy->curr_attack  = enemy->attack_delay;

    /* Enemy hits skane */
    skane_take_damage(skane, enemy->damage);
  }
}

static void
skane_and_missle_collision(Skane_t* skane, Missle_t* missle)
{
  /* check if missle isn't shooting own skane */
  if (skane->obj->identifier.id != missle->my_ska) {
    /* destroy missle (set for garbage collection) */
    missle->obj->identifier.id = 0;

    /* Missle hits skane */
    skane_take_damage(skane, missle->damage);
  }
}

static void
skane_and_food_collision(Skane_t* skane, Food_t* food)
{
  skane_nom(skane, food->nourishment);
  food->obj->identifier.id = 0; // Tag to be destroyed
}

static void
skane_and_wall_collision(Skane_t* skane, Wall_t* wall)
{
  if (wall->type == VERT_WALL) {
    if (skane->obj->x > wall->obj->x) { // if at left
      if (skane->curr_state == W || skane->curr_state == NW ||
          skane->curr_state == SW)
        skane->collision_direc += W;
    }
    else if (skane->obj->x + skane->cell_size > wall->obj->x) {
      if (skane->curr_state == E || skane->curr_state == NE ||
          skane->curr_state == SE)
        skane->collision_direc += E;
    }
  }
  else if (wall->type == HORIZ_WALL) {
    if (skane->obj->y > wall->obj->y) { // if at right
      if (skane->curr_state == N || skane->curr_state == NW ||
          skane->curr_state == NE)
        skane->collision_direc += N;
    }
    else if (skane->obj->y + skane->cell_size > wall->obj->y) {
      if (skane->curr_state == S || skane->curr_state == SW ||
          skane->curr_state == SE)
        skane->collision_direc += S;
    }
  }
}

static void
skane_and_skane_collision(Skane_t* ska1, Skane_t* ska2)
{
  if (!ska1->has_col_skane && !ska2->has_col_skane) {
    if (ska1->curr_state != STOP)
      skane_take_damage(ska1, ska2->damage);
    if (ska2->curr_state != STOP)
      skane_take_damage(ska2, ska1->damage);
  }

  ska1->has_col_skane = 2;
  ska2->has_col_skane = 2;
}

static void
skane_and_skabody_collision(Skane_t* ska, Skane_Body_t* skabody)
{
  /* If skane which collided is smaller, kill it */
  if (ska->curr_state != STOP && !ska->has_col_skane &&
      ska->health < ((Skane_t*)skabody->ska)->health) {
    ska->obj->identifier.id = 0;
  }
  /* else { // TODO */
    /* ska->has_col_skane                      = 2; */
    /* ((Skane_t*)skabody->ska)->has_col_skane = 2; */
  /* } */
}

static void
enemy_and_enemy_collision(Enemy_t* ene1, Enemy_t* ene2)
{
  /* can only collide with allies */
  if (ene1->ska->obj->identifier.id == ene2->ska->obj->identifier.id &&
      !ene1->collided_ene && !ene2->collided_ene) {
    ene1->obj->x -= ene1->obj->speed_x;
    ene1->obj->y -= ene1->obj->speed_y;

    ene1->collided_ene = true;
    ene2->collided_ene = true;
  }
}

void
collision_dispatcher(void* obj1, void* obj2)
{
  if (obj1 == NULL || obj2 == NULL)
    return; // One of the objects was already deleted, no collision will occur

  if (!((Derived_obj_t*)obj1)->obj->identifier.id ||
      !((Derived_obj_t*)obj2)->obj->identifier.id)
    return; // object doesn't collide if it will be garbage collected

  obj_type obj_t1 = ((Derived_obj_t*)obj1)->obj->identifier.type;
  obj_type obj_t2 = ((Derived_obj_t*)obj2)->obj->identifier.type;
  switch (obj_t1) {
    case MISSILE:
      switch (obj_t2) {
        case WALL:
          missile_and_wall_collision(obj1, obj2);
          break;
        case ENEMY:
          missle_and_enemy_collision(obj1, obj2);
          break;
        case SKANE_BODY:
          missle_and_skabody_collision(obj1, obj2);
          break;
        case SKANE:
          skane_and_missle_collision(obj2, obj1);
          break;
        default:
          break;
      }
      break;
    case SKANE:
      switch (obj_t2) {
        case WALL:
          skane_and_wall_collision(obj1, obj2);
          break;
        case ENEMY:
          skane_and_enemy_collision(obj1, obj2);
          break;
        case FOOD:
          skane_and_food_collision(obj1, obj2);
          break;
        case MISSILE:
          skane_and_missle_collision(obj1, obj2);
          break;
        case SKANE:
          skane_and_skane_collision(obj1, obj2);
          break;
        case SKANE_BODY:
          skane_and_skabody_collision(obj1, obj2);
        default:
          break;
      }
      break;
    case WALL:
      switch (obj_t2) {
        case MISSILE:
          missile_and_wall_collision(obj2, obj1);
          break;
        case ENEMY:
          enemy_and_wall_collision(obj2, obj1);
          break;
        default:
          break;
      }
      break;
    case ENEMY:
      switch (obj_t2) {
        case MISSILE:
          missle_and_enemy_collision(obj2, obj1);
          break;
        case SKANE:
          skane_and_enemy_collision(obj2, obj1);
          break;
        case WALL:
          enemy_and_wall_collision(obj1, obj2);
          break;
        case ENEMY:
          enemy_and_enemy_collision(obj1, obj2);
          break;
        default:
          break;
      }
      break;
    case FOOD:
      switch (obj_t2) {
        case SKANE:
          skane_and_food_collision(obj2, obj1);
          break;
        default:
          break;
      }
      break;
    case SKANE_BODY:
      switch (obj_t2) {
        case MISSILE:
          missle_and_skabody_collision(obj2, obj1);
          break;
        case SKANE:
          skane_and_skabody_collision(obj2, obj1);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
