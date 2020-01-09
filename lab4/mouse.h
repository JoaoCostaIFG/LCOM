#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>

/* enum: transition
 * DES_TOL:  0
 * DES_NTOL: 1
 * ASC_TOL:  2
 * ASC_NTOL: 3
 * LCLK:     4
 * RCLK:     5
 * LRE:      6
 * RRE:      7
 * FAIL:     8
 */
typedef enum {
	DES_TOL,
	DES_NTOL,
	ASC_TOL,
	ASC_NTOL,
	LCLK,
	RCLK,
	LRE,
	RRE,
	FAIL
} transitions_enum;

/* enum: state_enum
 * INIT:	0
 * SLOPE_LEFT:  1
 * VERTICE:	2
 * SLOPE_RIGHT: 3
 * ACCEPT:	4
 */
typedef enum {
	INIT,
	SLOPE_LEFT,
	VERTICE,
	SLOPE_RIGHT,
	ACCEPT
} state_enum;

void (mouse_ih)();

void (timer_int_handler)();

transitions_enum transition_calc(struct packet *pp, struct packet *pp_old, uint8_t x_len, int *curr_x, uint8_t tolerance);

void (event_ih)(struct packet *pp, struct packet *pp_old, state_enum *curr_state, uint8_t x_len, int *curr_x, uint8_t tolerance);

int mouse_sync_packet(uint8_t *curr_byte, struct packet *pp, uint32_t *cnt);

int mouse_iloop(const int *irq_sel, uint32_t cnt);

int mouse_packet(uint32_t *cnt);

int mouse_remote_poll(uint16_t *period, uint8_t cnt);

int mouse_remote(uint16_t *period, uint8_t *cnt);

int mouse_async_iloop(const int *mou_irq_sel, const int *timer0_irq_sel, uint8_t *idle_time);

int mouse_async(uint8_t *idle_time);

int mouse_gesture_iloop(const int *irq_sel, uint8_t *x_len, uint8_t *tolerance);

int mouse_gesture(uint8_t *x_len, uint8_t *tolerance);

#endif  // _LCOM_MOUSE_H_
