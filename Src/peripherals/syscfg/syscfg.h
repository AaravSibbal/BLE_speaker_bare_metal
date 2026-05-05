#ifndef SYSCFG_H
#define SYSCFG_H

#include "../../def.h"

#define SYSCFG_BASE (0x40013800)
#define EXTICR1_PA_BIT (0b0000)
#define EXTICR1_BIT_MSK ((1UL<<4)-1)

typedef struct SYSCFG{
    __IO uint32_t SYSCFG_MEMRMP;
    __IO uint32_t SYSCFG_PMC;
    __IO uint32_t SYSCFG_EXTICR[4];
    uint32_t RESERVED[2];
    __IO uint32_t SYSCFG_CMPCR;
} SYSCFG_t;

#define SYSCFG_ENGINE ((SYSCFG_t *) SYSCFG_BASE)

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

/**
 * values can range from 0-15
 */
void SYSCFG_enable_EXTI(SYSCFG_EXTI_Port_t port, uint8_t pin);

#endif