#include "queue.h"

#include "../../peripherals/I2C/I2C.h"
#include "../../arm/arm.h"

void queue_init(queue_t* self, uint8_t* buffer, queue_cap_t capacity){
    if(buffer == NULL){
        return NULL;
    }

    self->capacity = (1UL<<(capacity));
    self->buffer = buffer;
    self->head = 0;
    self->tail = 0;
}

__bool queue_is_empty(queue_t* self){
    return self->head == self->tail;
}

__bool queue_is_full(queue_t* self){
    return (((self->head) - (self->tail)) == self->capacity);
}
__bool queue_enqueue(queue_t* self, uint8_t data){
    if(queue_is_full(self)){
        return FALSE;
    }

    self->buffer[self->head & (self->capacity - 1)] = data;
    __DMB();
    self->head++
    return TRUE;
}

__bool queue_dequeue(queue_t* self, uint8_t* out_data){
    if(queue_is_empty(self)){
        *out_data = NULL;
        return FALSE;
    }
    *out_data = self->buffer[self->tail & (self->capacity - 1)]; 
    __DMB();
    self->tail++;
    return TRUE;
}

queue_cap_t queue_get_capacity(queue_t* self){
    return self->capacity;
}