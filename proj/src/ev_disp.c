#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/bmp.h"
#include "include/err_utils.h"
#include "include/ev_disp.h"
#include "include/kbd.h"
#include "include/menu.h"
#include "include/mouse.h"
#include "include/obj_handle.h"
#include "include/rtc.h"
#include "include/serial.h"
#include "include/timer.h"
#include "include/utils.h"
#include "include/vg.h"

static int hook_ids[]      = { 0, 0, 0, 0, 0 };
static gamestate gamest    = MENUST;
static uint16_t video_mode = DFLT_VIDEO_MODE;
char respath[PATH_MAXSIZE];

// Nem toda a gente vive no teu retard :( . Tabém?¿?

/* GAME LOCAL UTILITY FUNCTIONS */
static inline void
update(void)
{
  clear_collision_matrix();
  update_objs_collisions();
  calc_objs_pos();
  render_objects();
  /* debug_collisions(); */ // TODO collision are delayed 1 frame (for skane)

  next_buff();
  if (garbage_collector()) // cull dead objects
    exit_to_main_menu();   // a Skane died
}

/* GAME FUNCTIONS */
/* SETTERS */
void
set_resourcepath(const char* const path)
{
  strcpy(respath, path);
}

void
set_videomode(const uint16_t mode)
{
  video_mode = mode;
}

/* COMMUNICATION HANDLING */
static void
reshake(void)
{
  /* empty out our sending queue */
  uint8_t tries = 5;
  while (!serial_send_empty() && tries) {
    serial_send_all();
    --tries;
  }
  if (!serial_send_empty()) // if still not empty, going to lose data
    serial_clear_xmitfifo();

  /* shake hands again */
  if (serial_handshake() == -1)
    die("%s: The begin game handshake failed.", __func__);

  /* enable interrupt types we want */
  serial_en_dataint();
  serial_en_traholdint();
  /* serial_en_linestint(); */

  // queue first end frame packet for sending
  serial_send_push(HTCHECK + SERIAL_SYNC_PACK);
}

static bool
multiplayer_handshake(void)
{
  /* subscribe serial interrupts */
  hook_ids[4] = COM1_IRQ;
  if (subscribe_int(&hook_ids[4], COM1_IRQ, true))
    die("%s: Couldn't subscribe Serial port interrupts.", __func__);

  /* config serial */
  serial_8bpc();
  serial_1stopbits();
  serial_noparity();
  serial_set_maxrate();
  serial_enable_fifo();
  serial_set_1bytetrigger();
  serial_set_64byte_fifo();
  serial_clear_rcvrfifo();
  serial_clear_xmitfifo();

  /* disable interrupts */
  serial_dis_modemint();
  serial_dis_linestint();
  serial_dis_dataint();
  serial_dis_traholdint();

  /* draw(start_sing_menu); */
  /* sync game start */
  int handshake_answer = serial_handshake();
  if (handshake_answer == 0) // am player 1
    gamest = MULT1;
  else if (handshake_answer == 1) // am player 2
    gamest = MULT2;
  else if (handshake_answer == -1) { // failed handshake
    warn("%s: handshake failed", __func__);
    serial_restore_conf();
    return 1;
  }

  /* enable interrupt types we want */
  serial_en_dataint();
  serial_en_traholdint();
  /* serial_en_linestint(); */
  return 0; // successful handshake
}

