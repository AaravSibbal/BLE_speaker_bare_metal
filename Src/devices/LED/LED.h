#ifndef LED_H
#define LED_H

#include "../../def.h"
#include "../../peripherals/gpio/gpio.h"

typedef struct LED LED_t;

typedef enum LED_color{
    LED_GREEN = 12,
    LED_ORANGE,
    LED_RED,
    LED_BLUE
}LED_color_t;

#define LED_GPIO_PORT GPIO_PORT_D

LED_t* LED_init(LED_color_t color, GPIO_t* gpio);
void LED_toggle(LED_t* self);
GPIO_Pin_t LED_get_pin(LED_color_t color);

#endif