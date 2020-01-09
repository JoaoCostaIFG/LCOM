#include <lcom/lcf.h>
#include <stdlib.h>

#include "include/err_utils.h"
#include "include/queue.h"
#include "include/serial.h"
#include "include/utils.h"

#define POLL_WAIT  20
#define POLL_TRIES 800

/* QUEUE */
static bool can_transmit = true;
static Queue_t* receive_queue;
static Queue_t* send_queue;

void
serial_receive_delete()
{
  queue_delete(receive_queue);
}

void
serial_send_delete()
{
  queue_delete(send_queue);
}

bool
serial_can_transmit()
{
  return can_transmit;
}

bool
serial_receive_empty()
{
  return queue_empty(receive_queue);
}

void
serial_receive_pop()
{
  queue_pop(receive_queue);
}

uint8_t
serial_receive_front()
{
  return queue_front(receive_queue);
}

uint8_t
serial_receive_read()
{
  uint8_t data = queue_front(receive_queue);
  queue_pop(receive_queue);

  return data;
}

float
serial_receive_read_float()
{
  float2uint32 temp;

  temp.i = serial_receive_read() << 24;
  temp.i += serial_receive_read() << 16;
  temp.i += serial_receive_read() << 8;
  temp.i += serial_receive_read();

  return temp.f;
}

size_t
serial_receive_size(void)
{
  return receive_queue->size;
}

bool
serial_send_empty()
{
  return queue_empty(send_queue);
}

void
serial_send_push(uint8_t data)
{
  queue_push(send_queue, data);
}

void
serial_send_push_int(uint32_t data)
{
  if (data == 0) { // still need to send padding
    queue_push(send_queue, 0);
    queue_push(send_queue, 0);
    queue_push(send_queue, 0);
    queue_push(send_queue, 0);
    return;
  }

  queue_push(send_queue, (data & 0xFF000000) >> 24);
  queue_push(send_queue, (data & 0xFF0000) >> 16);
  queue_push(send_queue, (data & 0xFF00) >> 8);
  queue_push(send_queue, (data & 0xFF));
}

void
serial_send_push_float(float data)
{
  float2uint32 temp;
  temp.f = data;

  serial_send_push_int(temp.i);
}

void
serial_send()
{
  if (can_transmit) {
    sys_outb(COM1_BASEADDR + UART_THR, queue_front(send_queue));
    queue_pop(send_queue);
  }

  can_transmit = false;
}

/* void */
/* serial_send_force() */
/* { */
/* sys_outb(COM1_BASEADDR + UART_THR, queue_front(send_queue)); */
/* queue_pop(send_queue); */
/* } */

void
serial_send_force(uint8_t data)
{
  sys_outb(COM1_BASEADDR + UART_THR, data);
}

int
serial_send_all()
{
  uint8_t lsr;
  util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
  can_transmit = (lsr & LSR_TRAHOLD);

  while (!queue_empty(send_queue) && can_transmit) {
    sys_outb(COM1_BASEADDR + UART_THR, queue_front(send_queue));
    queue_pop(send_queue);

    util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
    can_transmit = (lsr & LSR_TRAHOLD);
  }

  return !(queue_empty(send_queue));
}

/* INTERRUPT HANDLER HELPER FUNCTIONS */
static inline int
serial_check_lsr(void)
{
  uint8_t lsr;
  if (util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr)) {
    warn("%s: line status reading failed", __func__);
    return 0;
  }

  /* errors */
  if (lsr & LSR_FIFOERR) { // FIFO is not reliable anymore
    serial_clear_rcvrfifo();
  }
  else if (lsr & (LSR_OVRERR | LSR_PARERR | LSR_FRERR)) {
    // get rid of corrupt data
    uint8_t data;
    util_sys_inb(COM1_BASEADDR + UART_RBR, &data);
  }

  can_transmit = (lsr & LSR_TRAHOLD || lsr & LSR_EMPTYREG);

  return (lsr & LSR_DATA);
}

static void
serial_get_data(void)
{
  uint8_t data;

  while (serial_check_lsr()) {
    /* get data */
    if (util_sys_inb(COM1_BASEADDR + UART_RBR, &data)) {
      die("%s: received data reading failed", __func__);
      return;
    }

    queue_push(receive_queue, data);
  }
}

