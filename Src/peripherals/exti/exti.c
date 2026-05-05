#include "exti.h"

void EXTI_set_IMR(EXTI_Line_t line){
    if(line >22){
        return;
    }
    EXTI_ENGINE->EXTI_IMR |= (1<<line);
}

void EXTI_set_RTSR(EXTI_Line_t line){
    if(line >22){
        return;
    }

    EXTI_ENGINE->EXTI_RTSR |= (1<<line);
}

void EXTI_clear_PR(EXTI_Line_t line){
    if(line>22){
        return;
    }
    EXTI_ENGINE->EXTI_PR = (1<<line);
}
