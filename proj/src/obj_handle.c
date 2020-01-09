#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/bmp.h"
#include "include/collisions.h"
#include "include/cursor.h"
#include "include/enemies.h"
#include "include/err_utils.h"
#include "include/obj_handle.h"
#include "include/object.h"
#include "include/serial.h"
#include "include/skane.h"
#include "include/vector.h"
#include "include/vg.h"
#include "include/wall.h"

#define NUM_LAYERS         NOT_SET
#define MAX_OBJS_PER_LAYER 1000

static Skane_t *ska, *ska2;
static Cursor_t* c;
static Menu_t *start_sing_menu, *start_mult_menu, *exit_menu, *title_menu,
  *loading_menu;
static vector* objs;
static vector* collision_matrix;
char respath[PATH_MAXSIZE];

/* OBJECT FUNCTIONS */
void
render_objects(void)
{
  /* Iterate through layers */
  for (size_t i = 0; i < objs->end; ++i) {
    /* iterate through objects in a layer */
    vector* curr_vec = (vector*)vector_at(objs, i);
    for (size_t j = 0; j < curr_vec->end; ++j)
      draw(vector_at(curr_vec, j));
  }

  draw(c);
}

void
calc_objs_pos(void)
{
  /* Iterate through layers */
  for (size_t i = 0; i < objs->end; ++i) {
    vector* curr_vec = (vector*)vector_at(objs, i);
    /* iterate through objects in a layer */
    for (size_t j = 0; j < curr_vec->end; ++j)
      updatePos(vector_at(curr_vec, j));
  }
}

void
clear_collision_matrix(void)
{
  for (size_t i = 0; i < collision_matrix->end; ++i)
    vector_clear(vector_at(collision_matrix, i));
}

void
update_objs_collisions(void)
{
  /* Iterate through layers until skane body */
  for (size_t i = 0; i < SKANE; ++i) {
    vector* curr_vec = (vector*)vector_at(objs, i);
    /* iterate through objects in a layer */
    for (size_t j = 0; j < curr_vec->end; ++j) {
      updateCollisions(vector_at(curr_vec, j), collision_matrix);
    }
  }

  /* Iterate through layers from skane body to last layer */
  for (size_t i = SKANE + 1; i < NOT_SET; ++i) {
    vector* curr_vec = (vector*)vector_at(objs, i);
    /* iterate through objects in a layer */
    for (size_t j = 0; j < curr_vec->end; ++j)
      updateCollisions(vector_at(curr_vec, j), collision_matrix);
  }

  /* Update skane body last (object of lesser importance collision-wise) */
  vector* curr_vec = (vector*)vector_at(objs, SKANE);
  /* iterate through objects in a layer */
  for (size_t j = 0; j < curr_vec->end; ++j)
    updateCollisions(vector_at(curr_vec, j), collision_matrix);
}

void
debug_collisions(void)
{
  static int cl = 0;
  for (size_t i = 0; i < collision_matrix->size; ++i) {
    vector* vec = (vector*)vector_at(collision_matrix, i);
    for (size_t j = 0; j < vec->end; ++j) {
      if (vector_at(vec, j) != NULL)
        draw_line(j, i, cl, 1);
    }
  }
  ++cl;
  if (cl > 10)
    cl = 0;
}

int
garbage_collector(void)
{
  /* a skane's death means an end game so we treat it independently */
  if (ska && ska->obj->identifier.id == 0) {
    destroy(ska);
    ska = NULL;
    if (ska2 && ska2->obj->identifier.id == 0) {
      destroy(ska2);
      ska2 = NULL;
    }

    return 1;
  }

  for (size_t i = 0; i < objs->end; ++i) {
    vector* curr_vec = (vector*)vector_at(objs, i);
    for (size_t j = 0; j < curr_vec->end; ++j) {
      Derived_obj_t* d_obj = (Derived_obj_t*)vector_at(curr_vec, j);
      if (d_obj->obj->identifier.id == 0) {
        vector_delete(curr_vec, j);
        --j;
      }
    }
  }

  return 0;
}

int
add_object(void* object_to_be_added, int layer)
{
  if (!object_to_be_added) {
    warn("%s: Tried to add an invalid object to objects array", __func__);
    return 1;
  }

  vector_push_back((vector*)vector_at(objs, layer), object_to_be_added);
  return 0;
}

