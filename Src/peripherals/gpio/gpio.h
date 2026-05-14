#ifndef GPIO_H
#define GPIO_H


#include "../../def.h"
#include "../rcc/rcc.h"

typedef struct GPIO GPIO_t; 

typedef enum GPIO_MODER {
    GPIO_MODE_INPUT = 0x00,
    GPIO_MODE_OUTPUT = 0x01,
    GPIO_MODE_ALT = 0x02,
    GPIO_MODE_ANALOG = 0x03
} GPIO_MODER_t;

typedef enum GPIO_OTYPER{
    GPIO_TYPE_PUSH_PULL = 0x00,
    GPIO_TYPE_OPEN_DRAIN = 0X01
} GPIO_OTYPER_t;

typedef enum GPIO_ODR {
    GPIO_OUTPUT_LOW = 0,
    GPIO_OUTPUT_HIGH
} GPIO_ODR_t;

typedef enum GPIO_AFx {
    AF0 = 0,
    AF1,
    AF2,
    AF3,
    AF4,
    AF5,
    AF6,
    AF7,
    AF8,
    AF9,
    AF10,
    AF11,
    AF12,
    AF13,
    AF14,
    AF15
}GPIO_AFx_t;

typedef enum GPIO_Pin {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15

}GPIO_Pin_t;

typedef enum PUPDR_val{
    NO_PUPD = 0,
    PULL_UP,
    PULL_DOWN
}PUPDR_t;



GPIO_t* GPIO_init(const GPIO_port_t port, const GPIO_Pin_t pin, RCC_t* rcc_obj);
GPIO_t* GPIO_init_empty(const GPIO_port_t port, const GPIO_Pin_t pin);
void GPIO_set_moder(GPIO_t* self, const GPIO_MODER_t mode);
void GPIO_set_otyper(GPIO_t* self, const GPIO_OTYPER_t type);
void GPIO_set_odr(GPIO_t* self, const GPIO_ODR_t output);
void GPIO_set_alt_func(GPIO_t* self, const GPIO_AFx_t function);
void GPIO_set_pupdr(GPIO_t *self, const PUPDR_t val);


#endif