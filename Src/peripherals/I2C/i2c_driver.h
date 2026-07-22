#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "../../def.h"

typedef struct I2C_driver I2C_driver_t;

typedef enum I2C_mode{
    I2C_MODE_STANDARD = 0,
    I2C_MODE_FAST = 1
}I2C_mode_t;

void I2C_en_reset(I2C_driver_t* driver);
void I2C_en_interrupts(I2C_driver_t* driver);
void I2C_dis_interrupts(I2C_driver_t* driver);
void I2C_en_errors(I2C_driver_t* driver);
void I2C_en_buffer(I2C_driver_t* driver);
void I2C_dis_buffer(I2C_driver_t* driver);
void I2C_en_ack(I2C_driver_t* driver);
void I2C_dis_ack(I2C_driver_t* driver);
uint32_t I2C_get_freq(I2C_mode_t mode);
void I2C_set_freq_bits(I2C_driver_t* driver, uint32_t clock_spd_Mhz);
void I2C_set_ccr(I2C_driver_t* driver, uint32_t ccr_val);
void I2C_set_trise(I2C_driver_t* driver, uint32_t trise_val);
void I2C_en_peripheral(I2C_driver_t* driver);
void I2C_dis_peripheral(I2C_driver_t* driver);
void I2C_start_gen(I2C_driver_t* driver);
void I2C_stop_gen(I2C_driver_t* driver);
void I2C_write_to_DR(I2C_driver_t* driver, uint8_t payload);
uint8_t I2C_get_DR_val(I2C_driver_t* driver);
uint32_t I2C_get_SR1(I2C_driver_t* driver);
uint32_t I2C_get_SR2(I2C_driver_t* driver);
void I2C_en_POS(I2C_driver_t* driver);
void I2C_dis_POS(I2C_driver_t* driver);
void I2C_clear_AF(I2C_driver_t* driver);
#endif
