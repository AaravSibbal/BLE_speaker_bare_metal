#include "gpio.h"
#include "../../assert.h"
#include "../../arm/arm.h"
#include "Src/def.h"

#define GPIO_BASE_ADDRESS (0x40020000)
#define GPIO_OFFSET (0x400)

typedef struct GPIO_driver{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_driver_t;

struct GPIO{
    GPIO_driver_t* driver;
    GPIO_Pin_t pin;
    RCC_t* rcc;
};

#define GPIO_MAX_INSTANCES 9
static GPIO_t gpio_pool[GPIO_MAX_INSTANCES];

__STATIC_INLINE void GPIO_BARE_ASSERT_self_and_driver(GPIO_t* self){
    BARE_ASSERT(self != NULL);
    BARE_ASSERT(self->driver != NULL);
}

__STATIC_INLINE uint32_t GPIO_get_msk(const GPIO_Pin_t pin, 
const uint32_t num_bits){
    return ((uint32_t)(msk_of_ones(num_bits) << (pin*num_bits)));
}

__STATIC_INLINE uint32_t GPIO_get_set_msk(const uint32_t val,
const GPIO_Pin_t pin, const uint32_t num_bits){
    return (uint32_t)((msk_of_ones(num_bits) & val) << (pin*num_bits));
}

#define MODER_ACCESS_BITS 2UL

void GPIO_set_moder(GPIO_t* self, const GPIO_MODER_t mode){
    GPIO_BARE_ASSERT_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->MODER &= ~(GPIO_get_msk(self->pin, MODER_ACCESS_BITS));
    self->driver->MODER |= GPIO_get_set_msk(
        mode, 
        self->pin, 
        MODER_ACCESS_BITS);
    __set_PRIMASK(primask);
}

#define OTYPER_ACCESS_BITS 1UL

void GPIO_set_otyper(GPIO_t* self, const GPIO_OTYPER_t type){
    GPIO_BARE_ASSERT_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->OTYPER &= ~(GPIO_get_msk(self->pin, OTYPER_ACCESS_BITS));
    self->driver->OTYPER |= GPIO_get_set_msk(
        type, 
        self->pin,
        OTYPER_ACCESS_BITS
    );
    __set_PRIMASK(primask);
}

#define ODR_ACCESS_BITS 1UL

__STATIC_INLINE void GPIO_set_bsrr(GPIO_t* self, const GPIO_ODR_t val){
    GPIO_BARE_ASSERT_self_and_driver(self);
    if(val == GPIO_OUTPUT_HIGH){
        self->driver->BSRR = (1UL<<self->pin);
    }else{
        self->driver->BSRR = (1UL<<(self->pin+16));
    }
}

void GPIO_set_odr(GPIO_t* self, const GPIO_ODR_t output){
    GPIO_BARE_ASSERT_self_and_driver(self);
    GPIO_set_bsrr(self, output);
}


#define ALT_ACCESS_BITS 4UL

void GPIO_set_alt_func(GPIO_t* self, const GPIO_AFx_t function){
    GPIO_BARE_ASSERT_self_and_driver(self);
    
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    
    uint32_t temp_pin = self->pin;
    uint32_t AFR_idx = 0;

    if(!(self->pin <= 7)){
        temp_pin = temp_pin - 8;
        AFR_idx = 1;
    }
    self->driver->AFR[AFR_idx] &= ~(GPIO_get_msk(temp_pin, ALT_ACCESS_BITS));
    self->driver->AFR[AFR_idx] |= GPIO_get_set_msk(
        function,
        self->pin, 
        ALT_ACCESS_BITS);
    __set_PRIMASK(primask);
}


#define PUPDR_ACCESS_BITS 2UL

void GPIO_set_pupdr(GPIO_t *self, const PUPDR_t val){
    GPIO_BARE_ASSERT_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->PUPDR &= ~(GPIO_get_msk(self->pin, PUPDR_ACCESS_BITS));
    self->driver->PUPDR |= GPIO_get_set_msk(
        val,
        self->pin,
        PUPDR_ACCESS_BITS);
    __set_PRIMASK(primask);
}   

GPIO_t* GPIO_init(const GPIO_port_t port, const GPIO_Pin_t pin, RCC_t* rcc_obj){
    GPIO_t* self = &gpio_pool[port];
    self->driver = ((GPIO_driver_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
    self->pin = pin;
    self->rcc = rcc_obj;
    RCC_en_GPIO(rcc_obj, port);
    return self;
}

GPIO_t* GPIO_init_empty(const GPIO_port_t port, const GPIO_Pin_t pin){
    GPIO_t* self = &gpio_pool[port];
    self->driver = ((GPIO_driver_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
    self->pin = pin;
    return self;
}

__STATIC_INLINE GPIO_driver_t* GPIO_get_driver(const GPIO_port_t port){
    return ((GPIO_driver_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
}

uint32_t GPIO_get_IDR_G(const GPIO_port_t port, const GPIO_Pin_t pin){
    GPIO_driver_t* driver = GPIO_get_driver(port);
    return (0x01UL & (driver->IDR >> pin));
}