void
remove_object(int object_id, int object_type)
{
  vector* vec = vector_at(objs, object_type);
  for (size_t i = 0; i < vec->end; i++) {
    void* object = (vector_at(vec, i));
    if (((Derived_obj_t*)object)->obj->identifier.id == object_id) {
      vector_delete(vec, i);
      destroy(object);
      return;
    }
  }
}

void
destroy_all_objects(void)
{
  if (ska) {
    destroy(ska);
    ska = NULL;
    if (ska2) {
      destroy(ska2);
      ska2 = NULL;
    }
  }
  /* free nested vectors and destroy all their objects */
  for (size_t i = 0; i < objs->end; ++i) {
    vector* curr_vec = (vector*)vector_at(objs, i);
    for (size_t j = 0; j < curr_vec->end; ++j) {
      destroy(vector_at(curr_vec, j));
    }
    free_vector(curr_vec);
  }
  free_vector(objs);
}

void
alloc_obj_matrix(void)
{
  /* allocate object matrix */
  objs = new_vector();
  vector_reserve(objs, NUM_LAYERS);
  for (size_t i = NUM_LAYERS; i; --i) {
    vector* new_objs = new_vector();
    vector_push_back(objs, new_objs);
  }
}

void
alloc_collison_matrix(void)
{
  /* allocate collision matrix */
  collision_matrix = new_vector();
  if (vector_reserve(collision_matrix, get_v_res()) != get_v_res())
    die("Not enough memory to allocate collision matrix");

  unsigned h_res = get_h_res();
  for (size_t i = get_v_res(); i; --i) {
    vector* new_col_vec = new_vector();
    if (vector_reserve(new_col_vec, h_res) != h_res)
      die("Not enough memory to allocate collision matrix");

    for (size_t j = h_res; j; --j)
      vector_push_back(new_col_vec, NULL);

    vector_push_back(collision_matrix, new_col_vec);
  }
}

/* MENUS */
void
delete_menus(void)
{
  if (start_sing_menu) {
    destroy(start_sing_menu);
    start_sing_menu = NULL;
  }
  if (start_mult_menu) {
    destroy(start_mult_menu);
    start_mult_menu = NULL;
  }
  if (exit_menu) {
    destroy(exit_menu);
    exit_menu = NULL;
  }
  if (loading_menu) {
    destroy(loading_menu);
    loading_menu = NULL;
  }
}

void
draw_menus(void)
{
  draw(title_menu);
  draw(start_sing_menu);
  draw(start_mult_menu);
  draw(exit_menu);
}

void
draw_loading_menu(void)
{
  draw(loading_menu);
}

/* INSTANCIATION FUNCTIONS */
static Skane_t*
inst_skane_1(void)
{
  ska_sprt_t ska_sprt;

  /* skane sprites */
  if (new_sprite_bmp(make_path(SKA1_HEADPATH), &ska_sprt.h_sprite))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_BODYPATH), &ska_sprt.b_sprite))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_TAILPATH), &ska_sprt.t_sprite))
    die("%s:", __func__);

  /* missle sprites */
  if (new_sprite_bmp(make_path(SKA1_MISPATH), &ska_sprt.m_sprite))
    die("%s:", __func__);

  /* food sprites */
  if (new_sprite_bmp(make_path(FOODPATH), &ska_sprt.f_sprite))
    die("%s:", __func__);

  /* enemy sprites */
  if (new_sprite_bmp(make_path(SKA1_ENEPATH), &ska_sprt.ene_sprite[0]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_ENEPATH_2), &ska_sprt.ene_sprite[1]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_ENEPATH_3), &ska_sprt.ene_sprite[2]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_ENEPATH_4), &ska_sprt.ene_sprite[3]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA1_ENEPATH_ATK), &ska_sprt.ene_sprite[4]))
    die("%s:", __func__);

  /* instanciate skane */
  Skane_t* ska = new_skane(SKA_X, SKA_Y, SKA_S, SKA_HP, MIS_DMG, &ska_sprt);
  if (!ska)
    die("%s: Can't instanciate skane.", __func__);

  return ska;
}

