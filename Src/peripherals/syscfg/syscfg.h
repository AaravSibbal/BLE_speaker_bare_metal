#ifndef SYSCFG_H
#define SYSCFG_H

#include "../../def.h"
#include "Src/peripherals/rcc/rcc.h"



typedef struct SYSCFG SYSCFG_t;


typedef enum SYSCFG_EXTI_Port {
    SYSCFG_EXTI_PORTA = 0,
    SYSCFG_EXTI_PORTB,
    SYSCFG_EXTI_PORTC,
    SYSCFG_EXTI_PORTD,
    SYSCFG_EXTI_PORTE,
    SYSCFG_EXTI_PORTF,
    SYSCFG_EXTI_PORTG,
    SYSCFG_EXTI_PORTH,
    SYSCFG_EXTI_PORTI
} SYSCFG_EXTI_Port_t;

SYSCFG_t* SYSCFG_init(RCC_t* rcc);
/**
 * values can range from 0-15
 */
void SYSCFG_enable_EXTI(SYSCFG_t* self, GPIO_port_t port, uint8_t pin);

#endif