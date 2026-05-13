#ifndef TIMER_H
#define TIMER_H

#include "../../def.h"

typedef struct Timer{
    void (*start)(void *self);
    void (*stop)(void *self);
    void (*reset)(void *self);
    uint32_t (*get_ticks)();
}Timer_t;

#endif