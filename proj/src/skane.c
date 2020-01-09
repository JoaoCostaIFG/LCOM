#include <math.h>
#include <stdio.h>

#include "include/bmp.h"
#include "include/err_utils.h"
#include "include/skane.h"

/* PRIVATE */
static inline void
update_dir(Skane_t* ska)
{
  switch (ska->curr_state) {
    case E:
      ska->obj->speed_x = ska->s;
      ska->obj->speed_y = 0;
      break;
    case N:
      ska->obj->speed_x = 0;
      ska->obj->speed_y = -ska->s;
      break;
    case W:
      ska->obj->speed_x = -ska->s;
      ska->obj->speed_y = 0;
      break;
    case S:
      ska->obj->speed_x = 0;
      ska->obj->speed_y = ska->s;
      break;
    case NE:
      ska->obj->speed_x = ska->s;
      ska->obj->speed_y = -ska->s;
      break;
    case NW:
      ska->obj->speed_x = -ska->s;
      ska->obj->speed_y = -ska->s;
      break;
    case SE:
      ska->obj->speed_x = ska->s;
      ska->obj->speed_y = ska->s;
      break;
    case SW:
      ska->obj->speed_x = -ska->s;
      ska->obj->speed_y = ska->s;
      break;
    default:
      ska->obj->speed_x = 0;
      ska->obj->speed_y = 0;
      break;
  }
}

static inline void
chain_step(Skane_t* ska, seg* seg)
{
  /* calculate skane's tail position after each step (goes from head to tail) */
  float speed = floor(ska->s) * seg->len;

  switch (seg->dir) {
    case E:
      ska->t_x -= speed;
      DRAW_RECT(ska->t_x,
                ska->t_y,
                speed + 2,
                ska->cell_size,
                ska->ska_sprt.b_sprite.Data[0]);
      break;
    case N:
      DRAW_RECT(ska->t_x,
                ska->t_y + ska->cell_size,
                ska->cell_size,
                speed,
                ska->ska_sprt.b_sprite.Data[0]);
      ska->t_y += speed;
      break;
    case W:
      DRAW_RECT(ska->t_x + ska->cell_size,
                ska->t_y,
                speed,
                ska->cell_size,
                ska->ska_sprt.b_sprite.Data[0]);
      ska->t_x += speed;
      break;
    case S:
      ska->t_y -= speed;
      DRAW_RECT(ska->t_x,
                ska->t_y,
                ska->cell_size,
                speed + 2,
                ska->ska_sprt.b_sprite.Data[0]);
      break;
    case NE:
      for (size_t i = 0; i < speed; ++i) {
        ++ska->t_y;
        --ska->t_x;
        DRAW_RECT(ska->t_x,
                  ska->t_y,
                  ska->cell_size,
                  ska->cell_size,
                  ska->ska_sprt.b_sprite.Data[0]);
      }
      break;
    case NW:
      for (size_t i = 0; i < speed; ++i) {
        ++ska->t_y;
        ++ska->t_x;
        DRAW_RECT(ska->t_x,
                  ska->t_y,
                  ska->cell_size,
                  ska->cell_size,
                  ska->ska_sprt.b_sprite.Data[0]);
      }
      break;
    case SE:
      for (size_t i = 0; i < speed; ++i) {
        --ska->t_y;
        --ska->t_x;
        DRAW_RECT(ska->t_x,
                  ska->t_y,
                  ska->cell_size,
                  ska->cell_size,
                  ska->ska_sprt.b_sprite.Data[0]);
      }
      break;
    case SW:
      for (size_t i = 0; i < speed; ++i) {
        --ska->t_y;
        ++ska->t_x;
        DRAW_RECT(ska->t_x,
                  ska->t_y,
                  ska->cell_size,
                  ska->cell_size,
                  ska->ska_sprt.b_sprite.Data[0]);
      }
      break;
    default:
      /* STOP case */
      break;
  }
}

