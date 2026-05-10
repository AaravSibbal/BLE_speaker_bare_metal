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

__STATIC_INLINE uint32_t GPIO_get_moder_set_msk(const GPIO_MODER_t mode, const GPIO_Pin_t pin){
    return ((uint32_t)mode << (2*pin));
}

__STATIC_INLINE uint32_t GPIO_get_moder_msk(const GPIO_Pin_t pin){
    return (uint32_t)(~(MSK_OF_ONES(2) << (2*pin)));
}

void GPIO_set_moder(GPIO_t* self, const GPIO_MODER_t mode){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->MODER &= ~(GPIO_get_moder_msk(self->pin));
    self->driver->MODER |= GPIO_get_moder_set_msk(mode, self->pin);
    __set_PRIMASK(primask);
}

__STATIC_INLINE uint32_t GPIO_get_otyper_msk(const GPIO_Pin_t pin){
    return ((uint32_t)(MSK_OF_ONES(1) << pin));
}

__STATIC_INLINE uint32_t GPIO_get_otyper_set_msk(const GPIO_OTYPER_t type,
    const GPIO_Pin_t pin){
    return ((uint32_t)(((uint32_t)type) << pin));
}
void GPIO_set_otyper(GPIO_t* self, const GPIO_OTYPER_t type){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->OTYPER &= ~(GPIO_get_otyper_msk(self->pin));
    self->driver->OTYPER |= GPIO_get_otyper_set_msk(type, self->pin);
    __set_PRIMASK(primask);
}

__STATIC_INLINE uint32_t GPIO_get_odr_msk(const GPIO_Pin_t pin){
    return ((uint32_t)(MSK_OF_ONES(1) << pin));
}

__STATIC_INLINE uint32_t 

void GPIO_set_odr(GPIO_t* self, const GPIO_ODR_t output){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->ODR &= ~(GPIO_get_odr_msk(self->pin));
    self->driver->ODR |= ((uint32_t)output<<self->pin);
    __set_PRIMASK(primask);
}

void GPIO_set_alt_func(GPIO_t* self, const GPIO_AFx_t function){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    if(self->pin <= 7){
        self->driver->AFR[0] &= ~(15UL<<(self->pin*4));
        self->driver->AFR[0] |= ((uint32_t)function<<(self->pin*4));
    }else{
        uint32_t temp_pin = self->pin - 8;
        self->driver->AFR[1] &= ~(15UL<<(temp_pin*4));
        self->driver->AFR[1] |= (function<<(temp_pin*4));
    }
    __set_PRIMASK(primask);
}

void GPIO_set_bssr(GPIO_t* self, const BSSR_value_t val){
    GPIO_assert_self_and_driver(self);
    if(val == BSSR_SET){
        self->driver->BSSR = (1UL<<self->pin);
    }
    self->driver->BSSR = (1UL<<(self->pin+16));
}

void GPIO_set_pupdr(GPIO_t *self, const PUPDR_t val){
    GPIO_assert_self_and_driver(self);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->PUPDR &= ~(3UL<<(self->pin*2));
    self->driver->PUPDR |= ((uint32_t)val<<(self->pin*2));
    __set_PRIMASK(primask);
}

GPIO_t* GPIO_init(GPIO_t* self, const GPIO_port_t port, const GPIO_Pin_t pin){
    GPIO_assert_self_and_driver(self);
    self->driver = ((GPIO_driver_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
    self->pin = pin;
    return self;
}