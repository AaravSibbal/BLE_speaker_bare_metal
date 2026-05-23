#include "gpio.h"
#include "../../assert.h"
#include "../../arm/arm.h"
#include "Src/def.h"
#include "Src/peripherals/rcc/rcc.h"

#define GPIO_BASE_ADDRESS (0x40020000)
#define GPIO_OFFSET (0x400)

typedef struct GPIO{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_t;

__STATIC_INLINE uint32_t GPIO_get_msk(const GPIO_Pin_t pin, 
const uint32_t num_bits){
    return ((uint32_t)(msk_of_ones(num_bits) << (pin*num_bits)));
}

__STATIC_INLINE uint32_t GPIO_get_set_msk(const uint32_t val,
const GPIO_Pin_t pin, const uint32_t num_bits){
    return (uint32_t)((msk_of_ones(num_bits) & val) << (pin*num_bits));
}

#define MODER_ACCESS_BITS 2UL

void GPIO_set_moder(GPIO_t* self, const GPIO_Pin_t pin, const GPIO_MODER_t mode){
    BARE_ASSERT(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->MODER &= ~(GPIO_get_msk(pin, MODER_ACCESS_BITS));
    self->MODER |= GPIO_get_set_msk(mode, pin, MODER_ACCESS_BITS);
    __set_PRIMASK(primask);
}

#define OTYPER_ACCESS_BITS 1UL

void GPIO_set_otyper(GPIO_t* self, const GPIO_Pin_t pin, const GPIO_OTYPER_t type){
    BARE_ASSERT(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->OTYPER &= ~(GPIO_get_msk(pin, OTYPER_ACCESS_BITS));
    self->OTYPER |= GPIO_get_set_msk(
        type, 
        pin,
        OTYPER_ACCESS_BITS
    );
    __set_PRIMASK(primask);
}

#define ODR_ACCESS_BITS 1UL

__STATIC_INLINE void GPIO_set_bsrr(
    GPIO_t* self, const GPIO_Pin_t pin, const GPIO_ODR_t val){
    BARE_ASSERT(self != NULL);
    if(val == GPIO_OUTPUT_HIGH){
        self->BSRR = (1UL<<pin);
    }else{
        self->BSRR = (1UL<<(pin+16));
    }
}

void GPIO_set_odr(GPIO_t* self, const GPIO_Pin_t pin, const GPIO_ODR_t output){
    GPIO_set_bsrr(self, pin, output);
}


#define ALT_ACCESS_BITS 4UL

void GPIO_set_alt_func(
    GPIO_t* self, const GPIO_Pin_t pin, const GPIO_AFx_t function){
    BARE_ASSERT(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint32_t temp_pin = pin;
    uint32_t AFR_idx = 0;

    if(!(pin <= 7)){
        temp_pin = temp_pin - 8;
        AFR_idx = 1;
    }
    self->AFR[AFR_idx] &= ~(GPIO_get_msk(temp_pin, ALT_ACCESS_BITS));
    self->AFR[AFR_idx] |= GPIO_get_set_msk(
        function,
        pin, 
        ALT_ACCESS_BITS);
    __set_PRIMASK(primask);
}


#define PUPDR_ACCESS_BITS 2UL

void GPIO_set_pupdr(GPIO_t *self, const GPIO_Pin_t pin, const GPIO_PUPDR_t val){
    BARE_ASSERT(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->PUPDR &= ~(GPIO_get_msk(pin, PUPDR_ACCESS_BITS));
    self->PUPDR |= GPIO_get_set_msk(val, pin, PUPDR_ACCESS_BITS);
    __set_PRIMASK(primask);
}   

GPIO_t* GPIO_init(const GPIO_port_t port, RCC_t* rcc){
    GPIO_t* gpio = ((GPIO_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
    RCC_en_GPIO(rcc, port);
    return gpio;
}

__STATIC_INLINE GPIO_t* GPIO_get_instance(const GPIO_port_t port){
    return ((GPIO_t*)(GPIO_BASE_ADDRESS + (port * GPIO_OFFSET)));
}

uint32_t GPIO_get_IDR_G(const GPIO_port_t port, const GPIO_Pin_t pin){
    GPIO_t* driver = GPIO_get_instance(port);
    return (0x01UL & (driver->IDR >> pin));
}
