#include "queue.h"

struct queue {
    QueueNode *head;
    QueueNode *tail;
    uint32_t size;
    Destructor destr_func;
};

struct queue_node {
    QueueNode *next;
    void *value;
};

Queue *queue_create(Destructor destr_func) {
    Queue *queue = calloc(1, sizeof(*queue));
    queue->destr_func = destr_func;
    return queue;
}

uint32_t queue_size(Queue *queue) {
    return queue->size;
}

void queue_insert(Queue *queue, void *value) {
    QueueNode *node = calloc(1, sizeof(*node));
    node->value = value;

    if (queue->size == 0) {
        queue->head = queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->size++;
}

void queue_delete(Queue *queue) {
    if (queue->size == 0)
        return;
    
    QueueNode *temp = queue->head;
    queue->head = queue->head->next;
    queue->size--;

    if (queue->size == 1)
        queue->tail = queue->head;
    
    if (queue->destr_func != NULL)
        queue->destr_func(temp->value);
    free(temp);
}

QueueNode *queue_head(Queue *queue) {
    return queue->size > 0 ? queue->head : NULL;
}

QueueNode *queue_next(QueueNode *node) {
    return node != NULL ? node->next : NULL;
}

void *queue_value(QueueNode *node) {
    return node != NULL ? node->value : NULL;
}

void queue_destroy(Queue *queue) {
    while (queue->size > 0)
        queue_delete(queue);
    
    free(queue);
}

