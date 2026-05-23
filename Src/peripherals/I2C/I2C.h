#ifndef I2C_H
#define I2C_H


#include "../../def.h"
#include "../gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"

typedef struct I2C_driver{
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t OAR1;
    __IO uint32_t OAR2;
    __IO uint32_t DR1;
    __IO uint32_t SR1;
    __IO uint32_t SR2;
    __IO uint32_t CCR;
    __IO uint32_t TRISE;
    __IO uint32_t FLTR;
} I2C_driver_t;

typedef struct I2C{
    I2C_driver_t* driver;
    GPIO_t* scl;
    GPIO_Pin_t scl_pin;
    GPIO_t* sda;
    GPIO_Pin_t sda_pin;
}I2C_t;

typedef enum I2C_instance{
    I2C_1 = 0,
    I2C_2 = 1,
    I2C_3 = 2
}I2C_instance_t;

typdef enum I2C_mode{
    I2C_MODE_STANDARD = 0,
    I2C_MODE_FAST = 1
}I2C_mode_t;

I2C_driver_t* I2C_get_instance(const I2C_instance_t instance);
void I2C_en_reset(I2C_driver_t* driver);
void I2C_en_interrupts(I2C_driver_t* driver);
void I2C_en_errors(I2C_driver_t* driver);
void I2C_en_buffer(I2C_driver_t* driver);
void I2C_en_ack(I2C_driver_t* driver);
void I2C_set_freq_bits(I2C_driver_t* driver, uint32_t clock_spd_Mhz);
void I2C_set_ccr(I2C_driver_t* driver, uint32_t ccr_val);
void I2C_set_trise(I2C_driver_t* driver, uint32_t trise_val);
void I2C_en_peripheral(I2C_driver_t* driver);
void I2C_en_clock(I2C_instance_t instance);

I2C_t* I2C_init(I2C_instance_t instance, I2C_mode_t mode, 
    GPIO_port_t sda_port, GPIO_Pin_t sda_pin, GPIO_port_t scl_port, 
    GPIO_Pin_t scl_pin, uint32_t clock_speed_Mhz, RCC_t* rcc);
