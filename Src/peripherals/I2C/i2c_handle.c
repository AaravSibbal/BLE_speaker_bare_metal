#include "i2c_handle.h"
#include "../../arm/arm.h"
#include "../../assert.h"
#include "Src/arm/arm.h"
#include "Src/peripherals/gpio/gpio.h"
#include "Src/peripherals/i2c/i2c_driver.h"

static I2C_handle_t i2c1_handle;
static I2C_handle_t i2c2_handle;
static I2C_handle_t i2c3_handle;

I2C_handle_t* I2C_handle_init(
I2C_t* i2c_device, I2C_instance_t i2c_instance, queue_t* rx_queue,
queue_t* tx_queue ,uint8_t peripheral_write_addr, 
void (*transfer_complete_cb)(void))
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

    BARE_ASSERT(rx_queue != NULL);
    BARE_ASSERT(tx_queue != NULL);

	i2c_handle->rx_queue = rx_queue;
	i2c_handle->tx_queue = tx_queue;
    i2c_handle->i2c_device = I2C_get_instance(i2c_instance);
    i2c_handle->peripheral_write_addr = peripheral_write_addr;
    i2c_handle->error_code = I2C_ERR_NONE;
	i2c_handle->transfer_size = 0;
	i2c_handle->transfer_complete_cb = transfer_complete_cb;
    return i2c_handle;
}


void I2C_write(I2C_handle_t* self){
    BARE_ASSERT(self->rx_queue != NULL);
    BARE_ASSERT(self->tx_queue != NULL);
    BARE_ASSERT(self->i2c_device != NULL);
    BARE_ASSERT(self->i2c_device->driver != NULL);
	BARE_ASSERT(queue_is_empty(self->tx_queue) == FALSE);
	BARE_ASSERT(!(self->i2c_device->driver->CR1 & (1UL << 9)));

	I2C_en_interrupts(self->i2c_device->driver);
	self->direction = I2C_DIR_WRITE;
    self->state = I2C_STATE_SB_SENT;
    I2C_start_gen(self->i2c_device->driver);
}

