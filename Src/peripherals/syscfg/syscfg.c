#include "syscfg.h"

void SYSCFG_enable_EXTI(SYSCFG_EXTI_Port_t port, uint8_t pin){
    if(pin >=16){
        return;
    }
    uint32_t EXTI_idx = (pin/4);
    uint32_t EXTI_bit = ((pin % 4) * 4);

    SYSCFG_ENGINE->SYSCFG_EXTICR[EXTI_idx] &= ~(EXTICR1_BIT_MSK<<EXTI_bit);
    SYSCFG_ENGINE->SYSCFG_EXTICR[EXTI_idx] |= (port<<EXTI_bit);
}