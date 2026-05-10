#include "gpio.h"
#include "assert.h"

#define GPIO_BASE_ADDRESS (0x40020000)
#define GPIO_OFFSET (0x400)

typedef struct GPIO_driver{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSSR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_driver_t;

struct GPIO{
    GPIO_driver_t* driver;
    GPIO_Pin_t pin;
};

__STATIC_INLINE void GPIO_assert_self_and_driver(GPIO_t* self){
    assert(self != NULL);
    assert(self->driver != NULL);
}

__STATIC_INLINE uint32_t GPIO_get_msk(const GPIO_Pin_t pin, 
const uint32_t num_bits){
    return ((uint32_t)(msk_of_ones(num_bits) << (pin*num_bits)));
}

__STATIC_INLINE uint32_t GPIO_get_set_msk(const uint32_t val,
const GPIO_Pin_t pin, const uint32_t num_bits){
    return (uint32_t)((mask_of_ones(num_bits) & val) << (pin*num_bits));
}

#define MODER_ACCESS_BITS 2UL

void GPIO_set_moder(GPIO_t* self, const GPIO_MODER_t mode){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->MODER &= ~(GPIO_get_msk(self->pin, MODER_ACCESS_BITS));
    (GPIO_get_moder_msk(self->pin));
    self->driver->MODER |= GPIO_get_set_msk(
        mode, 
        self->pin, 
        MODER_ACCESS_BITS);
    __set_PRIMASK(primask);
}

#define OTYPER_ACCESS_BITS 1UL

void GPIO_set_otyper(GPIO_t* self, const GPIO_OTYPER_t type){
    GPIO_assert_self_and_driver(self);
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

void GPIO_set_odr(GPIO_t* self, const GPIO_ODR_t output){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->ODR &= ~(GPIO_get_msk(self->pin, ODR_ACCESS_BITS));
    self->driver->ODR |= GPIO_get_set_msk(
        output,
        self->pin,
        ODR_ACCESS_BITS
    );
    __set_PRIMASK(primask);
}


#define ALT_ACCESS_BITS 4UL

void GPIO_set_alt_func(GPIO_t* self, const GPIO_AFx_t function){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    if(self->pin <= 7){
        self->driver->AFR[0] &= ~(GPIO_get_msk(self->pin, ALT_ACCESS_BITS));
        self->driver->AFR[0] |= GPIO_get_set_msk(
            function,
            self->pin, 
            ALT_ACCESS_BITS);
    }else{
        uint32_t temp_pin = self->pin - 8;
        self->driver->AFR[1] &= ~(GPIO_get_msk(self->pin, ALT_ACCESS_BITS));
        self->driver->AFR[1] |= GPIO_get_set_msk(
            function,
            self->pin,
            ALT_ACCESS_BITS
        );
    }
    __set_PRIMASK(primask);
}

void GPIO_set_bssr(GPIO_t* self, const BSSR_value_t val){
    GPIO_assert_self_and_driver(self);
    if(val == BSSR_SET){
        self->driver->BSSR = (1UL<<self->pin);
    }else{
        self->driver->BSSR = (1UL<<(self->pin+16));
    }
}

#define PUPDR_ACCESS_BITS 2
void GPIO_set_pupdr(GPIO_t *self, const PUPDR_t val){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->PUPDR &= ~(GPIO_get_msk(self->pin, PUPDR_ACCESS_BITS));
    self->driver->PUPDR |= GPIO_get_set_msk(
        val,
        self->pin,
        PUPDR_ACCESS_BITS);
    __set_PRIMASK(primask);
}

GPIO_t* GPIO_init(GPIO_t* self, const GPIO_port_t port, const GPIO_Pin_t pin){
    GPIO_assert_self_and_driver(self);
    self->driver = ((GPIO_driver_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
    self->pin = pin;
    return self;
}