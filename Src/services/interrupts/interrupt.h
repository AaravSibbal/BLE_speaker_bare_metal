#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "../../def.h"
#include "../../peripherals/nvic/nvic.h"
#include "../../peripherals/scb/scb.h"
#include "../../peripherals/timers/systick/systick.h"


void enable_IRQ(const IRQn_t IRQn);
void disable_IRQ(const IRQn_t IRQn);
void set_pending_IRQ(const IRQn_t IRQn);
void clear_pending_IRQ(const IRQn_t IRQn);
uint32_t get_pending_IRQ(const IRQn_t IRQn);
uint32_t get_active(const IRQn_t IRQn);
void set_priority(const IRQn_t IRQn, uint32_t priority);
uint32_t get_priority(const IRQn_t IRQn);
void set_priority_grouping(const PriorityGroup_t pg);


#endif