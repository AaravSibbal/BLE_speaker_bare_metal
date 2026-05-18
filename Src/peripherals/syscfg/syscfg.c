#include "syscfg.h"
#include "../../arm/arm.h"
#include "Src/def.h"
#define SYSCFG_BASE (0x40013800)
#define EXTICR1_PA_BIT (0b0000)
#define EXTICR1_BIT_MSK (msk_of_ones(4))

#define SYSCFG_ENGINE ((SYSCFG_t *) SYSCFG_BASE)

struct SYSCFG{
    __IO uint32_t MEMRMP;
    __IO uint32_t PMC;
    __IO uint32_t EXTICR[4];
    uint32_t RESERVED[2];
    __IO uint32_t CMPCR;
};

SYSCFG_t* SYSCFG_init(RCC_t* rcc){
    RCC_en_SYSCFG(rcc);
    return SYSCFG_ENGINE;   
}

void SYSCFG_enable_EXTI(SYSCFG_t* self, GPIO_port_t port, uint8_t pin){
    uint32_t EXTI_idx = (pin/4);
    uint32_t EXTI_bit = ((pin % 4) * 4);
    volatile uint32_t* addr = &self->EXTICR[EXTI_idx];
    uint32_t current_val;
    uint32_t status;
    do{
        current_val = __LDREXW(addr);
        current_val &= ~(EXTICR1_BIT_MSK<<EXTI_bit);
        current_val |= (port<<EXTI_bit);
        status = __STREXW(current_val, addr);
    }while(status != 0);
}