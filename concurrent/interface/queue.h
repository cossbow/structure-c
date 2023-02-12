//
// Created by jjj on 2023/2/12.
//

#ifndef STRUCTURE_QUEUE_H
#define STRUCTURE_QUEUE_H

#include <stdint.h>

typedef struct tagConcurrentQueueNode {
    struct tagConcurrentQueueNode *prev, *next;
} ConcurrentQueueNode;

typedef struct tagConcurrentQueue {
    ConcurrentQueueNode head;
    uint32_t size;
} ConcurrentQueue;

uint32_t ConcurrentQueueSize(ConcurrentQueue *queue);

void ConcurrentQueueIn(ConcurrentQueue *queue, ConcurrentQueueNode *insert);

ConcurrentQueueNode *ConcurrentQueueOut(ConcurrentQueue *queue);


#endif //STRUCTURE_QUEUE_H
