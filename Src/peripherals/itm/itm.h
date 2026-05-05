#ifndef ITM_H
#define ITM_H

#include "../../def.h"
#include "../gpio/gpio.h"
#include "../dbgmcu/dbgmcu.h"
#include "../demcr/demcr.h"
#include "../tpiu/tpiu.h"

#define ITM_BASE (0xE0000000)
#define ITM_UNLOCK_MAGIC_WRITE (0xC5ACCE55)
#define ITM_GLOBAL_EN_MSK (1<<0)
#define ITM_GPIO_PORT (GPIOB)
#define ITM_GPIO_PIN (GPIO_PIN_3)

typedef struct ITM{
    __IO uint32_t ITM_stim_port[32];
    uint32_t RESERVED_0[864];
    __IO uint32_t ITM_trace_en;
    uint32_t RESERVED_1[20];
    __IO uint32_t ITM_trace_priv;
    uint32_t RESERVED_2[15];
    __IO uint32_t ITM_trace_ctrl;
    uint32_t RESERVED_3[75];
    __IO uint32_t ITM_lock_access;
}ITM_t;

#define ITM_ENGINE ((ITM_t *) ITM_BASE)

/**
 * port values are from 0-31
 */
void ITM_init(ITM_t * const self);
/**
 * port values are from 0-31
 */
void ITM_put_char(ITM_t * const self, char ch, uint8_t port);
int __io_putchar(int ch);

#endif