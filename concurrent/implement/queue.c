//
// Created by jjj on 2023/2/12.
//

#include <stdatomic.h>

#include "queue.h"

#define compare_exchange(PTR, VAL, DES)  atomic_compare_exchange_strong(PTR, VAL, DES)


typedef struct tagConcurrentQueueNode ConNode;

typedef struct tagConcurrentQueue ConQueue;

ConNode *NullNode = NULL;

void ConcurrentQueueInit(ConQueue *queue)
{
    queue->head.next = &queue->head;
    queue->head.prev = &queue->head;
    queue->size = 0;
}

uint32_t ConcurrentQueueSize(ConQueue *queue)
{
    return atomic_load(&queue->size);
}

void ConcurrentQueueIn(ConQueue *queue, ConNode *insert)
{
    ConNode *prev = &queue->head;

    __auto_type *next = atomic_exchange(&prev->next, insert);
    atomic_store(&insert->next, next);

    compare_exchange(&insert->prev, &NullNode, prev);
    atomic_store(&next->prev, insert);

    atomic_fetch_add(&queue->size, 1);
}

ConNode *ConcurrentQueueOut(ConQueue *queue)
{
    ConNode *prev = &queue->head;

    ConNode *node, *next;
    do {
        node = atomic_load(&prev->next);
        next = atomic_load(&node->next);
    } while (compare_exchange(&prev->next, &node, next));
    next->prev = prev;
    atomic_store(&next->prev, prev);

    atomic_fetch_sub(&queue->size, 1);

    node->next = NULL;
    node->prev = NULL;
    return NULL;
}

