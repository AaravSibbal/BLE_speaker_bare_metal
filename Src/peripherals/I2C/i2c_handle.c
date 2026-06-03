#include "i2c_handle.h"
#include "Src/arm/arm.h"
#include "Src/assert.h"
#include "Src/data_structure/queue/queue.h"
#include "Src/def.h"
#include "i2c_driver.h"
#include "../../arm/arm.h"
#include <stdint.h>

static I2C_handle_t i2c1_handle;
static I2C_handle_t i2c2_handle;
static I2C_handle_t i2c3_handle;

static queue_t i2c1_rx_queue;
static queue_t i2c1_tx_queue;
static queue_t i2c2_tx_queue;
static queue_t i2c2_rx_queue;
static queue_t i2c3_tx_queue;
static queue_t i2c3_rx_queue;

static void I2C_init_queues(I2C_handle_t* handle, uint8_t *rx_buffer, 
uint8_t* tx_buffer, queue_cap_t capacity, I2C_instance_t instance){
    switch (instance) {
        case I2C_1:
            handle->rx_queue = queue_init(&i2c1_rx_queue, rx_buffer, capacity);
            handle->tx_queue = queue_init(&i2c1_tx_queue, tx_buffer, capacity);
            break;
        case I2C_2:
            handle->rx_queue = queue_init(&i2c2_rx_queue, rx_buffer, capacity);
            handle->tx_queue = queue_init(&i2c2_tx_queue, tx_buffer, capacity);
            break;
        case I2C_3:
            handle->rx_queue = queue_init(&i2c3_rx_queue, rx_buffer, capacity);
            handle->tx_queue = queue_init(&i2c3_tx_queue, tx_buffer, capacity);
            break;
        default:
            return;
    }
}

I2C_handle_t* I2C_handle_init(
I2C_t* i2c_device, I2C_instance_t i2c_instance, queue_cap_t capacity,
uint8_t* rx_buffer, uint8_t* tx_buffer, uint8_t peripheral_write_addr)
{
    I2C_handle_t* i2c_handle;
    switch (i2c_instance) {
        case I2C_1:
            i2c_handle = &i2c1_handle;
            break;
        case I2C_2:
            i2c_handle = &i2c2_handle;
            break;
        case I2C_3:
            i2c_handle = &i2c3_handle;
            break;
        default:
            i2c_handle = NULL;
    }
    if(i2c_handle == NULL){
        return i2c_handle;
    }

    I2C_init_queues(
        i2c_handle,
        rx_buffer, 
        tx_buffer, 
        capacity, 
        i2c_instance
    );

    BARE_ASSERT(i2c_handle->rx_queue != NULL);
    BARE_ASSERT(i2c_handle->tx_queue != NULL);
    i2c_handle->i2c_device = I2C_get_instance(i2c_instance);
    i2c_handle->peripheral_write_addr = peripheral_write_addr;
    i2c_handle->error_code = I2C_ERR_NONE;
	i2c_handle->transfer_size = 0;
    return i2c_handle;
}

void I2C_write(I2C_handle_t* self){
	I2C_en_buffer(self->i2c_device->driver);
	self->direction = I2C_DIR_WRITE;
    self->state = I2C_STATE_SB_SENT;
    I2C_start_gen(self->i2c_device->driver);
}

void I2C_read(I2C_handle_t* self, uint16_t transfer_size){
	I2C_en_ack(self->i2c_device->driver);
	self->transfer_size = transfer_size;
	self->direction = I2C_DIR_READ;
	self->state = I2C_STATE_SB_SENT;
	self->rx_count = 0;
	I2C_start_gen(self->i2c_device->driver);
}

#define ADDR_BIT 1UL
#define ADDR_BIT_MSK (1UL<<ADDR_BIT)

#define TxE_BIT 7UL
#define TxE_BIT_MSK (1UL<<TxE_BIT)

#define BTF_BIT 2UL
#define BTF_BIT_MSK (1UL<<BTF_BIT)

#define START_BIT 0UL
#define START_BIT_MSK (1UL<<START_BIT)

#define RxNE_BIT 6UL
#define RxNE_BIT_MSK (1UL<<RxNE_BIT)

