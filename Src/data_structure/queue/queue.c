#include "queue.h"

#include "../../peripherals/I2C/I2C.h"
#include "../../assert.h"
#include "Src/arm/arm.h"
#include "Src/assert.h"
#include "Src/def.h"


struct queue{
    uint8_t* buffer;
    volatile uint32_t head;
    volatile uint32_t tail;
    uint32_t capacity;
};

static queue_t i2c1_queue;
static queue_t i2c2_queue;
static queue_t i2c3_queue;

queue_t* queue_init(uint8_t* buffer, queue_cap_t capacity, I2C_instance_t i2c_instance){
    if(buffer == NULL){
        return NULL;
    }

    queue_t* queue;
    switch (i2c_instance) {
        case I2C_1:
            queue = &i2c1_queue;
            break;
        case I2C_2:
            queue = &i2c2_queue;
            break;
        case I2C_3:
            queue = &i2c3_queue;
            break;
        default:
            return NULL;
    }

    queue->capacity = (1UL<<(capacity+1));
    queue->buffer = buffer;
    queue->head = 0;
    queue->tail = 0;

    return queue;
}

__bool queue_is_empty(queue_t* self){
    BARE_ASSERT(self != NULL);
    return self->head == self->tail;
}

__bool queue_is_full(queue_t* self){
    BARE_ASSERT(self != NULL);
    return (((self->head + 1) & (self->capacity - 1)) == self->tail);
}
__bool queue_enqueue(queue_t* self, uint8_t data){
    BARE_ASSERT(self != NULL);
    if(queue_is_full(self)){
        return FALSE;
    }

    self->buffer[self->head] = data;
    __DMB();
    self->head = (self->head+1) & (self->capacity - 1);
    return TRUE;
}

__bool queue_dequeue(queue_t* self, uint8_t* out_data){
    BARE_ASSERT(self != NULL);
    if(queue_is_empty(self)){
        return FALSE;
    }
    *out_data = self->buffer[self->tail]; 
    __DMB();
    self->tail = ((self->tail + 1) & (self->capacity -1));
    return TRUE;
}