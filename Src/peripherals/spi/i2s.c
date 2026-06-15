#include "i2s.h"
#include "Src/arm/arm.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"

static const GPIO_port_t I2S3_MCK_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_MCK_GPIO_PIN = GPIO_PIN_7;
// static const GPIO_port_t I2S3_SCK_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_SCK_GPIO_PIN = GPIO_PIN_10;
// static const GPIO_port_t I2S3_SD_GPIO_PORT = GPIO_PORT_C;
static const GPIO_Pin_t I2S3_SD_GPIO_PIN = GPIO_PIN_12;
static const GPIO_port_t I2S3_WS_GPIO_PORT = GPIO_PORT_A;
static const GPIO_Pin_t I2S3_WS_GPIO_PIN = GPIO_PIN_4;

static const GPIO_MODER_t I2S_gpio_mode = GPIO_MODE_ALT;
static const GPIO_AFx_t I2S3_gpio_af = AF6;
static const GPIO_OTYPER_t I2S_gpio_otype = GPIO_TYPE_PUSH_PULL;
static const GPIO_PUPDR_t I2S_gpio_pupd = NO_PUPD;
static const GPIO_OSPEEDR_t I2S_gpio_speed = OSPEED_HIGH;


__STATIC_INLINE void i2s_conf_gpio(GPIO_t* gpio, GPIO_Pin_t pin, GPIO_AFx_t af_val){
    GPIO_set_alt_func(gpio, pin, af_val);
    GPIO_set_otyper(gpio, pin, I2S_gpio_otype);
    GPIO_set_ospeedr(gpio, pin, I2S_gpio_speed);
    GPIO_set_pupdr(gpio, pin, I2S_gpio_pupd);
    GPIO_set_moder(gpio, pin, I2S_gpio_mode);
}

void i2s_init_gpio(I2S_instance_t instance, RCC_t* rcc){
    switch (instance) {
        case I2S_INSTANCE_2:
            // not implemented yet
            __BKPT(0);
            break;
        case I2S_INSTANCE_3:{
            GPIO_t* gpio_c = GPIO_init(I2S3_MCK_GPIO_PORT, rcc);
            GPIO_t* gpio_a = GPIO_init(I2S3_WS_GPIO_PORT, rcc);
            i2s_conf_gpio(gpio_c, I2S3_MCK_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_c, I2S3_SCK_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_c, I2S3_SD_GPIO_PIN, I2S3_gpio_af);
            i2s_conf_gpio(gpio_a, I2S3_WS_GPIO_PIN, I2S3_gpio_af);
            break;
        }
        default:
            return;
    }
}

__STATIC_INLINE void i2s_init_clocks(I2S_instance_t instance, RCC_t* rcc){
    switch (instance) {
        case I2S_INSTANCE_2:
            RCC_en_SPI2(rcc);
            break;
        case I2S_INSTANCE_3:
            RCC_en_SPI3(rcc);
            break;
        default:
            return;
    }
}



void i2s_init(I2S_instance_t instance, RCC_t* rcc){
    i2s_init_gpio(instance, rcc);
    i2s_init_clocks(instance, rcc);

    
}