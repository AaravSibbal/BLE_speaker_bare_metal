#ifndef QUEUE_H
#define QUEUE_H

#include "../../def.h"
#include <stdint.h>

typedef struct queue queue_t;

typedef enum{
    QUEUE_CAP_2 = 0,
    QUEUE_CAP_4,
    QUEUE_CAP_8,
    QUEUE_CAP_16,
    QUEUE_CAP_32,
}queue_cap_t;


queue_t* queue_init(uint8_t* buffer, queue_cap_t capacity, I2C_instance_t i2c_instance);
__bool queue_is_empty(queue_t* self);
__bool queue_enqueue(queue_t* self, uint8_t data);
__bool queue_dequeue(queue_t* self, uint8_t* out_data);
__bool queue_is_full(queue_t* self);



#endif