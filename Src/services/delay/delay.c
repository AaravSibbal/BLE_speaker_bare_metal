#include "delay.h"

void delay_ms(Timer_t* timer, uint32_t ms){
    timer->reset(); // make sure that ticks is 0
    timer->start(timer);
    uint32_t curr_ticks = timer->get_ticks();
    while(1){
        __WFI();
        curr_ticks = timer->get_ticks();
        if(curr_ticks >= ms){
            break;
        }
    }
    timer->stop(timer);
}