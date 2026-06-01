#include "i2c_handle.h"
#include "i2c_driver.h"

static i2c_handle_static_pool[3];



#define ADDR_BIT 1UL
#define ADDR_BIT_MSK (1UL<<ADDR_BIT)

#define TxE_BIT 7UL
#define TxE_BIT_MSK (1UL<<TxE_BIT)

#define BTF_BIT 2UL
#define BTF_BIT_MSK (1UL<<BTF_BIT)



I2C_handle_t* I2C_handle_init(
I2C_t* i2c_device, I2C_instance_t i2c_instance,queue_cap_t capacity,
uint8_t* buffer, I2C_direction_t direction)
{
    I2C_handle_t* i2c_handle = &i2c_handle_static_pool[i2c_instance];
    i2c_handle->i2c_device = I2C_get_instance(i2c_instance);
    i2c_handle->queue = queue_init(buffer, capacity, i2c_instance);
    if(i2c_handle->queue == NULL){
        return NULL;
    }
    i2c_handle->direction = direction;
    return i2c_handle;
}

__bool I2C_write(I2C_handle_t* self){
    BARE_ASSERT(expr);
    I2C_start_gen(self->i2c_device->driver);
}

void I2C1_EV_IRQHandler(){
    I2C_handle_t* i2c1_handle = &i2c_handle_static_pool[0];
    BARE_ASSERT(i2c1_handle != NULL);
    BARE_ASSERT(i2c1_handle->queue != NULL);
    BARE_ASSERT(i2c1_handle->i2c_device != NULL);

    if(i2c1_handle->direction == I2C_DIR_READ){
        I2C_dis_ack(i2c1_handle->i2c_device->driver);
    }
    // already clears the addr if it is active
    uint32_t i2c_sr1 = I2C_get_SR1(i2c1_handle->i2c_device->driver);
    uint32_t i2c_sr2 = i2c_sr2
    uint8_t payload;
    if(i2c1_handle->direction == I2C_DIR_WRITE){
        if(i2c_sr1 & TxE_BIT_MSK){
            /**
                if is_valid = true
                    this means we have more data
                if is_valus = false;
                    this mean we ran out of data
            */
            __bool is_valid = queue_dequeue(i2c1_handle->queue, &payload); 
            if(!is_valid && (i2c_sr1 & BTF_BIT_MSK)){
                I2C_stop_gen(i2c1_handle->i2c_device->driver);
            }
            else if(!is_valid && !(i2c_sr1 & BTF_BIT_MSK)){
                I2C_dis_buffer(i2c1_handle->i2c_device->driver);
            }
            else if(is_valid){
                I2C_write_to_DR(i2c1_handle->i2c_device->driver);
            }
        }
    }
    else if (i2c1_handle->direction == I2C_DIR_READ) {
        
    }
    
    __DSB();
}