static void
com_handler(void)
{
  /** This function will handle the communication and parsing of the information
   * between the 2 machines. It is expecting a sync byte at the end of
   * everyframe (handshake). If after enough tries/time, no sync packet is
   * parsed, it will assume the other player disconnected or had problems and
   * quit the game
   * This makes sure we don't get 2 states, for the same object, for the same
   * frame and that we apply the information to the correct frame
   */
  uint8_t packet_size, packet_type;
  uint32_t tries = SERIAL_SYNC_TRIES;

  while (tries) {
    /* try parsing a packet byte */
    if (serial_receive_empty()) {
      --tries; // decrease number of tries
      serial_ih();
      if (!serial_send_empty())
        serial_send_all();
      continue;
    }
    packet_type = serial_receive_front();

    /* verify header */
    switch (packet_type & ~SERIAL_SYNC_PACK_HT) {
      case HTCHECK + SERIAL_SYNC_PACK:
        serial_receive_pop();
        return;
      case HTCHECK + SERIAL_SKA_MOV:
        packet_size = SERIAL_SKA_MOV_S;
        break;
      case HTCHECK + SERIAL_SKA_MIS:
        packet_size = SERIAL_SKA_MIS_S;
        break;
      case HTCHECK + SERIAL_ENE_SPA:
        packet_size = SERIAL_ENE_SPA_S;
        break;
      case HTCHECK + SERIAL_DEATH_PACK:
        exit_to_main_menu();
        break;
      case HTCHECK + SERIAL_EMPTY_PACK:
      default: // Treat has empty packet
        serial_receive_pop();
        continue;
    }

    /* verify if we got a full packet */
    if (serial_receive_size() - 1 >= packet_size) {
      serial_receive_pop();

      switch (packet_type & ~SERIAL_SYNC_PACK_HT) {
        case HTCHECK + SERIAL_SKA_MOV:
          set_ska2_state((int8_t)serial_receive_read()); // direc is int32_t
          break;
        case HTCHECK + SERIAL_SKA_MIS:
          /* no need to check NULL pointer here because add_object does that */
          ska2_fire_missle(serial_receive_read_float(),
                           serial_receive_read_float());
          break;
        case HTCHECK + SERIAL_ENE_SPA:
          spawn_allies(serial_receive_read(), gamest);
          break;
        case HTCHECK + SERIAL_EMPTY_PACK:
          serial_receive_pop();
          break;
        default: // Treat has empty packet
          serial_receive_pop();
          continue;
      }

      if (packet_type & SERIAL_SYNC_PACK_HT) {
        return;
      } // frame ending packet
    }
    else {         // the full packet hasn't been sent yet
      serial_ih(); // try reading data again
      if (!serial_send_empty())
        serial_send_all();
    }
  }

  if (!tries) { // game hanged too long
    warn("Game hanged for too long! Assumed other player disconnected. "
         "Quitting...");
    exit_to_main_menu();
  }
}

/* MENU FUNCTIONS */
static void
start_game(rtctime_t* curr_time)
{
  alloc_obj_matrix();
  alloc_collison_matrix();
  /* gameplay */
  inst_skane(gamest);
  create_map(gamest);

  /* get current time */
  rtc_ih(); // clear possibly missed interrupts
  rtc_get_time(curr_time);

  /* handshake again */
  if (gamest == MULT1 || gamest == MULT2)
    reshake(); // agree on start

  /* enemy spawn */
  rtc_set_alarm_ff_curr(curr_time, ENEMY_SPAWN_RATE);
}

static inline void
start_main_menu(void)
{
  inst_menus();
  gamest = MENUST;
}

void
update_menu_collisions(rtctime_t* curr_time)
{
  /* Collision with singleplayer menu */
  if (get_cursor_x() > get_menu_x(get_sing_menu()) &&
      get_cursor_x() <
        get_menu_x(get_sing_menu()) + get_sing_menu()->obj->sprite.Width &&
      get_cursor_y() > get_menu_y(get_sing_menu()) &&
      get_cursor_y() <
        get_menu_y(get_sing_menu()) + get_sing_menu()->obj->sprite.Height) {
    delete_menus();
    gamest = SINGLE;
    start_game(curr_time);
  }
  else if (get_cursor_x() > get_menu_x(get_mult_menu()) &&
           get_cursor_x() <
             get_menu_x(get_mult_menu()) + get_mult_menu()->obj->sprite.Width &&
           get_cursor_y() > get_menu_y(get_mult_menu()) &&
           get_cursor_y() <
             get_menu_y(get_mult_menu()) + get_mult_menu()->obj->sprite.Height) {
    // spawn loading screen
    vg_clear_all();
    draw_loading_menu();
    next_buff();
    /* Serial port (for multiplayer) (exclusive) */
    if (!multiplayer_handshake()) {
      delete_menus();
      start_game(curr_time);
    }
    else {
      unsubscribe_int(&hook_ids[4]);
      /* Clear loading sprite */
      vg_clear_all();
    }
  }
  else if (get_cursor_x() > get_menu_x(get_exit_menu()) &&
           get_cursor_x() < get_menu_x(get_exit_menu()) + get_exit_menu()->obj->sprite.Width &&
           get_cursor_y() > get_menu_y(get_exit_menu()) &&
           get_cursor_y() < get_menu_y(get_exit_menu()) + get_exit_menu()->obj->sprite.Height) {
    delete_menus();
    die(DEATHTXT);
  }
}

