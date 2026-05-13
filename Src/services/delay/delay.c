#include "delay.h"

void delay_ms(Timer_t* timer, uint32_t ms){
    timer->reset(timer); // make sure that ticks is 0
    timer->start(timer);
    uint32_t curr_ticks = timer->get_ticks();
    while((ms - timer->get_ticks(timer)) != 0){
        __WFI();
        curr_ticks = timer->get_ticks();
        if((ms-curr_ticks) == 0){
            break;
        }
    }
    timer->stop(timer);
}