/* INTERRUPT HANDLER */
void
serial_ih(void)
{
  uint8_t idint;
  util_sys_inb(COM1_BASEADDR + UART_IIR, &idint);

  while (!(idint & IIR_NOINT)) {
    if (idint & IIR_RCVRDATA || idint & IIR_FIFO) {
      serial_get_data();
    }
    else if (idint & IIR_TRANSHOLD) {
      can_transmit = true;
      if (!queue_empty(send_queue))
        serial_send_all();
    }
    else if (idint & IIR_LINEST_1 && idint & IIR_LINEST_2) {
      uint8_t lsr;
      util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);

      /* errors */
      if (lsr & LSR_FIFOERR) // FIFO is not reliable anymore
        serial_clear_rcvrfifo();
      else if (lsr & (LSR_OVRERR | LSR_PARERR |
                      LSR_FRERR)) { // get rid of corrupt data
        // error_found = true;
      }

      /* can send data */
      if (lsr & (LSR_TRAHOLD | LSR_EMPTYREG)) { // can send data
        can_transmit = true;
        if (!queue_empty(send_queue))
          serial_send_all();
      }
    }
    else if (!(idint & IIR_MODEMST)) {
      uint8_t msr;
      if (util_sys_inb(COM1_BASEADDR + UART_MSR, &msr))
        warn("Serial port modem status reading failed");
      else
        warn("Modem status: %X", msr);
    }

    util_sys_inb(COM1_BASEADDR + UART_IIR, &idint);
  }
}

/* INTERRUPT ENABLE REGISTER */
int
serial_en_dataint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf |= IER_DATAINT;
  receive_queue = new_queue();

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_dis_dataint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf &= ~IER_DATAINT;

  // queue_delete(receive_queue);

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_en_traholdint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf |= IER_TRAHOLDINT;
  send_queue = new_queue();

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_dis_traholdint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf &= ~IER_TRAHOLDINT;

  ////queue_delete(send_queue);

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_en_linestint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf |= IER_LINESTINT;

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_dis_linestint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf &= ~IER_LINESTINT;

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_en_modemint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf |= IER_MODEMINT;

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

int
serial_dis_modemint(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_IER, &curr_conf))
    return 1;

  curr_conf &= ~IER_MODEMINT;

  return sys_outb(COM1_BASEADDR + UART_IER, curr_conf);
}