void
exit_to_main_menu(void)
{
  destroy_all_objects();
  clear_collision_matrix();

  rtc_disable_alrm();
  if (gamest == MULT1 || gamest == MULT2) {
    serial_restore_conf();
    if (unsubscribe_int(&hook_ids[4]))
      warn("Couldn't unsubscribe serial port interrupts");
  }

  /* restore palette */
  if (set_color_palette_file(make_path(DFLT_PALLETE_FILE)))
    die("%s: Couldn't read/set info from given color palette file", __func__);

  vg_clear_all();
  start_main_menu();
}

/* MAINLOOP */
void
mainloop(void)
{
  /* driver recieve variables */
  int ipc_status, r;
  message msg;

  /* keyboard input */
  bool is_make_code;
  uint8_t kbd_bytes[2];
  /* mouse input */
  struct packet pp;
  memset(&pp, 0, sizeof(struct packet));
  /* game input */
  input_array_t input_array;
  memset(&input_array, 0, sizeof(input_array));
  rtctime_t curr_time;
  inst_cursor();

  start_main_menu();

  /* driver recieve loop */
  while (true) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) // get a request message
      warn("%s: driver_receive failed with: %d", __func__, r);

    else if (is_ipc_notify(ipc_status) &&
             (_ENDPOINT_P(msg.m_source) == HARDWARE)) {
      /* received notification */
      if (msg.m_notify.interrupts & BIT(MOU_IRQ)) { // MOUSE
        mouse_ih();                                 // mouse interrupt handler

        if (get_mbyte_valid()) {
          /* sync mouse packet's bytes for parsing */
          if (!mouse_sync_packet(&pp)) {
            /* Update cursor offset */
            update_cursor(pp.delta_x, pp.delta_y);
            input_array[lmb] = pp.lb;
            input_array[rmb] = pp.rb;
            input_array[mmb] = pp.mb;
          }
        }
      }
      else if (msg.m_notify.interrupts & BIT(KBD_IRQ)) { // KEYBOARD
        kbd_ih(); // keyboard interrupt handler

        if (get_kbyte_valid() && !kbd_scancode(&is_make_code, kbd_bytes)) {
          /* get pressed key */
          input_array[INPUT_HASH(kbd_bytes)] = is_make_code;
        }
      }
      else if (msg.m_notify.interrupts & BIT(TIMER0_IRQ)) { // TIMER0
        timer_ih(); // timer interrupt handler

        if (gamest != MENUST) {
          /* Handle missile fire */
          if (input_array[lmb])
            ska1_fire_missle();
          /* move skane */
          ska1_mov(input_array);

          /* get info */
          if (gamest == MULT1 || gamest == MULT2) {
            /* get and parse info */
            com_handler();
            if (gamest == MENUST) // If com handler quit the game, exit
              continue;
          }

          /* call update method */
          update();

          /* transmit info */
          if (gamest == MULT1 || gamest == MULT2) {
            if (transmit_skane_info()) // queue end frame packet for sending (if
                                       // no info sent)
              serial_send_push(HTCHECK + SERIAL_SYNC_PACK);
            /* transmit */
            serial_send_all();
          }

          /* Quit to main menu */
          if (input_array[ESC])
            exit_to_main_menu();
        }
        else {
          draw_menus();
          draw_cursor();
          next_buff();
          if (input_array[lmb]) {
            update_menu_collisions(&curr_time);
          }
        }
      }
      else if (msg.m_notify.interrupts & BIT(RTC_IRQ)) { // RTC
        warn("fcking RTC");
        rtc_ih(); // RTC interrupt handler
        uint8_t curr_creg = rtc_get_creg();

        /* there can be multiple interrupts at the same time */
        if (gamest != MENUST && curr_creg & RTC_AF) {
          /* spawn enemy and handle cooldown */
          spawn_enemy(gamest);
          /* set alarm for next enemy spawn */
          rtc_set_alarm_ff_curr(&curr_time, ENEMY_SPAWN_RATE);
        }
      }
      else if (msg.m_notify.interrupts & BIT(COM1_IRQ)) { // SERIAL PORT
        serial_ih();
      }
    }
  }
}

