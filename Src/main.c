
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

#include "def.h"
#include "devices/LED/LED.h"
#include "peripherals/gpio/gpio.h"
#include "peripherals/rcc/rcc.h"
#include "peripherals/timers/systick/systick.h"
#include "peripherals/timers/timer.h"
#include "services/delay/delay.h"
#include "services/interrupts/interrupt.h"
#include "arm/arm.h"

int main(void){
    RCC_t* rcc = init_RCC();

    GPIO_t* green_led_gpio = GPIO_init(LED_GPIO_PORT, LED_get_pin(LED_GREEN), rcc);
    LED_t* green_led = LED_init(LED_GREEN, green_led_gpio);

    enable_IRQ(SysTick_IRQn);
    __DSB();
    __ISB();
    Timer_t* timer = (Timer_t*)Systick_init(16000000, AHB);
    while(1){
        delay_ms(timer, 100);
        LED_toggle(green_led);
    }
}
