#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "../../def.h"
#include "../../peripherals/nvic/nvic.h"
#include "../../peripherals/scb/scb.h"


void enable_IRQ(IRQn_t IRQn);
void disable_IRQ(IRQn_t IRQn);
void set_pending_IRQ(IRQn_t IRQn);
void clear_pending_IRQ(IRQn_t IRQn);
uint32_t get_pending_IRQ(IRQn_t IRQn);
uint32_t get_active(IRQn_t IRQn);
void set_priority(IRQn_t IRQn, uint32_t priority);
uint32_t get_priority(IRQn_t IRQn);
void set_priority_grouping(PriorityGroup_t pg);


#endif