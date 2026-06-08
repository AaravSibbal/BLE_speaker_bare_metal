#ifndef I2C_HANDLE_H
#define I2C_HANDLE_H

#include "../../def.h"
#include "../../data_structure/queue/queue.h"
#include "I2C.h"
#include <stdint.h>

typedef enum{
    I2C_DIR_READ = 0UL,
    I2C_DIR_WRITE = 1UL
}I2C_direction_t;

typedef enum I2C_state_machine{
    I2C_STATE_SB_SENT = 0,
    I2C_STATE_SLAVE_ADDR_SENT,
    I2C_STATE_BUSY,
    I2C_STATE_DONE
} I2C_state_machine_t;

typedef enum{
    I2C_ERR_NONE = 0,
    I2C_ERR_DIRECTION_VAL,
    I2C_ERR_AF
}I2C_error_t;

typedef struct{
    I2C_t* i2c_device;
    I2C_direction_t direction;
    queue_t* rx_queue;
    queue_t* tx_queue;
    volatile I2C_state_machine_t state;
    volatile I2C_error_t error_code;
    volatile uint16_t transfer_size;
    volatile uint16_t rx_count;
    uint8_t peripheral_write_addr;
    void (*transfer_complete_cb)(void);
}I2C_handle_t;

I2C_handle_t* I2C_handle_init(
I2C_t* i2c_device, I2C_instance_t i2c_instance, queue_t* rx_queue,
queue_t* tx_queue, uint8_t peripheral_write_addr, void (*transfer_complete_cb)(void));


void I2C_write(I2C_handle_t* self);
void I2C_read(I2C_handle_t* self, uint16_t transfer_size);

void I2C1_EV_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C3_EV_IRQHandler(void);

void I2C1_ER_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void I2C3_ER_IRQHandler(void);
#endif