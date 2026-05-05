#include "nvic.h"

static uint32_t size_check(IRQn_t IRQn){
    if(IRQn < 0||  IRQn>81){
        return 0;
    }
    return 1;
}

void NVIC_enable_IRQ(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not enable IRQn: %d", IRQn);
        return;
    }
    uint32_t iser_idx = IRQn/32;
    uint32_t iser_bit = IRQn%32;

    self->NVIC_ISER[iser_idx] = (1UL<<iser_bit);
}

void NVIC_disable_IRQ(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not disable IRQn %d", IRQn);
        return;
    }
    uint32_t icer_idx = IRQn/32;
    uint32_t icer_bit = IRQn%32;

    self->NVIC_ICER[icer_idx] = (1UL<<icer_bit);
}

void NVIC_set_pending_IRQ(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not set pending IRQn: %d", IRQn);
        return;
    }
    uint32_t ispr_idx = IRQn/32;
    uint32_t ispr_bit = IRQn%32;

    self->NVIC_ISPR[ispr_idx] = (1UL<<ispr_bit);
}

void NVIC_clear_pending_IRQ(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not clear pending IRQn: %d", IRQn);
        return;
    }
    uint32_t icpr_idx = IRQn/32;
    uint32_t icpr_bit = IRQn%32;

    self->NVIC_ICPR[icpr_idx] = (1UL<<icpr_bit);
}

uint32_t NVIC_get_pending_IRQ(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not get pending IRQn: %d", IRQn);
        return 0;
    }
    uint32_t ispr_idx = IRQn/32;
    uint32_t ispr_bit = IRQn%32;
    uint32_t isPending  = (self->NVIC_ISPR[ispr_idx]) & (1UL<<ispr_bit);
    return (isPending >> ispr_bit);
}

uint32_t NVIC_get_active(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC:could not get the active for IRQn: %d", IRQn);
        return 0;
    }
    uint32_t iabr_idx = IRQn/32;
    uint32_t iabr_bit = IRQn%32;
    uint32_t isActive = (self->NVIC_IABR[iabr_idx]) & (1UL<<iabr_bit);
    return (isActive >> iabr_bit);
}

void NVIC_set_priority(nvic_t * const self, IRQn_t IRQn, uint32_t priority){
    if(priority > 15){
        printf("NVIC:could not set priority, Priority is too high, %d", priority);
        return;
    }
    if(!size_check(IRQn)){
        printf("NVIC:could not set priority for IRQn: %d", IRQn);
        return;
    }
    self->NVIC_IPR[IRQn] = (priority<<4);    
}

uint32_t NVIC_get_priority(nvic_t * const self, IRQn_t IRQn){
    if(!size_check(IRQn)){
        printf("NVIC: could not get the priority for IRQn: %d", IRQn);
        return 0;
    }
    uint8_t priority = self->NVIC_IPR[IRQn];
    uint32_t priority_32 = (priority>>4);
    return priority_32;
}