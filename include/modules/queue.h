#ifndef QUEUE_H
#define QUEUE_H

#include "common_types.h"

typedef struct queue Queue;
typedef struct queue_node QueueNode;

Queue *queue_create(Destructor destr_func);

uint32_t queue_size(Queue *queue);

void queue_insert(Queue *queue, void *value);

void queue_delete(Queue *queue);

QueueNode *queue_head(Queue *queue);

QueueNode *queue_next(QueueNode *node);

void *queue_value(QueueNode *node);

void queue_destroy(Queue *queue);


#endif /* QUEUE_H */

