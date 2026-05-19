#ifndef BUTTON_H
#define BUTTON_H

#include "stdint.h"

extern volatile uint8_t button_history;

#define BUTTON_GPIO_PORT GPIO_PORT_A
#define BUTTON_GPIO_PIN GPIO_PIN_0


#endif