static inline void
chain_step_coll(Skane_Body_t* ska_body,
                seg* seg,
                vector* col_matrix,
                vector* objs_to_ignore)
{
  Skane_t* ska = ska_body->ska;

  /* calculate skane's tail position after each step (goes from head to tail) */
  float speed = floor(ska->s) * seg->len;

  switch (seg->dir) {
    case E:
      ska->t_x -= speed;
      updateCollisionMatrixRect(ska_body,
                                col_matrix,
                                ska->t_x,
                                ska->t_y,
                                speed + 2,
                                ska->cell_size,
                                objs_to_ignore);
      break;
    case N:
      updateCollisionMatrixRect(ska_body,
                                col_matrix,
                                ska->t_x,
                                ska->t_y + ska->cell_size,
                                ska->cell_size,
                                speed,
                                objs_to_ignore);
      ska->t_y += speed;
      break;
    case W:
      updateCollisionMatrixRect(ska_body,
                                col_matrix,
                                ska->t_x + ska->cell_size,
                                ska->t_y,
                                speed,
                                ska->cell_size,
                                objs_to_ignore);
      ska->t_x += speed;
      break;
    case S:
      ska->t_y -= speed;
      updateCollisionMatrixRect(ska_body,
                                col_matrix,
                                ska->t_x,
                                ska->t_y,
                                ska->cell_size,
                                speed + 2,
                                objs_to_ignore);
      break;
    case NE:
      for (size_t i = 0; i < speed; ++i) {
        ++ska->t_y;
        --ska->t_x;
        updateCollisionMatrixRect(ska_body,
                                  col_matrix,
                                  ska->t_x,
                                  ska->t_y,
                                  ska->cell_size,
                                  ska->cell_size,
                                  objs_to_ignore);
      }
      break;
    case NW:
      for (size_t i = 0; i < speed; ++i) {
        ++ska->t_y;
        ++ska->t_x;
        updateCollisionMatrixRect(ska_body,
                                  col_matrix,
                                  ska->t_x,
                                  ska->t_y,
                                  ska->cell_size,
                                  ska->cell_size,
                                  objs_to_ignore);
      }
      break;
    case SE:
      for (size_t i = 0; i < speed; ++i) {
        --ska->t_y;
        --ska->t_x;
        updateCollisionMatrixRect(ska_body,
                                  col_matrix,
                                  ska->t_x,
                                  ska->t_y,
                                  ska->cell_size,
                                  ska->cell_size,
                                  objs_to_ignore);
      }
      break;
    case SW:
      for (size_t i = 0; i < speed; ++i) {
        --ska->t_y;
        ++ska->t_x;
        updateCollisionMatrixRect(ska_body,
                                  col_matrix,
                                  ska->t_x,
                                  ska->t_y,
                                  ska->cell_size,
                                  ska->cell_size,
                                  objs_to_ignore);
      }
      break;
    default:
      /* STOP case */
      break;
  }
}

static void
printSkane(void* skane)
{
  Skane_t* ska = (Skane_t*)skane;
  ska->obj->vtable->print(ska->obj);

  warn("Health: %u, Collision direc: %d, Curr_state: %d",
       ska->health,
       ska->collision_direc,
       ska->curr_state);

  for (size_t i = 0; i < ska->directions->end; ++i)
    warn("%d %d",
         ((seg*)vector_at(ska->directions, i))->dir,
         ((seg*)vector_at(ska->directions, i))->len);
  warn(" :\n");
}

static inline void
add_seg(Skane_t* ska)
{
  seg* temp_seg = (seg*)malloc(sizeof(seg));
  if (!temp_seg)
    return;

  temp_seg->len = 1;
  temp_seg->dir = ska->curr_state;
  vector_insert(ska->directions, 0, (void*)temp_seg);
}