static void
sub_interrupts(void)
{
  // if(lcom) panic()
  /* mouse (exclusive) */
  hook_ids[0] = MOU_IRQ;
  if (subscribe_int(&hook_ids[0], MOU_IRQ, true))
    die("%s: Couldn't subscribe mouse interrupts.", __func__);

  /* keyboard (exclusive) */
  hook_ids[1] = KBD_IRQ;
  if (subscribe_int(&hook_ids[1], KBD_IRQ, true))
    die("%s: Couldn't subscribe keyboard interrupts.", __func__);

  /* timer0 (non-exclusive) */
  hook_ids[2] = TIMER0_IRQ;
  if (subscribe_int(&hook_ids[2], TIMER0_IRQ, false))
    die("%s: Couldn't subscribe timer0 interrupts.", __func__);

  /* RTC (for alarms) (exclusive) */
  hook_ids[3] = RTC_IRQ;
  if (subscribe_int(&hook_ids[3], RTC_IRQ, false))
    die("%s: Couldn't subscribe RTC interrupts.", __func__);
}

void
init(void)
{
  /* clear log file contents */
  clrlogs();

  /* set default resources path if none is set */
  if (strlen(respath) == 0)
    strcpy(respath, DFLT_RESPATH);

  /* initialize random seed */
  srand(time(NULL));

  /* set alarm for enemy spawn */
  rtc_disable_alrm();
  rtc_ih(); // clear possible missed interrupts
  /* mouse setup */
  if (mouse_en_data_report())
    warn("Mouse data reporting enabling failed");

  /* subscribe interupts */
  sub_interrupts();

  /* video mode stuff */
  if (!vginit(video_mode, true))
    die("%s: Couldn't initialize video mode", __func__);

  vg_clear_all(); // initialize all video buffers to 0

  if (set_truecolor())
    warn("%s: Couldn't set truecolor mode", __func__);

  if (set_color_palette_file(make_path(DFLT_PALLETE_FILE)))
    die("%s: Couldn't read/set info from given color palette file", __func__);

  /* call the game main loop */
  mainloop();
}

void
quit(void)
{
  if (gamest != MENUST)
    destroy_all_objects();

  /* unsubscribe mouse interrupts */
  mouse_set_stream_mode();
  if (unsubscribe_int(&hook_ids[0]))
    warn("%s: Couldn't unsubscribe mouse interrupts.", __func__);

  /* unsubscribe keyboard interrupts */
  kbd_restore_conf();
  if (unsubscribe_int(&hook_ids[1]))
    warn("%s: Couldn't unsubscribe keyboard interrupts.", __func__);

  /* unsubscribe timer0 interrupts */
  if (unsubscribe_int(&hook_ids[2]))
    warn("%s: Couldn't unsubscribe timer0 interrupts.", __func__);

  /* unsubscribe RTC interrupts */
  rtc_disable_all_int();
  rtc_allow_update();
  if (unsubscribe_int(&hook_ids[3]))
    warn("%s: Couldn't unsubscribe RTC interrupts.", __func__);

  /* unsubscribe Serial port interrupts */
  if (gamest == MULT1 || gamest == MULT2) {
    if (unsubscribe_int(&hook_ids[4]))
      warn("%s: Couldn't unsubscribe Serial port interrupts.", __func__);

    /* clear queues */
    serial_clear_rcvrfifo();
    serial_clear_xmitfifo();

    /* send death packet for safety/game quits */
    serial_send_push(HTCHECK + SERIAL_DEATH_PACK);
    serial_send();
    serial_restore_conf();
  }
  /* return to text mode */
  if (vg_exit())
    warn("%s: Couldn't set text mode correctly.", __func__);

  byebye();
  warn("Exit success!");
  exit(0);
}
