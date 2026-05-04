#include "itm.h"
#include "dbgmcu.h"
#include "../../peripherals/drivers/gpio.h"
#include "demcr.h"
#include "tpiu.h"

static void ITM_unlock_access(ITM_t * const self){
    self->ITM_lock_access = ITM_UNLOCK_MAGIC_WRITE; 
}

static void ITM_enable(ITM_t * const self){
    self->ITM_trace_ctrl |= ITM_GLOBAL_EN_MSK;
}

static void ITM_gpio_setup(void){
    GPIO_set_moder(ITM_GPIO_PORT, ITM_GPIO_PIN, GPIO_MODE_ALT);
    GPIO_set_alt_func(ITM_GPIO_PORT, ITM_GPIO_PIN, AF0);
}

static void ITM_unlock_port(ITM_t * const self, uint8_t port){
    if(port >= 32){
        return;
    }

    uint32_t trace_priv_bit = port/4;
    self->ITM_trace_priv |= (1UL<<trace_priv_bit);
    self->ITM_trace_en |= (1UL<<port);
}

void ITM_init(ITM_t * const self){
    ITM_gpio_setup();
    DEMCR_enable_trace();
    DBGMCU_debug_enable(DBGMCU_ASYNC);
    // TPIU_set_proto(TPIU_SWO_NRZ);
    ITM_unlock_access(self);
    ITM_enable(self);
    ITM_unlock_port(self, 0);
}

void ITM_put_char(ITM_t * const self, char ch, uint8_t port){
    if(port >= 32){
        return;
    }

    while(self->ITM_stim_port[port] == 0){
        // loop 
    }

    self->ITM_stim_port[port] = (uint32_t)ch;
}

static void ITM_put_char_int(ITM_t * const self, int ch, uint8_t port){
    if(port >= 32){
        return;
    }

    while(self->ITM_stim_port[port] == 0){
        // loop 
    }

    self->ITM_stim_port[port] = (uint32_t)ch;
}


int __io_putchar(int ch){
    ITM_put_char_int(ITM_ENGINE, ch, 0);
    return ch;
}