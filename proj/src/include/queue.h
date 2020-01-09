/** @file queue.h */
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** @addtogroup	util_grp
 * @{
 */

/** A queue node */
typedef struct node Node_t;

/** @struct QUEUE_T
 *  A queue object
 */
typedef struct QUEUE_T
{
  Node_t* back;  /** previous node */
  Node_t* front; /** next node */
  size_t size;   /** number of nodes in the queue */
} Queue_t;

/**
 * @brief Creates a new queue object.
 * @return  Pointer to the instanciated queue object, on success\n
 *          NULL, otherwise.
 */
Queue_t* new_queue(void);

/**
 * @brief Checks if a given queue is empty.
 *
 * @param queue Queue to check.
 *
 * @return  True, if it is empty\n
 *          False, otherwise.
 */
bool queue_empty(Queue_t* queue);

/**
 * @brief Push element into queue.
 *
 * @param queue Queue object to push the element into.
 * @param data  Data of the element to push (node with it is created).
 */
void queue_push(Queue_t* queue, uint8_t data);

/**
 * @brief Pop element from given queue.
 *
 * @param queue Queue to pop the element from.
 */
void queue_pop(Queue_t* queue);

/**
 * @brief Get data of the next queue element.
 *
 * @param queue Queue to get the element from.
 *
 * @return The data of the element.
 */
uint8_t queue_front(Queue_t* queue);

/**
 * @brief Fully free and erase a queue.
 *
 * @param queue Queue to delete.
 */
void queue_delete(Queue_t* queue);

/** @} */

#endif // __QUEUE_H__