/* LINE CONTROL REGISTER */
int
serial_8bpc(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf |= LCR_8BPC;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_2stopbits(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf |= LCR_2STOPB;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_1stopbits(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf &= ~LCR_2STOPB;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_oddparity(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf &= ~LCR_0PAR;
  curr_conf |= LCR_ODDPAR;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_noparity(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf &= LCR_NOPAR;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_BRKCTRL(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf |= LCR_BREAKCTRL;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_noBRKCTRL(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf &= ~LCR_BREAKCTRL;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_open_divlatch(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf |= LCR_DIVLATCHACCESS;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

int
serial_close_divlatch(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_LCR, &curr_conf))
    return 1;

  curr_conf &= ~LCR_DIVLATCHACCESS;

  return sys_outb(COM1_BASEADDR + UART_LCR, curr_conf);
}

/* FIFO CONTROL REGISTER */
int
serial_enable_fifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf |= FCR_BOTHFIFO;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_disable_fifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf &= ~FCR_BOTHFIFO;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_clear_rcvrfifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf |= FCR_CLRRCVR;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_clear_xmitfifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf |= FCR_CLRXMIT;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_1bytetrigger(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf &= ~FCR_RCVRTRIG16;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_4bytetrigger(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf &= ~FCR_RCVRTRIG16;
  curr_conf |= FCR_RCVRTRIG4;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_8bytetrigger(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf &= ~FCR_RCVRTRIG16;
  curr_conf |= FCR_RCVRTRIG8;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_16bytetrigger(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf |= FCR_RCVRTRIG16;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_64byte_fifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf |= FCR_ENBL64;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

int
serial_set_16byte_fifo(void)
{
  uint8_t curr_conf;
  if (util_sys_inb(COM1_BASEADDR + UART_FCR, &curr_conf))
    return 1;

  curr_conf &= ~FCR_ENBL64;

  return sys_outb(COM1_BASEADDR + UART_FCR, curr_conf);
}

/* DIVISOR LATCH */
int
serial_set_maxrate(void)
{
  if (serial_open_divlatch())
    return 1;

  bool fail = false;
  if (sys_outb(COM1_BASEADDR + UART_DLM, 0))
    fail = true;
  if (sys_outb(COM1_BASEADDR + UART_DLL, 1))
    fail = true;

  if (serial_close_divlatch())
    fail = true;

  return fail;
}

/* RESTORE DEFAULT MINIX CONFIGS */
int
serial_restore_conf(void)
{
  bool fail = false;

  /* restore LCR */
  if (sys_outb(COM1_BASEADDR + UART_LCR, DFLT_LCR))
    fail = true;

  /* restore IER */
  if (sys_outb(COM1_BASEADDR + UART_IER, DFLT_IER))
    fail = true;

  /* FIFO defaults */
  if (serial_set_16byte_fifo() || serial_set_1bytetrigger())
    fail = true;

  /* restore DLM and DLL */
  if (serial_open_divlatch())
    fail = true;
  else {
    if (sys_outb(COM1_BASEADDR + UART_DLM, DFLT_DLM))
      fail = true;

    if (sys_outb(COM1_BASEADDR + UART_DLL, DFLT_DLL))
      fail = true;
  }
  if (serial_close_divlatch())
    fail = true;

  /* delete queues */
  if (send_queue)
    serial_send_delete();
  if (receive_queue)
    serial_receive_delete();

  return fail;
}

/* SYNC */
static int
serial_poll_send(uint8_t data)
{
  uint32_t tries = POLL_TRIES;
  uint8_t lsr;
  util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
  while (!(lsr & LSR_TRAHOLD) && tries) {
    /* wait until the transmiter is holding the register empty */
    tickdelay(micros_to_ticks(POLL_WAIT));
    --tries;
    util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
  }

  if (lsr & LSR_TRAHOLD) { // ready to send
    if (sys_outb(COM1_BASEADDR + UART_THR, data))
      return 1;
  }
  else
    return 1;

  return 0;
}

static int
serial_poll_receive(uint8_t* data)
{
  uint32_t tries = POLL_TRIES;
  uint8_t lsr;
  util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
  while (!(lsr & LSR_DATA) && tries) {
    /* wait until the transmiter sends data */
    tickdelay(micros_to_ticks(POLL_WAIT));
    --tries;
    util_sys_inb(COM1_BASEADDR + UART_LSR, &lsr);
  }

  if (lsr & LSR_DATA) { // ready to receive
    if (util_sys_inb(COM1_BASEADDR + UART_RBR, data))
      return 1;
  }
  else
    return 1;

  return 0;
}

int
serial_handshake()
{
  /* send ready packet */
  if (serial_poll_send(HTCHECK + SYNC_RDY)) {
    warn("%s: failed sending RDY packet", __func__);
    return -1;
  }

  /* get ready packet */
  uint8_t data;
  if (serial_poll_receive(&data)) {
    warn("%s: failed receiving packet", __func__);
    return -1;
  }

  if (data == HTCHECK + SYNC_RDY) { // I'm the first player
    /* got RDY, answer with OK */
    if (serial_poll_send(HTCHECK + SYNC_OK)) {
      warn("%s: failed sending OK packet", __func__);
      return -1;
    }

    /* get OK packet */
    if (serial_poll_receive(&data)) {
      warn("%s: failed receiving packet", __func__);
      return -1;
    }
    if (data == HTCHECK + SYNC_OK) {
      return 0; // we are 1st player successfully
    }
    else
      return -1;
  }
  else if (data == HTCHECK + SYNC_OK) { // I'm the second player
    if (serial_poll_send(HTCHECK + SYNC_OK)) {
      warn("%s: failed sending OK packet", __func__);
      return -1;
    }

    return 1; // we are 2nd player successfully
  }

  return -1;
}
