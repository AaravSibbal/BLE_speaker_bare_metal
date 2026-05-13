#include "LED.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"

struct LED{
    GPIO_t* gpio;
    LED_color_t color;
    __bool taken;
    __bool is_on;
};

static LED_t led_pool[4];

LED_t* LED_init(LED_color_t color, GPIO_t* gpio, RCC_t* rcc){
    uint8_t led_idx = color-12;
    assert(!led_pool[led_idx].taken);
    
    LED_t* self = &led_pool[led_idx];
    GPIO_set_moder(gpio, GPIO_MODE_OUTPUT);
    
    self->taken = TRUE;
    self->is_on = FALSE;
    return self;
}

void LED_toggle(LED_t* self){
    if(self->is_on){
        GPIO_set_odr(self->gpio, GPIO_OUTPUT_LOW);
    }else{
        GPIO_set_odr(self->gpio, GPIO_OUTPUT_HIGH);
    }
    self->is_on = !self->is_on;
}


__INLINE GPIO_Pin_t LED_get_pin(LED_color_t color){
    return ((GPIO_Pin_t)color);
}