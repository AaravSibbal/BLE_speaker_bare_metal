#include "LED.h"
#include "Src/peripherals/gpio/gpio.h"

struct LED{
    GPIO_t* gpio_driver;
    GPIO_Pin_t gpio_pin;
    LED_color_t color;
    __bool taken;
    __bool is_on;
};

static LED_t led_pool[4];

LED_t* LED_init(LED_color_t color, GPIO_t* gpio){
    uint8_t led_idx = color-12;
    BARE_ASSERT(!led_pool[led_idx].taken);
    
    LED_t* self = &led_pool[led_idx];
    self->gpio_driver = gpio;
    self->gpio_pin = LED_get_pin(color);

    GPIO_set_moder(
        gpio, 
        self->gpio_pin,
         GPIO_MODE_OUTPUT
    );
    
    self->taken = TRUE;
    self->is_on = FALSE;
    return self;
}

void LED_toggle(LED_t* self){
    if(self->is_on){
        GPIO_set_odr(
            self->gpio_driver, 
            self->gpio_pin, 
            GPIO_OUTPUT_LOW
        );
    }else{
        GPIO_set_odr(
            self->gpio_driver, 
            self->gpio_pin, 
            GPIO_OUTPUT_HIGH
        );
    }
    self->is_on = !self->is_on;
}


__INLINE GPIO_Pin_t LED_get_pin(LED_color_t color){
    return ((GPIO_Pin_t)color);
}