static void
updateSkanePos(void* skane)
{
  /* cast skane */
  Skane_t* ska = (Skane_t*)skane;

  /* take care of the head */
  ska->curr_state -= ska->collision_direc;
  ska->collision_direc = STOP;
  update_dir(ska); // update current speed values based on state

  /* update directions vector */
  if (ska->curr_state != STOP) {
    ska->obj->x += ska->obj->speed_x;
    ska->obj->y += ska->obj->speed_y; // move head based on speed values

    /* take care of the head */
    seg* temp_seg = (seg*)vector_begin(ska->directions);
    if (!temp_seg)
      return;
    if (temp_seg->dir == ska->curr_state)
      ++temp_seg->len; // add another step in this direction
    else
      add_seg(ska); // add a new segment

    /* get rid of the processed tail part */
    temp_seg = vector_end(ska->directions);
    --temp_seg->len;
    if (!temp_seg->len)
      vector_pop_and_free(ska->directions);
  }

  /* shooting cooldown */
  if (ska->fire_cd)
    --ska->fire_cd;
}

static void
renderSkane(void* skane)
{
  Skane_t* ska = (Skane_t*)skane;

  /* take care of the tail */
  /* DRAW_SPRITE( */
  /* &ska->ska_sprt.t_sprite, ska->t_x, ska->t_y, ska->obj->transparency); */

  /* draw body pieces (right next to head, until tail) */
  ska->t_x = ska->obj->x;
  ska->t_y = ska->obj->y; // calculate new tail pos
  seg* curr_dir;

  for (size_t i = 0; i < ska->directions->end; ++i) {
    curr_dir = ((seg*)vector_at(ska->directions, i));
    chain_step(ska, curr_dir);
  }

  /* draw head */
  Sprite_t* new;
  /* no need to do these calculations if the Skane doesn't move */
  if (ska->draw_direc != ska->curr_state) {
    /* can't free the N head sprite (base one) */
    if (ska->ska_sprt.h_sprite.Data != ska->obj->sprite.Data &&
        ska->curr_state != STOP)
      free(ska->obj->sprite.Data);

    /* update last drawn state */
    ska->draw_direc    = ska->curr_state;
    ska->changed_direc = true;

    switch (ska->curr_state) {
      case E:
        new          = rotate_sprite_intPI(&ska->ska_sprt.h_sprite, -1);
        ska->offsetx = 0;
        ska->offsety = 0;
        break;
      case N:
        new          = &ska->ska_sprt.h_sprite;
        ska->offsetx = 0;
        ska->offsety = 0;
        break;
      case W:
        new          = rotate_sprite_intPI(&ska->ska_sprt.h_sprite, 1);
        ska->offsetx = 0;
        ska->offsety = 0;
        break;
      case S:
        new          = rotate_sprite_intPI(&ska->ska_sprt.h_sprite, 2);
        ska->offsetx = 0;
        ska->offsety = 0;
        break;
      case NE:
        new          = rotate_sprite(&ska->ska_sprt.h_sprite, -M_PI_2 / 2);
        ska->offsetx = 0;
        ska->offsety = ska->cell_size - new->Height;
        break;
      case NW:
        new          = rotate_sprite(&ska->ska_sprt.h_sprite, M_PI_2 / 2);
        ska->offsetx = ska->cell_size - new->Height;
        ska->offsety = ska->cell_size - new->Height;
        break;
      case SE:
        new          = rotate_sprite(&ska->ska_sprt.h_sprite, -M_PI * 0.75);
        ska->offsetx = sqrt(ska->cell_size);
        ska->offsety = sqrt(ska->cell_size);
        break;
      case SW:
        new          = rotate_sprite(&ska->ska_sprt.h_sprite, M_PI * 0.75);
        ska->offsetx = ska->cell_size - new->Height;
        ska->offsety = 0;
        break;
      case STOP:
        new = NULL;
        break;
      default: // for safety
        new          = sprite_cpy(&ska->ska_sprt.h_sprite);
        ska->offsetx = 0;
        ska->offsety = 0;
        break;
    }

    /* change sprite */
    if (new) {
      ska->obj->sprite = *new;
      free(new);
    }
  }
  else { // skane didn't change direction
    ska->changed_direc = false;
  }

  if (!ska->obj->sprite.Data) {
    warn("%s: Skane sprite broke.", __func__);
    return;
  }
  DRAW_SPRITE(&ska->obj->sprite,
              ska->obj->x + ska->offsetx,
              ska->obj->y + ska->offsety,
              1);
}