static Skane_t*
inst_skane_2(void)
{
  ska_sprt_t ska_sprt;

  /* skane sprites */
  if (new_sprite_bmp(make_path(SKA2_HEADPATH), &ska_sprt.h_sprite))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_BODYPATH), &ska_sprt.b_sprite))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_TAILPATH), &ska_sprt.t_sprite))
    die("%s:", __func__);

  /* missle sprites */
  if (new_sprite_bmp(make_path(SKA2_MISPATH), &ska_sprt.m_sprite))
    die("%s:", __func__);

  /* food sprites */
  if (new_sprite_bmp(make_path(FOODPATH), &ska_sprt.f_sprite))
    die("%s:", __func__);

  /* enemy sprites */
  if (new_sprite_bmp(make_path(SKA2_ENEPATH), &ska_sprt.ene_sprite[0]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_ENEPATH_2), &ska_sprt.ene_sprite[1]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_ENEPATH_3), &ska_sprt.ene_sprite[2]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_ENEPATH_4), &ska_sprt.ene_sprite[3]))
    die("%s:", __func__);
  if (new_sprite_bmp(make_path(SKA2_ENEPATH_ATK), &ska_sprt.ene_sprite[4]))
    die("%s:", __func__);

  /* instanciate skane */
  Skane_t* ska = new_skane(get_h_res() - SKA_X_2,
                           get_v_res() - SKA_Y_2,
                           SKA_S,
                           SKA_HP,
                           MIS_DMG,
                           &ska_sprt);
  if (!ska)
    die("%s: Can't instanciate skane.", __func__);

  return ska;
}

void
inst_skane(gamestate gamest)
{
  /* set players skane colors */
  if (gamest == SINGLE) {
    ska = inst_skane_1();
    add_object(ska, SKANE);
  }
  else if (gamest == MULT1) {
    ska = inst_skane_1();
    add_object(ska, SKANE);
    ska2 = inst_skane_2();
    add_object(ska2, SKANE);
  }
  else if (gamest == MULT2) {
    ska = inst_skane_2();
    add_object(ska, SKANE);
    ska2 = inst_skane_1();
    add_object(ska2, SKANE);
  }
  else
    die("%s: Unexpected game state.", __func__);
}

void
inst_cursor(void)
{
  Sprite_t c_spr;
  if (new_sprite_bmp(make_path(CURSORPATH), &c_spr))
    die("%s:", __func__);

  c = new_cursor(&c_spr);
  if (!c)
    die("%s: Can't instanciate cursor.", __func__);
}

void
inst_menus(void)
{
  Sprite_t m_title;
  if (new_sprite_bmp(make_path(MENU_TITLE_PATH), &m_title))
    die("Can't inst main menu title %s:", __func__);
  title_menu = new_menu(get_h_res() - m_title.Width - MENU_TITLE_SCR_X,
                        get_v_res() / 2 - m_title.Height / 2 - MENU_TITLE_SCR_Y,
                        &m_title,
                        MENU_TITLE_ID);

  Sprite_t m_loading;
  if (new_sprite_bmp(make_path(MENU_LOAD_PATH), &m_loading))
    die("Can't inst loading menu %s:", __func__);
  loading_menu = new_menu(get_h_res() / 2 - m_loading.Width / 2,
                          get_v_res() / 2 - m_loading.Height / 2,
                          &m_loading,
                          MENU_LOAD_ID);

  Sprite_t m_sing_spr;
  if (new_sprite_bmp(make_path(MENU_START_SINGLE_PATH), &m_sing_spr))
    die("%s:", __func__);
  start_sing_menu =
    new_menu(MENU_SINGLE_X_POS, MENU_SINGLE_Y_POS, &m_sing_spr, MENU_SINGLE_ID);
  if (!start_sing_menu)
    die("%s: Can't inst singleplayer menu.", __func__);

  Sprite_t m_mult_spr;
  if (new_sprite_bmp(make_path(MENU_START_MULTIP_PATH), &m_mult_spr))
    die("%s:", __func__);
  start_mult_menu =
    new_menu(MENU_MULTIP_X_POS,
             get_v_res() / 2 - m_mult_spr.Height / 2 + MENU_MULTIP_Y_POS,
             &m_mult_spr,
             MENU_MULTIP_ID);
  if (!start_mult_menu)
    die("%s: Can't inst multliplayer menu.", __func__);

  Sprite_t m_exit_spr;
  if (new_sprite_bmp(make_path(MENU_EXIT_PATH), &m_exit_spr))
    die("%s:", __func__);
  exit_menu = new_menu(MENU_EXIT_X_POS,
                       get_v_res() - m_mult_spr.Height - MENU_EXIT_Y_POS,
                       &m_exit_spr,
                       MENU_EXIT_ID);
  if (!exit_menu)
    die("%s: Can't inst exit menu.", __func__);
}

