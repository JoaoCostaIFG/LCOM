#include <stdio.h>
#include <stdlib.h>

#include "include/collisions.h"
#include "include/err_utils.h"
#include "include/object.h"
#include "include/vg.h"

/* OBJECT */

/* PRIVATE */
static void
printObject(void* obj)
{
  Object_t* object = (Object_t*)obj;
  warn("Object_id: %d Object_type: %d : %.2f : %.2f : %.2f : %.2f",
       object->identifier.id,
       object->identifier.type,
       object->speed_x,
       object->speed_y,
       object->x,
       object->y);
}

static void
updateObjectPos(void* obj)
{
  Object_t* o = (Object_t*)obj;

  o->x += o->speed_x;
  o->y += o->speed_y;
}

static void
renderObject(void* obj)
{
  Object_t* o = (Object_t*)obj;
  DRAW_SPRITE(&o->sprite, (uint16_t)o->x, (uint16_t)o->y, o->transparency);
}

static void
destroyObj(void* obj)
{
  Object_t* o = (Object_t*)obj;
  free(o->sprite.Data);
  free(o);
}

/* This one is a bit different from the other virtual methods.
 * The void* obj MUST BE a derived class from object.
 * This is due to the fact that when handling the collision the
 * pointer to the entire derived class must be passed instead of
 * the pointer to the base class */
static void
updateCollisionObj(void* obj, vector* col_matrix)
{
  Derived_obj_t* deriv_obj = (Derived_obj_t*)obj;
  Object_t* base_o         = deriv_obj->obj;

  vector* collided_objs = new_vector();
  updateCollisionMatrix(
    obj, col_matrix, base_o->x, base_o->y, &base_o->sprite, collided_objs);
}

const static Object_Vtable_t object_vtable = { .draw      = renderObject,
                                               .print     = printObject,
                                               .updatePos = updateObjectPos,
                                               .destroy   = destroyObj,
                                               .updateCollision =
                                                 updateCollisionObj };

/* PUBLIC */
Object_t*
new_object(float speed_x, float speed_y, float x, float y, Sprite_t* sprite)
{
  Object_t* obj = (Object_t*)malloc(sizeof(Object_t));
  if (!obj)
    return NULL;

  obj->anim_cnt     = 0;
  obj->speed_x      = speed_x;
  obj->speed_y      = speed_y;
  obj->x            = x;
  obj->y            = y;
  obj->transparency = DFLT_TRANSP;
  if (sprite != NULL)
    obj->sprite = *sprite;

  obj->identifier.id   = 0;
  obj->identifier.type = NOT_SET;
  obj->vtable          = &object_vtable;
  return obj;
}

void
updateCollisionMatrix(void* obj,
                      vector* col_matrix,
                      uint16_t x,
                      uint16_t y,
                      Sprite_t* spr,
                      vector* already_collided_objs)
{
  uint16_t v_size = col_matrix->end;
  uint16_t h_size = ((vector*)(vector_at(col_matrix, 0)))->end;

  if (x > h_size || y > v_size || x < 0 || y < 0)
    return;

  /* Initialize borders */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_size - y) > spr->Height)
    v_lim = spr->Height;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_size - x) > spr->Width)
    h_lim = spr->Width;

  size_t curr_line = y;
  /* Initalize Sprite Vars */
  uint8_t* sprite_ptr    = spr->Data; // get sprite data location
  size_t sprite_ptr_skip = (spr->Width - h_lim) * get_bytespixel();

  for (size_t i = 0; i < v_lim; ++i, ++curr_line) {
    vector* curr_vec  = vector_at(col_matrix, curr_line);
    size_t curr_index = x;

    for (size_t j = 0; j < h_lim; ++j, ++curr_index) {
      void* curr_obj = vector_at(curr_vec, curr_index);
      if (*sprite_ptr != DFLT_TRANSP) {
        if (curr_obj == NULL) // No object on current position
          vector_set(curr_vec, curr_index, obj);
        else // Collision
        {
          if (!vector_contains(already_collided_objs, curr_obj)) {
            collision_dispatcher(obj, curr_obj);
            vector_push_back(already_collided_objs, curr_obj);
          }
        }
      }
      sprite_ptr += get_bytespixel();
    }
    sprite_ptr += sprite_ptr_skip;
  }
}

void
updateCollisionMatrixRect(void* obj,
                          vector* col_matrix,
                          uint16_t x,
                          uint16_t y,
                          uint16_t height,
                          uint16_t width,
                          vector* already_collided_objs)
{
  uint16_t v_size = col_matrix->end;
  uint16_t h_size = ((vector*)(vector_at(col_matrix, 0)))->end;

  if (x > h_size || y > v_size || x < 0 || y < 0)
    return;

  /* Initialize borders */
  size_t v_lim; // part of the image that isn't drawn (is out of screen v_res),
                // if any
  if ((v_lim = v_size - y) > width)
    v_lim = width;

  size_t h_lim; // part of the image that isn't drawn (is out of screen h_res),
                // if any
  if ((h_lim = h_size - x) > height)
    h_lim = height;

  size_t curr_line = y;
  /* Initalize Sprite Vars */

  for (size_t i = 0; i < v_lim; ++i, ++curr_line) {
    vector* curr_vec  = vector_at(col_matrix, curr_line);
    size_t curr_index = x;

    for (size_t j = 0; j < h_lim; ++j, ++curr_index) {
      void* curr_obj = vector_at(curr_vec, curr_index);
      if (curr_obj == NULL) // No object on current position
        vector_set(curr_vec, curr_index, obj);
      else if (curr_obj != obj) // Collision
      {
        if (!vector_contains(already_collided_objs, curr_obj)) {
          collision_dispatcher(obj, curr_obj);
          vector_push_back(already_collided_objs, curr_obj);
        }
      }
    }
  }
}

/* VIRTUAL FUNCTIONS WRAPPERS */
void
print(void* obj)
{
  /* Calls virtual function of respective derived 'class' */
  ((Derived_obj_t*)obj)->vtable->print(obj);
}

void
draw(void* obj)
{
  /* Calls virtual function of respective derived 'class' */
  ((Derived_obj_t*)obj)->vtable->draw(obj);
}

void
updatePos(void* obj)
{
  /* Calls virtual function of respective derived 'class' */
  ((Derived_obj_t*)obj)->vtable->updatePos(obj);
}

void
destroy(void* obj)
{
  ((Derived_obj_t*)obj)->vtable->destroy(obj);
}

void
updateCollisions(void* obj, vector* col_matrix)
{
  ((Derived_obj_t*)obj)->vtable->updateCollision(obj, col_matrix);
}
