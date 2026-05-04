#include <stdint.h>

/**
 *  The thing I am trying to do is set up in inturrupt on the user button so led blinks

So I think the first thing that I need to do if I want to start the LED inturrupt let's say.

turn on the rcc clock for GPIOD in ahb1 by setting bit 3.

turn on the rcc clock for GPIOA in ahb1 by setting bit 0.

turn on the rcc clock for syscfg in apb2 by setting bit 14

now that we have all three system clock on time for configuration.

go to syscfg_EXTICR1 and turn on the interrupt line for PA and set bits 0:3 to 0000

this is because of the diagram that I saw on section 12.1.3 and extiCR2

set exti_IMR bit 0 to 1. For interrput line PA. this is because we want to cause an ISR.

set exti_RTSR bit 0 to 1, this is us defining the actual even that triggers the ISR.


now for the gpios it is pretty standard stuff here. 
like the odr, moder. we set odr for PD4, we set up GPIOD moder 
to output, I don't exactly at this time know what we do with gpioa 
though from my blinky code we did set it up in input state for moder 
and that makes sense to me.


 */




#include "Src/print/driver/itm.h"
#include "interrupts/drivers/scb.h"
#include "rcc.h"
#include "peripherals/drivers/syscfg.h"
#include "print/printf.h"

#define USER_BTN_PIN ((uint8_t) 0)
#define USER_BTN_SYSCFG_PORT (SYSCFG_EXTI_PORTA)

void init_clocks(void);

int main(void){
    init_clocks();
    // SYSCFG_enable_EXTI(USER_BTN_SYSCFG_PORT, USER_BTN_PIN);
    ITM_init(ITM_ENGINE);
    SCB_init(SCB_ENGINE);
    SCB_write_priority_grouping(SCB_ENGINE, PRIGROUP_4PRE_0SUB);

    while(1){

    }
}

void init_clocks(void){
    RCC_AHB1_ENR |= (RCC_AHB1_GPIOA_EN) | (RCC_AHB1_GPIOD_EN) | (RCC_AHB1_GPIOB_EN);
    RCC_APB2_ENR |= (RCC_APB2_SYSCFG_EN);
}