#include "gpio.h"

void GPIO_set_moder(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_MODER_t mode){
    gpio->MODER &= ~(3 << (2*pin));
    gpio->MODER |= (mode <<(2*pin));
}

void GPIO_set_otyper(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_OTYPER_t type){
    gpio->OTYPER &= ~(1<<pin);
    gpio->OTYPER |= (type<<pin);
}

void GPIO_set_odr(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_ODR_t output){
    gpio->ODR &= ~(1<<pin);
    gpio->ODR |= (output<<pin);
}

void GPIO_set_alt_func(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_AFx_t function){
    if(pin <= 7){
        gpio->AFR[0] &= ~(15<<(pin*4));
        gpio->AFR[0] |= (function<<(pin*4));
        return;
    }
    pin = pin - 8;
    gpio->AFR[1] &= ~(15<<(pin*4));
    gpio->AFR[1] |= (function<<(pin*4));
}

void GPIO_set_bssr(GPIO_t* gpio, GPIO_Pin_t pin, BSSR_value_t val){
    if(val == BSSR_SET){
        gpio->BSSR |= (1<<pin);
        return;
    }
    gpio->BSSR |= (1<<(pin+16));
}

void GPIO_set_pupdr(GPIO_t *gpio, GPIO_Pin_t pin, PUPDR_t val){
    gpio->GPIO_PUPDR &= ~(3<<(pin*2));
    gpio->GPIO_PUPDR |= (val<<(pin*2));
}
