#ifndef BUTTON_H
#define BUTTON_H

#include "../../peripherals/gpio/gpio.h"

typedef struct Button Button_t;

Button_t* Button_init(GPIO_t* gpio, RCC_t* rcc);



#endif