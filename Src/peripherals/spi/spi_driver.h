
#ifndef SPI_DRIVER
#define SPI_DRIVER

#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include <stdint.h>

typedef enum SPI_instance{
    SPI_INSTANCE_1 = 0,
    SPI_INSTANCE_2,
    SPI_INSTANCE_3,
    SPI_INSTANCE_4
}SPI_instance_t;

typedef enum SPI_mode{
    SPI_SPI_MODE = 0,
    SPI_I2S_MODE
}SPI_mode_t;

typedef enum I2S_conf{
    I2S_SLAVE_TRANSMIT = 0x0,
    I2S_SLAVE_RECEIVE = 0x1,
    I2S_MASTER_TRANSMIT = 0x2,
    I2S_MASTER_RECEIVE = 0x3
}I2S_conf_t;

typedef enum I2S_std{
    I2S_PHILLIPS = 0x0,
    I2S_MSB_ = 0x1,
    I2S_LSB = 0x2,
    I2S_PCM = 0x3
}I2S_std_t;

typedef enum I2S_data_len{
    I2S_DATA_LEN_16 = 0x0,
    I2S_DATA_LEN_24 = 0x1,
    I2S_DATA_LEN_32 = 0x2,
}I2S_data_len_t;

typedef enum I2S_chan_len{
    I2S_CHAN_LEN_16 = 0x0,
    I2S_CHAN_LEN_32 = 0x1,
}I2S_chan_len_t;

typedef enum I2S_ckpol{
    I2S_CKPOL_LOW = 0x0,
    I2S_CKPOL_HIGH = 0x1
}I2S_ckpol_t;

typedef struct SPI_driver SPI_driver_t;

SPI_driver_t* SPI_get_instance(SPI_instance_t instance);
void SPI_en_MCK(SPI_driver_t* self);
void SPI_dis_MCK(SPI_driver_t* self);
void SPI_set_ODD_bit(SPI_driver_t* self);
void SPI_reset_ODD_bit(SPI_driver_t* self);
/**
    val can't be 0 or 1 because division
*/
__bool SPI_set_I2SDIV(SPI_driver_t* self, uint8_t val);
void SPI_set_mode(SPI_driver_t* self, SPI_mode_t mode);
void SPI_en_I2S(SPI_driver_t* self);
void SPI_dis_I2S(SPI_driver_t* self);
void SPI_set_I2S_conf(SPI_driver_t* self, I2S_conf_t conf);
void SPI_set_I2S_std(SPI_driver_t* self, I2S_std_t std);
void SPI_set_I2S_data_len(SPI_driver_t* self, I2S_data_len_t len);
void SPI_set_I2S_chan_len(SPI_driver_t* self, I2S_chan_len_t len);
void SPI_set_I2S_ckpol(SPI_driver_t* self, I2S_ckpol_t val);


#endif