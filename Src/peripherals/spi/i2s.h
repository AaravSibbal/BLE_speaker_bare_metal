#ifndef I2S_H
#define I2S_H

#include "../rcc/rcc.h"
typedef enum I2S_instance{
    I2S_INSTANCE_2 = 0,
    I2S_INSTANCE_3 = 1
}I2S_instance_t;

typedef enum I2S_mode{
    I2S_MODE_POLLING = 0,
    I2S_MODE_DMA_RX = 1,
    I2S_MODE_DMA_TX = 2
}I2S_mode_t;

void i2s_init(I2S_instance_t instance, RCC_t* rcc, I2S_mode_t mode);

void SPI3_IRQHandler(void);
#endif