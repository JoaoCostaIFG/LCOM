#include <stdio.h>
#include <stdlib.h>

#include "include/err_utils.h"
#include "include/missile.h"
#include "include/object.h"

/* VIRTUAL FUNCTIONS */
static void
printMissle(void* mis)
{
  Missle_t* missle = (Missle_t*)mis;
  missle->obj->vtable->print(missle->obj);
  warn("Damage : %u", missle->damage);
}

static void
updateMisslePos(void* mis)
{
  Missle_t* m = (Missle_t*)mis;

  m->obj->vtable->updatePos(m->obj);
}

static void
renderMissle(void* mis)
{
  Missle_t* m = (Missle_t*)mis;
  m->obj->vtable->draw(m->obj);
}

static void
destroyMissle(void* mis)
{
  Missle_t* m = (Missle_t*)mis;
  m->obj->vtable->destroy(m->obj);
  free(m);
}

static void
updateCollisionMissle(void* mis, vector* col_matrix)
{
  Missle_t* m = (Missle_t*)mis;
  m->obj->vtable->updateCollision(m, col_matrix);
}

const static Object_Vtable_t missle_vtable = { .draw      = renderMissle,
                                               .print     = printMissle,
                                               .updatePos = updateMisslePos,
                                               .destroy   = destroyMissle,
                                               .updateCollision =
                                                 updateCollisionMissle };

/* PUBLIC */
Missle_t*
new_missle(float speed_x,
           float speed_y,
           float x,
           float y,
           Sprite_t* sprite,
           uint16_t damage,
           uint8_t my_ska)
{
  Missle_t* missle = malloc(sizeof(Missle_t));
  if (!missle)
    return NULL;

  Object_t* obj = new_object(speed_x, speed_y, x, y, sprite);
  if (!obj) {
    free(missle);
    return NULL;
  }
  missle->obj = obj;

  missle->vtable = &missle_vtable;

  missle->damage = damage;
  missle->my_ska = my_ska;

  static size_t curr_id        = 1;
  missle->obj->identifier.id   = curr_id;
  missle->obj->identifier.type = MISSILE;
  ++curr_id;

  return missle;
}