static void
destroySka(void* skane)
{
  Skane_t* ska = (Skane_t*)skane;
  ska->obj->vtable->destroy(ska->obj);
  free(ska->ska_body->obj);
  free(ska->ska_body);
  free_vector_data(ska->directions);
  free(ska->ska_sprt.h_sprite.Data);
  free(ska->ska_sprt.b_sprite.Data);
  free(ska->ska_sprt.t_sprite.Data);
  free(ska->ska_sprt.m_sprite.Data);
  free(ska->ska_sprt.f_sprite.Data);

  for (size_t i = 0; i < ENE_ANIMCYCLE + ENE_ATK_ANIMCYCLE; ++i)
    free(ska->ska_sprt.ene_sprite[i].Data);
  free(ska->ska_sprt.ene_sprite);

  free(ska);
}

static void
updateCollisionSkane(void* skane, vector* col_matrix)
{
  Skane_t* ska = (Skane_t*)skane;
  ska->obj->vtable->updateCollision(ska, col_matrix);

  /* Ignore skane head */
  vector* objs_to_ignore = new_vector();
  vector_push_back(objs_to_ignore, skane);

  ska->t_x = ska->obj->x;
  ska->t_y = ska->obj->y; // calculate new tail pos
  seg* curr_dir;

  /* Update skane collsion with other skane */
  if (ska->has_col_skane != 0)
    --ska->has_col_skane;

  for (size_t i = 0; i < ska->directions->end; ++i) {
    curr_dir = ((seg*)vector_at(ska->directions, i));
    chain_step_coll(ska->ska_body, curr_dir, col_matrix, objs_to_ignore);
  }
}

/* Skane Vtable */
const static Object_Vtable_t skane_vtable = { .draw      = renderSkane,
                                              .print     = printSkane,
                                              .updatePos = updateSkanePos,
                                              .destroy   = destroySka,
                                              .updateCollision =
                                                updateCollisionSkane };

