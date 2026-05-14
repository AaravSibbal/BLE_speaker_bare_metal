#ifndef ITM_H
#define ITM_H

#include "../../def.h"
#include "../gpio/gpio.h"
#include "../rcc/rcc.h"

typedef struct ITM ITM_t;



/**
 * port values are from 0-31
 */
ITM_t* ITM_init(ITM_t* self, GPIO_t* gpio, RCC_t* rcc_obj);
/**
 * port values are from 0-31
 */
void ITM_put_char(ITM_t* self, char ch, uint8_t port);
int __io_putchar(int ch);

#endif