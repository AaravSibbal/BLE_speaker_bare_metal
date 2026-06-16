#ifndef I2C_H
#define I2C_H

#include "../../def.h"
#include "../gpio/gpio.h"
#include "../rcc/rcc.h"
#include "i2c_driver.h"

typedef struct I2C{
    I2C_driver_t* driver;
    GPIO_t* scl;
    GPIO_Pin_t scl_pin;
    GPIO_t* sda;
    GPIO_Pin_t sda_pin;
}I2C_t;

extern const GPIO_port_t i2c1_sda_gpio_port;
extern const GPIO_port_t i2c1_scl_gpio_port;
extern const GPIO_Pin_t i2c1_scl_gpio_pin;
extern const GPIO_Pin_t i2c1_sda_gpio_pin;

I2C_t* I2C_init(I2C_instance_t instance, I2C_mode_t mode, 
    GPIO_port_t sda_port, GPIO_Pin_t sda_pin, GPIO_port_t scl_port, 
    GPIO_Pin_t scl_pin, uint32_t clock_speed_Mhz, RCC_t* rcc); 

I2C_t* I2C_get_instance(const I2C_instance_t instance);

#endif