#include "itm.h"
#include "../dbgmcu/dbgmcu.h"
#include "../demcr/demcr.h"
#include "../tpiu/tpiu.h"
#include "Src/arm/arm.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"

#define ITM_BASE (0xE0000000)
#define ITM_UNLOCK_MAGIC_WRITE (0xC5ACCE55)
#define ITM_GLOBAL_EN_MSK (1<<0)
#define ITM_ENGINE ((ITM_driver_t *) ITM_BASE)
#define ITM_GPIO_PORT GPIO_PORT_B
#define ITM_GPIO_PIN GPIO_PIN_3


ITM_t itm;

typedef struct ITM_driver{
    __IO uint32_t ITM_stim_port[32];
    uint32_t RESERVED_0[864];
    __IO uint32_t ITM_trace_en;
    uint32_t RESERVED_1[20];
    __IO uint32_t ITM_trace_priv;
    uint32_t RESERVED_2[15];
    __IO uint32_t ITM_trace_ctrl;
    uint32_t RESERVED_3[75];
    __IO uint32_t ITM_lock_access;
}ITM_driver_t;

struct ITM{
    ITM_driver_t* driver;
};

static void ITM_unlock_access(ITM_t* self){
    self->driver->ITM_lock_access = ITM_UNLOCK_MAGIC_WRITE; 
}

static void ITM_enable(ITM_t* self){
    self->driver->ITM_trace_ctrl |= ITM_GLOBAL_EN_MSK;
}

static void ITM_gpio_setup(GPIO_t* gpio){
    GPIO_set_moder(gpio, ITM_GPIO_PIN, GPIO_MODE_ALT);
    GPIO_set_alt_func(gpio, ITM_GPIO_PIN, AF0);
}

static void ITM_unlock_port(ITM_t* self, uint8_t port){
    if(port >= 32){
        return;
    }

    uint32_t trace_priv_bit = port/4;
    self->driver->ITM_trace_priv |= (1UL<<trace_priv_bit);
    self->driver->ITM_trace_en |= (1UL<<port);
}

ITM_t* ITM_init(RCC_t* rcc_obj){
    
    itm.driver = (ITM_driver_t*)ITM_ENGINE;
    GPIO_t* gpio_b = GPIO_init(GPIO_PORT_B, rcc_obj);
    ITM_gpio_setup(gpio_b);
    DEMCR_enable_trace();
    DBGMCU_debug_enable(DBGMCU_ASYNC);
    ITM_unlock_access(&itm);
    ITM_enable(&itm);
    ITM_unlock_port(&itm, 0);
    return &itm;
}

void ITM_put_char(ITM_t* self, char ch, uint8_t port){
    if(port >= 32){
        return;
    }

    while(self->driver->ITM_stim_port[port] == 0){
        // loop 
    }

    self->driver->ITM_stim_port[port] = (uint32_t)ch;
}

static void ITM_put_char_int(ITM_driver_t* self, int ch, uint8_t port){
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