#include "include/wall.h"
#include "include/err_utils.h"

/* VIRTUAL METHODS */
static void
destroyWal(void* w)
{
  Wall_t* wall = (Wall_t*)w;
  wall->obj->vtable->destroy(wall->obj);
  free(wall);
}

static void
printWall(void* w)
{
  Wall_t* wall = (Wall_t*)w;
  warn("Wall Type: %d", wall->type);
  wall->obj->vtable->print(wall->obj);
}

static void
updateWalPos(void* w)
{
  Wall_t* wall = (Wall_t*)w;
  wall->obj->vtable->updatePos(wall->obj);
}

static void
renderWall(void* w)
{
  Wall_t* wall    = (Wall_t*)w;
  uint16_t curr_y = (uint16_t)wall->obj->y;

  for (size_t i = 0; i < wall->height; ++i) {
    uint16_t curr_x = (uint16_t)wall->obj->x;
    for (size_t j = 0; j < wall->length; ++j) {
      DRAW_SPRITE(&wall->obj->sprite, curr_x, curr_y, wall->obj->transparency);
      curr_x += wall->obj->sprite.Width;
    }
    curr_y += wall->obj->sprite.Height;
  }
}

static void
updateCollisionWall(void* w, vector* col_matrix)
{
  Wall_t* wall          = (Wall_t*)w;
  uint16_t curr_y       = (uint16_t)wall->obj->y;
  vector* collided_objs = new_vector();

  for (size_t i = 0; i < wall->height; ++i) {
    uint16_t curr_x = (uint16_t)wall->obj->x;
    for (size_t j = 0; j < wall->length; ++j) {
      updateCollisionMatrix(
        w, col_matrix, curr_x, curr_y, &wall->obj->sprite, collided_objs);
      curr_x += wall->obj->sprite.Width;
    }
    curr_y += wall->obj->sprite.Height;
  }
}

const static Object_Vtable_t wall_vtable = { .draw      = renderWall,
                                             .print     = printWall,
                                             .updatePos = updateWalPos,
                                             .destroy   = destroyWal,
                                             .updateCollision =
                                               updateCollisionWall };

Wall_t*
new_wall(float x,
         float y,
         uint8_t length,
         uint8_t height,
         Sprite_t* sprite,
         wall_type type)
{
  Wall_t* wall = malloc(sizeof(Wall_t));
  if (!wall)
    return NULL;

  Object_t* obj = new_object(0, 0, x, y, sprite);
  if (!obj) {
    free(wall);
    return NULL;
  }
  wall->obj    = obj;
  wall->vtable = &wall_vtable;
  wall->length = length;
  wall->height = height;

  static int8_t curr_id      = 1;
  wall->obj->identifier.id   = curr_id;
  wall->obj->identifier.type = WALL;
  wall->type                 = type;
  ++curr_id;
  return wall;
}
