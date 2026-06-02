#ifndef QUEUE_H
#define QUEUE_H

#include "../../def.h"

typedef struct queue{
    uint8_t* buffer;
    volatile uint32_t head;
    volatile uint32_t tail;
    uint32_t capacity;
}queue_t;

typedef enum{
    QUEUE_CAP_1 = 0,
    QUEUE_CAP_2,
    QUEUE_CAP_4,
    QUEUE_CAP_8,
    QUEUE_CAP_16,
    QUEUE_CAP_32,
}queue_cap_t;


void queue_init(queue_t* self, uint8_t* buffer, queue_cap_t capacity);
__bool queue_is_empty(queue_t* self);
__bool queue_enqueue(queue_t* self, uint8_t data);
__bool queue_dequeue(queue_t* self, uint8_t* out_data);
__bool queue_is_full(queue_t* self);
queue_cap_t queue_get_capacity(queue_t* self);



#endif