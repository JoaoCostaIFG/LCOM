#include "include/queue.h"

#include <stdlib.h>

struct node
{
  uint8_t data;
  struct node* next;
};

Queue_t*
new_queue()
{
  Queue_t* queue = (Queue_t*)malloc(sizeof(Queue_t));
  if (!queue)
    return NULL;

  queue->back  = NULL;
  queue->front = NULL;
  queue->size  = 0;

  return queue;
}

bool
queue_empty(Queue_t* queue)
{
  return queue->front == NULL;
}

void
queue_push(Queue_t* queue, uint8_t data)
{
  Node_t* node = (Node_t*)malloc(sizeof(Node_t));

  if (!node)
    return;

  node->data = data;
  node->next = NULL;

  if (queue->back != NULL)
    queue->back->next = node;

  queue->back = node;

  if (queue_empty(queue))
    queue->front = node;

  ++queue->size;
}

void
queue_pop(Queue_t* queue)
{
  if (!queue_empty(queue)) {
    if (queue->back == queue->front) // If queue only has one element
      queue->back = NULL;

    Node_t* toDel = queue->front;
    queue->front  = queue->front->next;
    free(toDel);

    --queue->size;
  }
}

uint8_t
queue_front(Queue_t* queue)
{
  return queue->front->data;
}

void
queue_delete(Queue_t* queue)
{
  while (!queue_empty(queue))
    queue_pop(queue);
}