/* PUBLIC */
Skane_t*
new_skane(float x,
          float y,
          float speed,
          uint8_t health,
          uint16_t damage,
          ska_sprt_t* ska_sprt)
{
  /* Allocation */
  Skane_t* skane = (Skane_t*)malloc(sizeof(Skane_t));
  if (!skane)
    return NULL;

  /* allocate body segments vector */
  skane->directions = new_vector();
  if (!skane->directions) {
    free(skane);
    return NULL;
  }

  /* reset skane's enemies difficulty */
  skane->ediff = (enemy_diff*)malloc(sizeof(enemy_diff));
  if (!skane->ediff) {
    free_vector(skane->directions);
    free(skane);
    return NULL;
  }
  skane->ediff->es    = 0;
  skane->ediff->gsize = 0;
  skane->ediff->shots   = 0;
  skane->ediff->shotup  = 0;

  /* Sprite_t* new = sprite_cpy(&ska_sprt->h_sprite); */
  skane->obj = new_object(speed, speed, x, y, &ska_sprt->h_sprite);
  if (!skane->obj) {
    free(skane->ediff);
    free_vector(skane->directions);
    free(skane);
    return NULL;
  }

  /* Initialization */
  /* sprites */
  skane->ska_sprt = *ska_sprt;

  /* speeds */
  skane->obj->speed_x = 0;
  skane->obj->speed_y = 0;     // current speed
  skane->s            = speed; // saved max speed

  /* other stats */
  skane->health  = health; // skane's health (total length)
  skane->damage  = damage; // skane's missle dmg
  skane->fire_cd = MIS_CD;

  /* sizes */
  skane->mis_offset = ska_sprt->m_sprite.Height / 2.0;
  skane->cell_size  = DFLT_C_SIZE;

  /* positions */
  skane->changed_direc = false;
  skane->draw_direc    = STOP;
  skane->curr_state    = STOP; // skane starts stopped
  /* initial body segment */
  seg* temp_seg = (seg*)malloc(sizeof(seg));
  if (!temp_seg) {
    free_vector(skane->directions);
    free(skane);
    return NULL;
  }

  temp_seg->len = health;
  temp_seg->dir = N;
  vector_push_back(skane->directions, (void*)temp_seg);
  skane->collision_direc = STOP;
  skane->has_col_skane   = 0;

  /* head initial position */
  skane->obj->x  = x;
  skane->obj->y  = y;
  skane->offsetx = 0;
  skane->offsety = 0;

  /* tail initial position */
  skane->t_x = x;
  skane->t_y = y + 2 * skane->cell_size;

  /* id */
  static size_t curr_id       = 1;
  skane->obj->identifier.id   = curr_id;
  skane->obj->identifier.type = SKANE;
  ++curr_id;

  /* Create skane's body obj to write in col_matrix */
  Skane_Body_t* ska_body = (Skane_Body_t*)malloc(sizeof(Skane_Body_t));
  if (!ska_body) {
    free_vector(skane->directions);
    free(skane);
    free(temp_seg);
    return NULL;
  }
  Object_t* body_obj = new_object(0, 0, 0, 0, NULL);
  if (!body_obj) {
    free_vector(skane->directions);
    free(skane);
    free(temp_seg);
    free(ska_body);
    return NULL;
  }
  ska_body->obj                  = body_obj;
  ska_body->ska                  = skane;
  ska_body->vtable               = NULL;
  ska_body->obj->identifier.id   = skane->obj->identifier.id;
  ska_body->obj->identifier.type = SKANE_BODY;
  skane->ska_body                = ska_body;

  skane->vtable = &skane_vtable;
  return skane;
}

void
skane_mov(Skane_t* ska, input_array_t input_array)
{
  /* static const direc skane_ev_mtr[][8] = { */
  /* [>d,    w,    a,    s,    bd,   bw,   ba,   bs  <] */
  /* { E,    NE,   STOP, SE,   STOP, E,    E,    E },    // (E)ast */
  /* { NE,   N,    NW,   STOP, N,    STOP, N,    N },    // (N)orth */
  /* { STOP, NW,   W,    SW,   W,    W,    STOP, W },    // (W)est */
  /* { SE,   STOP, SW,   S,    S,    S,    S,    STOP }, // (S)outh */
  /* { NE,   NE,   N,    E,    N,    E,    NE,   NE },   // (N)orth(E)ast */
  /* { N,    NW,   NW,   W,    NW,   W,    N,    NW },   // (N)orth(W)est */
  /* { SE,   E,    S,    SE,   S,    SE,   SE,   E },    // (S)outh(E)ast */
  /* { S,    W,    SW,   SW,   SW,   SW,   S,    W },    // (S)outh(W)est */
  /* { E,    N,    W,    S,    STOP, STOP, STOP, STOP }  // STOP */
  /* }; */

  ska->curr_state = STOP;

  if (input_array[d])
    ska->curr_state += E;
  if (input_array[w])
    ska->curr_state += N;
  if (input_array[a])
    ska->curr_state += W;
  if (input_array[s])
    ska->curr_state += S;
}

void
skane_nom(Skane_t* ska, uint16_t nourishment)
{
  /* reduce speed when getting bigger */
  /* if (ska->s > 0.5) { */
  /* ska->s -= 0.1; */
  /* } */
  // TODO reducing speed will require some revamping of skane movement
  /* ska->s += 0.1; */

  /* get bigger */
  ((seg*)vector_end(ska->directions))->len += nourishment;
  ska->health += nourishment;
}

