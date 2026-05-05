#ifndef GPIO_H
#define GPIO_H


#include "../../def.h"

#define GPIOB_BASE  ((uint32_t)0x40020400)
#define GPIOD_BASE ((uint32_t)0x40020C00)

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

typedef struct gpio_struct{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t GPIO_OSPEEDR;
    __IO uint32_t GPIO_PUPDR;
    __IO uint32_t GPIO_IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSSR;
    __IO uint32_t GPIO_LCKR;
    __IO uint32_t AFR[2];
} GPIO_t;

typedef enum BSSR_value{
    BSSR_RESET = 0,
    BSSR_SET
}BSSR_value_t;

typedef enum PUPDR_val{
    NO_PUPD = 0,
    PULL_UP,
    PULL_DOWN
}PUPDR_t;

#define GPIOB ((GPIO_t *) GPIOB_BASE)
#define GPIOD ((GPIO_t *) GPIOD_BASE)

void GPIO_set_moder(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_MODER_t mode);
void GPIO_set_otyper(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_OTYPER_t type);
void GPIO_set_odr(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_ODR_t output);
void GPIO_set_alt_func(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_AFx_t function);
void GPIO_set_bssr(GPIO_t* gpio, GPIO_Pin_t pin, BSSR_value_t);
void GPIO_set_pupdr(GPIO_t *gpio, GPIO_Pin_t pin, PUPDR_t val);


#endif