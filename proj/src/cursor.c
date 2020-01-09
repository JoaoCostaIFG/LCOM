#include "include/cursor.h"
#include "include/err_utils.h"
#include "include/vg.h"

void
updateCursor(Cursor_t* cursor, int16_t delta_x, int16_t delta_y)
{
  /* saving h_res and v_res info to prevent multiple getter calls */
  unsigned h_res = get_h_res();
  unsigned v_res = get_v_res();

  if ((cursor->obj->x += delta_x) < 0)
    cursor->obj->x = 0;
  else if ((cursor->obj->x + cursor->obj->sprite.Width) > h_res)
    cursor->obj->x = h_res - cursor->obj->sprite.Width;

  if ((cursor->obj->y -= delta_y) < 0)
    cursor->obj->y = 0;
  else if ((cursor->obj->y + cursor->obj->sprite.Height) > v_res)
    cursor->obj->y = v_res - cursor->obj->sprite.Height;
}

float
get_center_x(Cursor_t* cursor)
{
  return cursor->obj->x + cursor->coff_x;
}

float
get_center_y(Cursor_t* cursor)
{
  return cursor->obj->y + cursor->coff_y;
}

static void
renderCursor(void* cursor)
{
  Cursor_t* c = (Cursor_t*)cursor;
  c->obj->vtable->draw(c->obj);
}

static void
printCursor(void* cursor)
{
  Cursor_t* c = (Cursor_t*)cursor;
  warn("CURSOR");
  c->obj->vtable->print(c->obj);
}

static void
destroyCursor(void* cursor)
{
  Cursor_t* c = (Cursor_t*)cursor;
  c->obj->vtable->destroy(cursor);
  free(c);
}

static void
updateCollisionCursor(void* cursor, vector* col_matrix)
{
  Cursor_t* c = (Cursor_t*)cursor;
  c->obj->vtable->updateCollision(cursor, col_matrix);
}

const static Object_Vtable_t cursor_vtable = { .draw    = renderCursor,
                                               .print   = printCursor,
                                               .destroy = destroyCursor,
                                               .updateCollision =
                                                 updateCollisionCursor };

Cursor_t*
new_cursor(Sprite_t* sprite)
{
  Cursor_t* cursor = (Cursor_t*)malloc(sizeof(Cursor_t));
  if (!cursor)
    return NULL;

  /* Set initial cursor to the middle of the screen */
  Object_t* obj =
    new_object(0, 0, get_h_res() / 2.0, get_v_res() / 2.0, sprite);
  if (!obj)
    return NULL;
  cursor->obj = obj;

  /* save info related to the center of the cursor */
  cursor->coff_x = sprite->Width / 2.0;
  cursor->coff_y = sprite->Height / 2.0;

  static size_t curr_id        = 1;
  cursor->obj->identifier.id   = curr_id;
  cursor->obj->identifier.type = CURSOR;
  ++curr_id;

  cursor->vtable = &cursor_vtable;
  return cursor;
}
