#ifndef SYSTICK_H
#define SYSTICK_H

#include "../timer.h"
#include "../../../def.h"

typedef struct Systick Systick_t;

typedef enum CLCK_SRC{
    AHB_BY_8 = 0,
    AHB = 1
} Clck_src_t;

Systick_t* Systick_init(uint32_t clck_speed_hz, Clck_src_t clck_src);
void Systick_start(void* self);
void Systick_stop(void* self);
void Systick_reset(void);
uint32_t Systick_get_ticks(void);
void Systick_enable_interrupt(void);
void Systick_disable_interrupt(void);


void SysTick_Handler(void);

#endif