#ifndef I2S_H
#define I2S_H

#include "../rcc/rcc.h"
#include "Src/peripherals/dma/dma.h"
#include "Src/peripherals/spi/spi_driver.h"
typedef enum I2S_instance{
    I2S_INSTANCE_2 = 0,
    I2S_INSTANCE_3 = 1,
}I2S_instance_t;

typedef enum I2S_mode{
    I2S_MODE_POLLING = 0,
    I2S_MODE_DMA_RX = 1,
    I2S_MODE_DMA_TX = 2
}I2S_mode_t;

typedef enum I2S_error{
    I2S_ERROR_NONE = 0,
    I2S_ERROR_OVR = 1,
    I2S_ERROR_UDR = 2,
    I2S_ERROR_FRE = 3
}I2S_error_t;

typedef struct I2S_handle{
    SPI_driver_t* driver;
    I2S_error_t error_state;
}I2S_handle_t;

// typedef struct I2S_DMA_data{

// }I2S_DMA_data_t;


I2S_handle_t* i2s_configure(I2S_instance_t instance, RCC_t* rcc, I2S_mode_t mode, 
DMA_callback_t hc_cb, DMA_callback_t tc_cb, DMA_callback_t error_cb,
void* user_data, uint32_t mem0_addr, uint32_t mem1_addr);

void i2s_init(I2S_handle_t* self);


void SPI3_IRQHandler(void);
#endif