void
spawn_enemy(gamestate gamest)
{
  /* variable that scale difficulty */
  uint8_t grp_size;
  if (gamest == SINGLE) { // skale difficulty in single player
    skane_diff(ska, false);
    grp_size = (uint8_t)((rand() % (int)(ENE_MAX_GRPSIZE + ska->ediff->gsize)) +
                         ENE_MIN_GRPSIZE + ska->ediff->gsize);
  }
  else {
    grp_size = (rand() % ENE_MAX_GRPSIZE) + ENE_MIN_GRPSIZE;
  }

  /* get random enemy group size */
  uint16_t spawn_x, spawn_y;
  uint8_t spawn_cnt = 0;
  Enemy_t* newene;

  /* set spawn location */
  if (gamest == SINGLE || gamest == MULT1) {
    spawn_x = ENE_X;
    spawn_y = ENE_Y;
  }
  else if (gamest == MULT2) {
    spawn_x = get_h_res() - ENE_X;
    spawn_y = get_v_res() - ENE_Y;
  }
  else {
    die("%s: Player number not supported", __func__);
  }

  /* spawn enemy group */
  for (; grp_size; --grp_size) {
    newene = new_enemy(spawn_x,
                       spawn_y,
                       ENE_S + ska->ediff->es, // sums 0 if multiplyer
                       ENE_DMG,
                       ENE_HP,
                       ENE_NOURISH,
                       ENE_ATKDELAY,
                       ska);
    if (newene) { // skip spawning failed enemies
      add_object(newene, ENEMY);
      ++spawn_cnt;
    }
    else
      warn("%s: Failed spawing enemy", __func__);
  }

  /* send multiplayer info */
  if (gamest == MULT1 || gamest == MULT2) {
    serial_send_push(HTCHECK + SERIAL_ENE_SPA);
    serial_send_push(spawn_cnt);
  }
}

void
spawn_allies(uint8_t grp_size, gamestate gamest)
{
  /* get random enemy group size */
  uint16_t spawn_x, spawn_y;
  Enemy_t* newene;

  /* set spawn location */
  if (gamest == SINGLE || gamest == MULT1) {
    spawn_x = get_h_res() - ENE_X;
    spawn_y = get_v_res() - ENE_Y;
  }
  else if (gamest == MULT2) {
    spawn_x = ENE_X;
    spawn_y = ENE_Y;
  }
  else {
    die("%s: Player number not supported", __func__);
  }

  /* spawn enemy group */
  for (; grp_size; --grp_size) {
    newene = new_enemy(spawn_x,
                       spawn_y,
                       ENE_S,
                       ENE_DMG,
                       ENE_HP,
                       ENE_NOURISH,
                       ENE_ATKDELAY,
                       ska2);
    if (newene) // skip spawning failed enemies
      add_object(newene, ENEMY);
    else
      warn("%s: Failed spawing enemy", __func__);
  }
}

