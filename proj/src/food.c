#include "include/food.h"
#include "include/err_utils.h"

/* VIRTUAL FUNCTIONS */
static void
printFood(void* food)
{
  Food_t* f = (Food_t*)food;
  f->obj->vtable->print(f->obj);
  warn("Heal_amt : %u", f->nourishment);
}

static void
updateFoodPos(void* food)
{
  Food_t* f = (Food_t*)food;

  f->obj->vtable->updatePos(f->obj);
}

static void
renderFood(void* food)
{
  Food_t* f = (Food_t*)food;
  f->obj->vtable->draw(f->obj);
}

static void
destroyFood(void* food)
{
  Food_t* f = (Food_t*)food;
  f->obj->vtable->destroy(f->obj);
  free(f);
}

static void
updateCollisionFood(void* food, vector* col_matrix)
{
  Food_t* f = (Food_t*)food;
  f->obj->vtable->updateCollision(f, col_matrix);
}

const static Object_Vtable_t food_vtable = { .draw      = renderFood,
                                             .print     = printFood,
                                             .updatePos = updateFoodPos,
                                             .destroy   = destroyFood,
                                             .updateCollision =
                                               updateCollisionFood };

/* PUBLIC FUNCTIONS */
Food_t*
new_food(float x, float y, u_int16_t nourishment, Sprite_t* sprite)
{
  Food_t* food = malloc(sizeof(Food_t));

  if (!food)
    return NULL;

  Object_t* obj = new_object(0, 0, x, y, sprite);
  if (!obj) {
    free(food);
    return NULL;
  }
  food->obj = obj;

  food->nourishment = nourishment;
  food->vtable      = &food_vtable;

  static size_t curr_id      = 1;
  food->obj->identifier.id   = curr_id;
  food->obj->identifier.type = FOOD;
  ++curr_id;

  return food;
}