void I2C_read(I2C_handle_t* self, uint16_t transfer_size){
    BARE_ASSERT(self->rx_queue != NULL);
    BARE_ASSERT(self->tx_queue != NULL);
    BARE_ASSERT(self->i2c_device != NULL);
    BARE_ASSERT(self->i2c_device->driver != NULL);

	I2C_en_interrupts(self->i2c_device->driver);
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


__STATIC_INLINE void I2C_ev_handler(I2C_handle_t* i2c_handle){

	uint32_t dummy_read;
    uint32_t i2c_sr1 = I2C_get_SR1(i2c_handle->i2c_device->driver);
    uint32_t i2c_sr2;
    if(i2c_sr1 & START_BIT_MSK){
        if(i2c_handle->state == I2C_STATE_SB_SENT){
            switch(i2c_handle->direction){
                case I2C_DIR_READ:
                    I2C_write_to_DR(
                        i2c_handle->i2c_device->driver,
                        i2c_handle->peripheral_write_addr + 1
                    );
                    break;
                case I2C_DIR_WRITE:
                    I2C_write_to_DR(
                        i2c_handle->i2c_device->driver,
                        i2c_handle->peripheral_write_addr
                    );
                    break;
                default:
                    i2c_handle->error_code = I2C_ERR_DIRECTION_VAL;
            }
            i2c_handle->state = I2C_STATE_SLAVE_ADDR_SENT;
        }
    }

    else if(i2c_handle->direction == I2C_DIR_READ){
        if(i2c_sr1 & ADDR_BIT_MSK){
            if(i2c_handle->transfer_size == 1){
                // disable ack bit before clearing addr
                I2C_dis_ack(i2c_handle->i2c_device->driver);
                // clear addr
				dummy_read = I2C_get_SR1(i2c_handle->i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c_handle->i2c_device->driver);
				(void)dummy_read;
				(void)i2c_sr2;
                // generate stop
                I2C_stop_gen(i2c_handle->i2c_device->driver);
            }
            else if(i2c_handle->transfer_size == 2){
				I2C_dis_ack(i2c_handle->i2c_device->driver);
				I2C_en_POS(i2c_handle->i2c_device->driver);
                dummy_read = I2C_get_SR1(i2c_handle->i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c_handle->i2c_device->driver);
				(void)dummy_read;
				(void)i2c_sr2;
				// we have now cleared addr
            }
			else{
                dummy_read = I2C_get_SR1(i2c_handle->i2c_device->driver);
                i2c_sr2 = I2C_get_SR2(i2c_handle->i2c_device->driver);
				(void)dummy_read;
				(void)i2c_sr2;
			}
            i2c_handle->state = I2C_STATE_BUSY;

			I2C_en_buffer(i2c_handle->i2c_device->driver);
		}
        else if(i2c_sr1 & BTF_BIT_MSK){
			if(i2c_handle->transfer_size == 2){
				I2C_stop_gen(i2c_handle->i2c_device->driver);
				queue_enqueue(
					i2c_handle->rx_queue,
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				queue_enqueue(
					i2c_handle->rx_queue,
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				// reset everything and move on;
				I2C_dis_POS(i2c_handle->i2c_device->driver);
				I2C_en_ack(i2c_handle->i2c_device->driver);
				i2c_handle->state = I2C_STATE_DONE;
				if(i2c_handle->transfer_complete_cb != NULL){
					i2c_handle->transfer_complete_cb();
				}
			}
			else if(
				((i2c_handle->transfer_size - i2c_handle->rx_count) == 3)
				&& (i2c_handle->transfer_size > 2)
			)
			{
				I2C_dis_ack(i2c_handle->i2c_device->driver);
				queue_enqueue(
					i2c_handle->rx_queue, 
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				i2c_handle->rx_count++;
			}
			else if(
				((i2c_handle->transfer_size - i2c_handle->rx_count) == 2)
				&& (i2c_handle->transfer_size > 2)
			)
			{
				I2C_stop_gen(i2c_handle->i2c_device->driver);
				I2C_en_ack(i2c_handle->i2c_device->driver);
				queue_enqueue(
					i2c_handle->rx_queue, 
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				queue_enqueue(
					i2c_handle->rx_queue, 
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				i2c_handle->state = I2C_STATE_DONE;
				if(i2c_handle->transfer_complete_cb != NULL){
					i2c_handle->transfer_complete_cb();
				}
			}
		}
		else if(i2c_sr1 & RxNE_BIT_MSK){
			if(i2c_handle->transfer_size == 1){
				queue_enqueue(
					i2c_handle->rx_queue,
					I2C_get_DR_val(i2c_handle->i2c_device->driver)
				);
				I2C_en_ack(i2c_handle->i2c_device->driver);
				i2c_handle->state = I2C_STATE_DONE;
				if(i2c_handle->transfer_complete_cb != NULL){
					i2c_handle->transfer_complete_cb();
				}
			}
			else if(i2c_handle->transfer_size > 2){
				if(i2c_handle->transfer_size - i2c_handle->rx_count > 3){
					queue_enqueue(
						i2c_handle->rx_queue,
						I2C_get_DR_val(i2c_handle->i2c_device->driver)
					);
					i2c_handle->rx_count++;
				}
			}
		}
    }
    else if(i2c_handle->direction == I2C_DIR_WRITE){
		// 1. Always do the dummy reads to ensure ADDR is cleared if it just set
        dummy_read = I2C_get_SR1(i2c_handle->i2c_device->driver);
        i2c_sr2 = I2C_get_SR2(i2c_handle->i2c_device->driver);
        (void)dummy_read;
        (void)i2c_sr2;

        // 2. Enable buffer interrupt now that ADDR is clear, so TxE can pull from the queue
        I2C_en_buffer(i2c_handle->i2c_device->driver);

        // 3. Handle BTF FIRST (End of transfer condition)
        if(i2c_sr1 & BTF_BIT_MSK){
            I2C_stop_gen(i2c_handle->i2c_device->driver);
            I2C_dis_buffer(i2c_handle->i2c_device->driver); // Keep it off!
			I2C_dis_interrupts(i2c_handle->i2c_device->driver);
            i2c_handle->state = I2C_STATE_DONE;
            if(i2c_handle->transfer_complete_cb != NULL){
                i2c_handle->transfer_complete_cb();
            }
        }
        // 4. Handle TxE (Dumping data into DR)
        else if(i2c_sr1 & TxE_BIT_MSK){
            uint8_t payload;
            if(queue_dequeue(i2c_handle->tx_queue, &payload)){
                i2c_handle->state = I2C_STATE_BUSY;
                I2C_write_to_DR(i2c_handle->i2c_device->driver, payload);
            } else {
                // Queue is empty. Disable buffer interrupt so we don't get trapped in an endless TxE loop.
                // The hardware will now shift out the very last byte on the bus and trigger BTF when done.
                I2C_dis_buffer(i2c_handle->i2c_device->driver);
            }
        }
    }
	__DSB();
}

void I2C1_EV_IRQHandler(void){
	I2C_ev_handler(&i2c1_handle);
}

void I2C2_EV_IRQHandler(void){
	I2C_ev_handler(&i2c2_handle);
}

void I2C3_EV_IRQHandler(void){
	I2C_ev_handler(&i2c3_handle);
}

void I2C1_ER_IRQHandler(void){
    uint32_t sr1 = I2C_get_SR1(i2c1_handle.i2c_device->driver);
    __BKPT(0);
    // Check if the error was an Acknowledge Failure (NACK)
    if(sr1 & (1UL << 10)){
        // Clear the AF bit by writing 0 to it
		I2C_clear_AF(i2c1_handle.i2c_device->driver);
        
        // Generate a STOP condition to free the bus
        I2C_stop_gen(i2c1_handle.i2c_device->driver);
		i2c1_handle.error_code = I2C_ERR_AF;
        i2c1_handle.state = I2C_STATE_DONE; 
		__BKPT(0);
    }
	// bus error
	if(sr1 & (1UL<<8)){
		__BKPT(0);
	}
	if(sr1 & (1UL<<9)){
		__BKPT(0);
	}
	if(sr1 & (1UL<<11)){
		__BKPT(0);
	}
}