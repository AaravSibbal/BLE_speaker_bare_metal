#ifndef I2C_H
#define I2C_H


#include "../../def.h"
#include "../gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"
#include "../../data_structure/queue/queue.h"

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
    queue_t* queue;
}I2C_t;


typedef enum I2C_mode{
    I2C_MODE_STANDARD = 0,
    I2C_MODE_FAST = 1
}I2C_mode_t;

typedef enum I2C_state_machine{
    I2C_STATE_READY = 0,
    I2C_STATE_BUSY_TX,
    I2C_STATE_BUSY_RX,
    I2C_STATE_DONE
} I2C_state_machine_t;

I2C_t* I2C_init(I2C_instance_t instance, I2C_mode_t mode, 
    GPIO_port_t sda_port, GPIO_Pin_t sda_pin, GPIO_port_t scl_port, 
    GPIO_Pin_t scl_pin, uint32_t clock_speed_Mhz, RCC_t* rcc, 
    queue_cap_t capacity, uint8_t* buffer);

#endif