static int
skane_unom(Skane_t* ska, uint8_t damage)
{
  if (damage > ska->health - 1)
    return 1; // returns 1 when the damage takes exceeds the skane's health

  ska->health -= damage;
  for (uint8_t i = damage; i; --i) {
    seg* temp_seg = (seg*)vector_end(ska->directions);
    if (!temp_seg) {
      warn("%s: Failed getting skane segment", __func__);
      return 0;
    }

    if (temp_seg->len > i) {
      temp_seg->len -= i;
      break;
    }
    else if (temp_seg->len == i) {
      vector_pop_and_free(ska->directions);
      break;
    }
    else { // temp_seg->len < i
      i -= temp_seg->len;
      ++i; // couteract the for loop decrement
      vector_pop_and_free(ska->directions);
    }
  }

  return 0;
}

void
skane_take_damage(Skane_t* ska, uint8_t damage)
{
  /* we become smaller if we take damage */
  if (skane_unom(ska, damage)) {
    ska->obj->identifier.id = 0; // tell the game the skane died
    return;
  }
}

Missle_t*
fire_missle(Skane_t* ska, float fx, float fy)
{
  /* check if skane can shoot */
  if (ska->fire_cd)
    return NULL;

  /* shooting damages skane */
  if (skane_unom(ska, SKA_SHOOTING_COST))
    return NULL; // can't shoot if doesn't have enought health (would kys)

  /* spawn and fire a missle */
  Missle_t* m;
  float del_x = fx - ska->obj->x - ska->mis_offset;
  float del_y = fy - ska->obj->y - ska->mis_offset;

  if (del_x || del_y) {
    float norm = sqrt(del_x * del_x + del_y * del_y);

    float v_x = (float)del_x / norm * (MIS_SPEED + ska->ediff->shotup);
    float v_y = (float)del_y / norm * (MIS_SPEED + ska->ediff->shotup);

    m = new_missle(v_x,
                   v_y,
                   ska->obj->x,
                   ska->obj->y,
                   &ska->ska_sprt.m_sprite,
                   ska->damage,
                   ska->obj->identifier.id);
  }
  else {
    /* if the cursor is right on top of the skane's head coordinates (top
     * left corner of the skane's head sprite), we fire horizontally, to the
     * left. This fixes the problems with division by 0 */
    m = new_missle(-MIS_SPEED - ska->ediff->shotup,
                   0,
                   ska->obj->x,
                   ska->obj->y,
                   &ska->ska_sprt.m_sprite,
                   ska->damage,
                   ska->obj->identifier.id);
  }

  if (!m)
    return NULL;

  /* success so set shooting on cooldown */
  ska->fire_cd = MIS_CD - ska->ediff->shots;
  return m;
}

bool
skane_can_shoot(Skane_t* ska)
{
  return !ska->fire_cd;
}

bool
skane_just_shot(Skane_t* ska)
{
  return ska->fire_cd == (MIS_CD - ska->ediff->shots - 1);
}

void
skane_diff(Skane_t* ska, bool reset)
{
  /* reset difficulty (end run) */
  if (reset) {
    /* restore palette */
    if (set_color_palette_file(make_path(DFLT_PALLETE_FILE)))
      die("%s: Couldn't read/set info from given color palette file", __func__);

    ska->ediff->gsize  = 0;
    ska->ediff->es     = 0;
    ska->ediff->shots  = 0;
    ska->ediff->shotup = 0;
    return;
  }

  /* skane lvl up (new level) */
  if (ska->ediff->gsize >= ENE_GRP_LIM) {
    ska->ediff->gsize = 0;
    ska->ediff->shotup += MIS_UP_SCALE;
    if ((ska->ediff->shots += MIS_SPEED_SCALE) >= MIS_CD)
      ska->ediff->shots -= MIS_SPEED_SCALE;
    return;
  }

  /* new round */
  // randomize color palette each round (doesn't randomize on first round)
  if (ska->ediff->es && YOUGOTEPILEPSY < 256)
    set_random_color_palette(255, YOUGOTEPILEPSY);

  /* scale enemies' group size and speed */
  ska->ediff->es += ENE_SPEED_SCALE;
  ska->ediff->gsize += ENE_GRP_SCALE;
}
