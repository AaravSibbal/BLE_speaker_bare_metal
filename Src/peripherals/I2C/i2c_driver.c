#include "i2c_driver.h"
#include "Src/def.h"


struct I2C_driver{
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t OAR1;
    __IO uint32_t OAR2;
    __IO uint32_t DR;
    __IO uint32_t SR1;
    __IO uint32_t SR2;
    __IO uint32_t CCR;
    __IO uint32_t TRISE;
    __IO uint32_t FLTR;
};

#define SWRST_BIT 15

__INLINE void I2C_en_reset(I2C_driver_t* driver){
    volatile uint32_t addr = (uint32_t)&driver->CR1;
    bit_band_write(addr, SWRST_BIT, 1);
    (void)driver->CR2; // dummy read
    bit_band_write(addr, SWRST_BIT, 0);
    (void)driver->CR2; // dummy read
}

#define ITEVTEN_BIT 9

// DM00031020: 27.6.2: Bit 9 ITEVTEN 
__INLINE void I2C_en_interrupts(I2C_driver_t* driver){
    volatile uint32_t addr = (uint32_t)&driver->CR2;
    bit_band_write(addr, ITEVTEN_BIT, 1);
}


#define ITERREN_BIT 8UL

// DM00031020: 27.6.2: Bit 8 ITERREN 
__INLINE void I2C_en_errors(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR2;
    bit_band_write(addr, ITERREN_BIT, 1);
}

#define ITBUFFEN_BIT 10UL

__INLINE void I2C_en_buffer(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR2;
    bit_band_write(addr, ITBUFFEN_BIT, 1);
}

__INLINE void I2C_dis_buffer(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR2;
    bit_band_write(addr, ITBUFFEN_BIT, 0);
}


#define ACK_BIT 10

__INLINE void I2C_en_ack(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR1;
    bit_band_write(addr, ACK_BIT, 1);
}

__INLINE void I2C_dis_ack(I2C_driver_t* driver){
	uint32_t addr = (uint32_t)&driver->CR1;
	bit_band_write(addr, ACK_BIT, 0);
}

#define I2C_STANDARD_FREQ (100000)

uint32_t I2C_get_freq(I2C_mode_t mode){
	if(mode != I2C_MODE_STANDARD){
		return 0;
	}
	return ((uint32_t)I2C_STANDARD_FREQ);
}

/**
27.6.2 I2C Control register 2 (I2C_CR2)
Bits 5:0 FREQ[5:0]: Peripheral clock frequency
*/

#define I2C_FREQ_BIT 0UL
#define I2C_FREQ_FIELD_LEN 6UL

__INLINE void I2C_set_freq_bits(I2C_driver_t* driver, uint32_t clock_spd_Mhz){
    rwm32_hw(
        &driver->CR2,
        I2C_FREQ_BIT,
        I2C_FREQ_FIELD_LEN,
        clock_spd_Mhz
    );
}

#define I2C_CCR_BIT 0UL
#define I2C_CCR_FLIED_LEN 12UL

__INLINE void I2C_set_ccr(I2C_driver_t* driver, uint32_t ccr_val){
    rwm32_hw(
        &driver->CCR,
        I2C_CCR_BIT, 
        I2C_CCR_FLIED_LEN, 
        ccr_val
    );
}

#define I2C_TRISE_BIT 0
#define I2C_TRISE_FIELD_LEN 6

__INLINE void I2C_set_trise(I2C_driver_t* driver, uint32_t trise_val){
    // sanitizing the data
    trise_val &= (msk_of_ones(I2C_TRISE_FIELD_LEN));
    rwm32_hw(
        &driver->TRISE,
        I2C_TRISE_BIT,
        I2C_TRISE_FIELD_LEN, 
        trise_val
    );
}

#define I2C_PERIPHERAL_EN_BIT 0UL

__INLINE void I2C_en_peripheral(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR1;
    bit_band_write(addr, I2C_PERIPHERAL_EN_BIT, 1);
}

#define CR1_START_BIT 8UL

__INLINE void I2C_start_gen(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR1;
    bit_band_write(addr, CR1_START_BIT, 1);
}

#define CR1_STOP_BIT 9UL

__INLINE void I2C_stop_gen(I2C_driver_t* driver){
    uint32_t addr = (uint32_t)&driver->CR1;
    bit_band_write(addr, CR1_STOP_BIT, 1);
}

__INLINE void I2C_write_to_DR(I2C_driver_t* driver, uint8_t payload){
    driver->DR = (uint32_t)payload;
}


uint8_t I2C_get_DR_val(I2C_driver_t* driver){
    return (uint8_t)driver->DR;
}

__INLINE uint32_t I2C_get_SR1(I2C_driver_t* driver){
    return driver->SR1;
}

__INLINE uint32_t I2C_get_SR2(I2C_driver_t* driver){
    return driver->SR2;
}

#define I2C_CR1_POS_BIT 11UL

__INLINE void I2C_en_POS(I2C_driver_t* driver){
    bit_band_write(((uint32_t)&driver->CR1), I2C_CR1_POS_BIT, 1);
}

__INLINE void I2C_dis_POS(I2C_driver_t* driver){
    bit_band_write(((uint32_t)&driver->CR1), I2C_CR1_POS_BIT, 0);
}
