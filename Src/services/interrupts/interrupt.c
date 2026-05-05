#include "interrupt.h"
#include "drivers/scb.h"

void enable_IRQ(IRQn_t IRQn){
    if(IRQn >= 0){
        NVIC_enable_IRQ(NVIC_ENGINE, IRQn);
        return;
    }
    if(IRQn == SysTick_IRQn){
        printf("I haven't made the systick driver yet, couldn't enable systick interrupt\n");
        return;
    }
    SCB_enable_IRQ(SCB_ENGINE, IRQn);
}

void disable_IRQ(IRQn_t IRQn){
    if(IRQn >= 0){
        NVIC_disable_IRQ(NVIC_ENGINE, IRQn);
        return;
    }
    if(IRQn == SysTick_IRQn){
        printf("I haven't made the systick driver yet, couldn't disable systick interrupt\n");
        return;
    }
    SCB_disable_IRQ(SCB_ENGINE, IRQn);
}

void set_pending_IRQ(IRQn_t IRQn){
    if(IRQn >= 0){
        NVIC_set_pending_IRQ(NVIC_ENGINE, IRQn);
        return;
    }
    SCB_set_pending_IRQ(SCB_ENGINE, IRQn);
}

void clear_pending_IRQ(IRQn_t IRQn){
    if(IRQn >= 0){
        NVIC_clear_pending_IRQ(NVIC_ENGINE, IRQn);
        return;
    }
    SCB_clear_pending_IRQ(SCB_ENGINE, IRQn);
}

uint32_t get_pending_IRQ(IRQn_t IRQn){
    if(IRQn >= 0){
        return NVIC_get_pending_IRQ(NVIC_ENGINE, IRQn);
    }
    return SCB_get_pending_IRQ(SCB_ENGINE, IRQn);   
}

uint32_t get_active(IRQn_t IRQn){
    if(IRQn >= 0){
        return NVIC_get_active(NVIC_ENGINE, IRQn);
    }
    return SCB_get_active(SCB_ENGINE, IRQn);
}

void set_priority(IRQn_t IRQn, uint32_t priority){
    if(IRQn >= 0){
        NVIC_set_priority(NVIC_ENGINE, IRQn, priority);
        return;
    }
    SCB_set_priority(SCB_ENGINE, IRQn, priority);
}

uint32_t get_priority(IRQn_t IRQn){
    if(IRQn >= 0){
        return NVIC_get_priority(NVIC_ENGINE, IRQn);
    }
    return SCB_get_priority(SCB_ENGINE, IRQn);
}

void set_priority_grouping(PriorityGroup_t pg){
    SCB_write_priority_grouping(SCB_ENGINE, pg);
}