void I2C1_EV_IRQHandler(){
    BARE_ASSERT(i2c1_handle.rx_queue != NULL);
    BARE_ASSERT(i2c1_handle.tx_queue != NULL);
    BARE_ASSERT(i2c1_handle.i2c_device != NULL);
    BARE_ASSERT(i2c1_handle.i2c_device->driver != NULL);
    
	uint32_t dummy_read;
    uint32_t i2c_sr1 = I2C_get_SR1(i2c1_handle.i2c_device->driver);
    uint32_t i2c_sr2;
    if(i2c_sr1 & START_BIT_MSK){
        if(i2c1_handle.state == I2C_STATE_SB_SENT){
            switch(i2c1_handle.direction){
                case I2C_DIR_READ:
                    I2C_write_to_DR(
                        i2c1_handle.i2c_device->driver,
                        i2c1_handle.peripheral_write_addr | 1
                    );
                    break;
                case I2C_DIR_WRITE:
                    I2C_write_to_DR(
                        i2c1_handle.i2c_device->driver,
                        i2c1_handle.peripheral_write_addr
                    );
                    break;
                default:
                    i2c1_handle.error_code = I2C_ERR_DIRECTION_VAL;
            }
            i2c1_handle.state = I2C_STATE_SLAVE_ADDR_SENT;
        }
    }

    if(i2c1_handle.direction == I2C_DIR_READ){
        if(i2c_sr1 & ADDR_BIT_MSK){
            if(i2c1_handle.transfer_size == 1){
                // disable ack bit before clearing addr
                I2C_dis_ack(i2c1_handle.i2c_device->driver);
                // clear addr
				dummy_read = I2C_get_SR1(i2c1_handle.i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c1_handle.i2c_device->driver);
                // generate stop
                I2C_stop_gen(i2c1_handle.i2c_device->driver);
            }
            else if(i2c1_handle.transfer_size == 2){
				I2C_dis_ack(i2c1_handle.i2c_device->driver);
				I2C_en_POS(i2c1_handle.i2c_device->driver);
                dummy_read = I2C_get_SR1(i2c1_handle.i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c1_handle.i2c_device->driver);
				// we have now cleared addr
            }
			else{
                dummy_read = I2C_get_SR1(i2c1_handle.i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c1_handle.i2c_device->driver);
			}
            i2c1_handle.state = I2C_STATE_BUSY;
        }
        else if(i2c_sr1 & BTF_BIT_MSK){
			if(i2c1_handle.transfer_size == 2){
				I2C_stop_gen(i2c1_handle.i2c_device->driver);
				queue_enqueue(
					i2c1_handle.rx_queue,
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
				queue_enqueue(
					i2c1_handle.rx_queue,
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
				// reset everything and move on;
				I2C_dis_POS(i2c1_handle.i2c_device->driver);
				I2C_en_ack(i2c1_handle.i2c_device->driver);
				i2c1_handle.state = I2C_STATE_DONE;
			}
			else if(
				((i2c1_handle.transfer_size - i2c1_handle.rx_count) == 3)
				&& (i2c1_handle.transfer_size > 2)
			)
			{
				I2C_dis_ack(i2c1_handle.i2c_device->driver);
				queue_enqueue(
					i2c1_handle.rx_queue, 
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
				i2c1_handle.rx_count++;
			}
			else if(
				((i2c1_handle.transfer_size - i2c1_handle.rx_count) == 2)
				&& (i2c1_handle.transfer_size > 2)
			)
			{
				I2C_stop_gen(i2c1_handle.i2c_device->driver);
				I2C_en_ack(i2c1_handle.i2c_device->driver);
				i2c1_handle.state = I2C_STATE_DONE;
				queue_enqueue(
					i2c1_handle.rx_queue, 
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
				queue_enqueue(
					i2c1_handle.rx_queue, 
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
			}
		}
		else if(i2c_sr1 & RxNE_BIT_MSK){
			if(i2c1_handle.transfer_size == 1){
				queue_enqueue(
					i2c1_handle.rx_queue,
					I2C_get_DR_val(i2c1_handle.i2c_device->driver)
				);
				I2C_en_ack(i2c1_handle.i2c_device->driver);
				i2c1_handle.state = I2C_STATE_DONE;
			}
			else if(i2c1_handle.transfer_size > 2){
				if(i2c1_handle.transfer_size - i2c1_handle.rx_count > 3){
					queue_enqueue(
						i2c1_handle.rx_queue,
						I2C_get_DR_val(i2c1_handle.i2c_device->driver)
					);
					i2c1_handle.rx_count++;
				}
			}
		}
    }
    else if(i2c1_handle.direction == I2C_DIR_WRITE){
        // already clears the addr if it is active
		dummy_read = I2C_get_SR1(i2c1_handle.i2c_device->driver);
        i2c_sr2 = I2C_get_SR2(i2c1_handle.i2c_device->driver);
        uint8_t payload;
		if(i2c_sr1 & TxE_BIT_MSK){
			/**
				if is_valid = true
					this means we have more data
				if is_valus = false;
					this mean we ran out of data
			*/
			__bool is_valid = queue_dequeue(i2c1_handle.tx_queue, &payload); 
			if(!is_valid && (i2c_sr1 & BTF_BIT_MSK)){
				I2C_stop_gen(i2c1_handle.i2c_device->driver);
				i2c1_handle.state = I2C_STATE_DONE;
			}
			else if(!is_valid && !(i2c_sr1 & BTF_BIT_MSK)){
				I2C_dis_buffer(i2c1_handle.i2c_device->driver);
			}
			else if(is_valid){
				I2C_write_to_DR(i2c1_handle.i2c_device->driver, payload);
			}
		}
    }
	__DSB();
}