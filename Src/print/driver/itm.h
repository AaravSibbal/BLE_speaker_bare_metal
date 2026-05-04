#ifndef ITM_H
#define ITM_H

#include "../../def.h"
#include "../../peripherals/drivers/gpio.h"
#include "dbgmcu.h"
#include "demcr.h"
#include "tpiu.h"

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

/**
 *Your ITM Implementation Checklist

Since you are about to write the code, here is the final sequence based on our discussion. Stick to this order:

    Pin Config: Configure PB3 to Alternate Function 0 (AF0).

    MCU Debug Config: Write to DBGMCU_CR. Set bits 7:6 for Asynchronous mode, and crucially, set bit 5 to 1 to enable Trace I/O.

    TPIU Config: Set the Selected Pin Protocol Register (SPPR) to use NRZ encoding (not Manchester).

    Unlock: Write $0xC5ACCE55$ to the ITM Lock Access Register (LAR).

    Global Enable: Enable the ITM globally via the Trace Control Register (TCR).

    Port Enable: Enable Port 0 using the Trace Enable Register (TER) and Trace Privilege Register (TPR).

    Transmit Function: Write your putchar logic. Remember to check the Stimulus Port 0 FIFO status bit before writing your character. 
 */

/**
 * Configure the TPIU and assign TRACE I/Os by configuring the DBGMCU_CR (refer to
Section 38.17.2 and Section 38.16.3)
• Write 0xC5ACCE55 to the ITM Lock Access register to unlock the write access to the
ITM registers
• Write 0x00010005 to the ITM Trace Control register to enable the ITM with Sync
enabled and an ATB ID different from 0x00
• Write 0x1 to the ITM Trace Enable register to enable the Stimulus Port 0
• Write 0x1 to the ITM Trace Privilege register to unmask stimulus ports 7:0
• Write the value to output in the Stimulus Port register 0: this can be done by software
(using a printf function)
 */

#endif