void
create_map(gamestate gamest)
{
  // TODO read map from file
  Wall_t* w;
  Sprite_t h_w_spr;
  if (new_sprite_bmp(make_path(WALLSEGMENT), &h_w_spr))
    die("%s:", __func__);

  Sprite_t* v_w_spr = rotate_sprite_intPI(&h_w_spr, 1);
  if (!v_w_spr)
    die("%s:", __func__);

  Sprite_t c_w_spr;
  if (new_sprite_bmp(make_path(WALLCORNER), &c_w_spr))
    die("%s:", __func__);

  /* Map Borders */
  w = new_wall(0, 0, get_h_res() / h_w_spr.Width, 1, &h_w_spr, HORIZ_WALL);
  add_object(w, WALL);

  w = new_wall(0,
               get_v_res() - h_w_spr.Height,
               get_h_res() / h_w_spr.Width,
               1,
               &h_w_spr,
               HORIZ_WALL);
  add_object(w, WALL);

  w = new_wall(0,
               v_w_spr->Height,
               1,
               get_v_res() / v_w_spr->Height - 1,
               v_w_spr,
               VERT_WALL);
  add_object(w, WALL);

  w = new_wall(get_h_res() - v_w_spr->Width,
               v_w_spr->Height,
               1,
               get_v_res() / v_w_spr->Height - 1,
               v_w_spr,
               VERT_WALL);
  add_object(w, WALL);

  /* border corners */
  w = new_wall(0, 0, 1, 1, &c_w_spr, VERT_WALL);
  add_object(w, WALL);
  w = new_wall(0, get_v_res() - c_w_spr.Height, 1, 1, &c_w_spr, VERT_WALL);
  add_object(w, WALL);
  w = new_wall(get_h_res() - c_w_spr.Width, 0, 1, 1, &c_w_spr, VERT_WALL);
  add_object(w, WALL);
  w = new_wall(get_h_res() - c_w_spr.Width,
               get_v_res() - c_w_spr.Height,
               1,
               1,
               &c_w_spr,
               VERT_WALL);
  add_object(w, WALL);

  /* spawners */
  Sprite_t spawner_spr;
  if (new_sprite_bmp(make_path(SKA1_SPAWNER), &spawner_spr))
    die("%s:", __func__);
  w = new_wall(ENE_X - spawner_spr.Width / 2,
               ENE_Y - spawner_spr.Height / 2,
               1,
               1,
               &spawner_spr,
               RECT_WALL);
  add_object(w, WALL);

  if (gamest == MULT1 || gamest == MULT2) {
    if (new_sprite_bmp(make_path(SKA2_SPAWNER), &spawner_spr))
      die("%s:", __func__);
    w = new_wall(get_h_res() - ENE_X - spawner_spr.Width / 2,
                 get_v_res() - ENE_Y - spawner_spr.Height / 2,
                 1,
                 1,
                 &spawner_spr,
                 RECT_WALL);
    add_object(w, WALL);
  }

  /* Some walls for enemy and wall collision testing */
  /* w = new_wall(200, 200, 20, 1, &h_w_spr, HORIZ_WALL); */
  /* add_object(w, WALL); */
  /* w = new_wall(200 + 19 * h_w_spr.Width, 200, 1, 1, &c_w_spr, VERT_WALL); */
  /* add_object(w, WALL); */

  /* w = new_wall(200, 200, 1, 20, v_w_spr, VERT_WALL); */
  /* add_object(w, WALL); */
}

/* GETTERS/SETTERS */
void
set_ska2_state(int8_t new_state)
{
  ska2->curr_state = new_state;
}

int
ska1_fire_missle(void)
{
  if (skane_can_shoot(ska))
    return add_object(fire_missle(ska, get_center_x(c), get_center_y(c)),
                      MISSILE);

  return 0;
}

int
ska2_fire_missle(float x, float y)
{
  return add_object(fire_missle(ska2, x, y), MISSILE);
}

void
ska1_mov(input_array_t input_array)
{
  skane_mov(ska, input_array);
}

float
get_cursor_x(void)
{
  return c->obj->x;
}

float
get_cursor_y(void)
{
  return c->obj->y;
}

float
get_cursor_center_x(void)
{
  return get_center_x(c);
}

float
get_cursor_center_y(void)
{
  return get_center_y(c);
}

float
get_menu_x(Menu_t* menu)
{
  return menu->obj->x;
}

float
get_menu_y(Menu_t* menu)
{
  return menu->obj->y;
}

Menu_t*
get_title_menu(void)
{
  return title_menu;
}

Menu_t*
get_exit_menu(void)
{
  return exit_menu;
}

Menu_t*
get_sing_menu(void)
{
  return start_sing_menu;
}

Menu_t*
get_mult_menu(void)
{
  return start_mult_menu;
}

void
draw_cursor(void)
{
  draw(c);
}

void
update_cursor(int16_t x, int16_t y)
{
  updateCursor(c, x, y);
}

/* SYNC */
int
transmit_skane_info(void)
{
  if (skane_just_shot(ska)) {
    if (ska->changed_direc) {
      /* queue in skane missle */
      serial_send_push(HTCHECK + SERIAL_SKA_MIS);
      serial_send_push_float(get_cursor_center_x());
      serial_send_push_float(get_cursor_center_y());

      /* queue in skane movement changes as last byte */
      serial_send_push(HTCHECK + SERIAL_SKA_MOV + SERIAL_SYNC_PACK_HT);
      serial_send_push(ska->curr_state); // send new skane state info

      return 0;
    }
    else {
      /* queue in skane missle as last byte */
      serial_send_push(HTCHECK + SERIAL_SKA_MIS + SERIAL_SYNC_PACK_HT);
      serial_send_push_float(get_center_x(c));
      serial_send_push_float(get_center_y(c));

      return 0;
    }
  }
  else if (ska->changed_direc) { // changed directions/state
    /* queue in skane movement changes as last byte */
    serial_send_push(HTCHECK + SERIAL_SKA_MOV + SERIAL_SYNC_PACK_HT);
    serial_send_push(ska->curr_state); // send new skane state info

    return 0;
  }

  return 1;
}
