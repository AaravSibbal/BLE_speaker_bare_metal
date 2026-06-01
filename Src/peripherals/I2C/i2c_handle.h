#ifndef I2C_HANDLE_H
#define I2C_HANDLE_H

#include "../../def.h"
#include "../../data_structure/queue/queue.h"
#include "I2C.h"

typedef enum{
    I2C_DIR_READ = 0UL,
    I2C_DIR_WRITE = 1UL
}I2C_direction_t;

typedef enum I2C_state_machine{
    I2C_STATE_READY = 0,
    I2C_STATE_BUSY_TX,
    I2C_STATE_BUSY_RX,
    I2C_STATE_DONE
} I2C_state_machine_t;

typedef struct{
    I2C_t* i2c_device;
    I2C_direction_t direction;
    queue_t* queue;
}I2C_handle_t;

I2C_handle_t* I2C_handle_init(
I2C_t* i2c_device, I2C_instance_t i2c_instance,queue_cap_t capacity, 
uint8_t* buffer, I2C_direction_t direction);

__bool I2C_write(I2C_handle_t* i2c_handle, uint8_t peripheral_addr);
__bool I2C_read(I2C_handle_t* i2c_handle, uint8_t peripheral_addr);

#endif