#include "include/menu.h"

/* VIRTUAL FUNCTIONS */
static void
printMenu(void* menu)
{
  Menu_t* m = (Menu_t*)menu;
  m->obj->vtable->print(m->obj);
}

static void
renderMenu(void* menu)
{
  Menu_t* f = (Menu_t*)menu;
  f->obj->vtable->draw(f->obj);
}

static void
destroyMenu(void* menu)
{
  Menu_t* f = (Menu_t*)menu;
  f->obj->vtable->destroy(f->obj);
  free(f);
}

const static Object_Vtable_t menu_vtable = { .draw            = renderMenu,
                                             .print           = printMenu,
                                             .updatePos       = NULL,
                                             .destroy         = destroyMenu,
                                             .updateCollision = NULL };

Menu_t*
new_menu(float x, float y, Sprite_t* sprite, size_t ID)
{
  Menu_t* menu = malloc(sizeof(Menu_t));

  if (!menu)
    return NULL;

  Object_t* obj = new_object(0, 0, x, y, sprite);
  if (!obj) {
    free(menu);
    return NULL;
  }
  menu->obj = obj;

  menu->vtable = &menu_vtable;

  menu->obj->identifier.id   = ID;
  menu->obj->identifier.type = MENU;

  return menu;
}
