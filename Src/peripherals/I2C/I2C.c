#include "I2C.h"
#include "I2C.H"

#include "../../assert.h"
#include "Src/arm/arm.h"
#include "Src/assert.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"
#include <cstddef>
#include <stdint.h>

#define I2C1 (0x40005400)
#define I2C2 (0x40005800)
#define I2C3 (0x40005400)
#define MHZ_TO_HZ_MULTIPLIER (1000000)
#define I2C_STANDARD_FREQ (100000)

static I2C_t i2c_static_pool[3];

static const GPIO_AFx_t I2C_GPIO_ALT_FUNC_MAPPING = AF4;
static const GPIO_MODER_t I2C_GPIO_MODE = GPIO_MODE_ALT;
static const GPIO_OTYPER_t I2C_GPIO_TYPE = GPIO_TYPE_OPEN_DRAIN;
static const GPIO_PUPDR_t I2C_GPIO_PUPDR_VAL = PULL_UP;

static I2C_driver_t* I2C_get_instance(const I2C_instance_t instance){
    switch(instance){
        case I2C_1:
            return ((I2C_driver_t *) (I2C1));
            break;
        case I2C_2:
            return ((I2C_driver_t *) (I2C2));
            break;
        case I2C_3:
            return ((I2C_driver_t *) (I2C3));
            break;
        default:
            return NULL;
    } 
}

i2c_driver_t* I2C1_instance = I2C_get_instance(I2C_1);

#define SWRST_BIT 15

__STATIC_INLINE void I2C_en_reset(I2C_driver_t* driver){
    BARE_ASSERT(self != NULL);
    uint32_t addr = &driver->CR1;
    bit_band_write(addr, SWRST_BIT, 1);
    (void)driver->CR2 // dummy read
    bit_band_write(addr, SWRST_BIT, 0);
    (void)driver->CR2 // dummy read
}

#define ITEVTEN_BIT 9

// DM00031020: 27.6.2: Bit 9 ITEVTEN 
// this interrupt is generated when:
// – SB = 1 (Master) (sr1 bit 0 set by hardware when )
// – ADDR = 1 (Master/Slave)
// – ADD10= 1 (Master)
// – STOPF = 1 (Slave)
// – BTF = 1 with no TxE or RxNE event
// – TxE event to 1 if ITBUFEN = 1
// – RxNE event to 1if ITBUFEN = 1
__STATIC_INLINE void I2C_en_interrupts(I2C_driver_t* driver){
    BARE_ASSERT(self != NULL);
    uint32_t addr = &driver->CR2;
    bit_band_write(addr, ITEVTEN_BIT, 1);
}


#define ITERREN_BIT 8UL

// DM00031020: 27.6.2: Bit 8 ITERREN 
// This interrupt is generated when:
// – BERR = 1
// – ARLO = 1
// – AF = 1
// – OVR = 1
// – PECERR = 1
// – TIMEOUT = 1
// – SMBALERT = 1
__STATIC_INLINE void I2C_en_errors(I2C_driver_t* driver){
    BARE_ASSERT(self != NULL);
    uint32_t addr = &driver->CR2;
    bit_band_write(addr, ITERREN_BIT, 1);
}

#define ITBUFFEN_BIT 10UL

__STATIC_INLINE void I2C_en_buffer(I2C_driver_t* driver){
    BARE_ASSERT(self != NULL);
    uint32_t addr = &driver->CR2;
    bit_band_write(addr, ITBUFFEN_BIT, 1);
}


#define ACK_BIT 10

__STATIC_INLINE void I2C_en_ack(I2C_driver_t* driver){
    BARE_ASSERT(self != NULL);
    uint32_t addr = &driver->CR1;
    bit_band_write(addr, ACK_BIT, 1);
}

__STATIC_INLINE void I2C_conf_gpio(GPIO_t* gpio, GPIO_Pin_t pin){
    GPIO_set_moder(gpio, pin, I2C_GPIO_MODE);
    GPIO_set_otyper(gpio, pin, I2C_GPIO_TYPE);
    GPIO_set_alt_func(gpio, pin, I2C_GPIO_ALT_FUNC_MAPPING);
    GPIO_set_pupdr(gpio, pin, I2C_GPIO_PUPDR_VAL);
}

static uint32_t I2C_get_freq(I2C_mode_t mode){
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

void I2C_set_freq_bits(I2C_driver_t* driver, uint32_t clock_spd_Mhz){
    rwm32_hw(
        &driver->CR2,
        I2C_FREQ_BIT,
        I2C_FREQ_FIELD_LEN,
        clock_spd_Mhz
    );
}

#define I2C_CCR_BIT 0UL
#define I2C_CCR_FLIED_LEN 12UL

void I2C_set_ccr(I2C_driver_t* driver, uint32_t ccr_val){
    rwm32_hw(
        &driver->CCR,
        I2C_CCR_BIT, 
        I2C_CCR_FLIED_LEN, 
        ccr_val
    );
}

#define I2C_TRISE_BIT 0
#define I2C_TRISE_FIELD_LEN 6

void I2C_set_trise(I2C_driver_t* driver, uint32_t trise_val){
    // sanitizing the data
    trise_val &= (msk_of_ones(I2C_TRISE_FIELD_LEN));
    rwm32_hw(
        driver->TRISE,
        I2C_TRISE_BIT,
        I2C_TRISE_FIELD_LEN, 
        trise_val
    );
}

#define I2C_PERIPHERAL_EN_BIT 0

void I2C_en_peripheral(I2C_driver_t* driver){
    BARE_ASSERT(driver != NULL);
    uint32_t addr = &driver->CR1;
    bit_band_write(addr, I2C_PERIPHERAL_EN_BIT, 1);
}

void I2C_en_clock(I2C_instance_t instance, RCC_t* rcc){
    switch (instance) {
        case I2C_1:
            RCC_en_I2C1(rcc);
            break;
        case I2C_2:
            RCC_en_I2C2(rcc);
            break;
        case I2C_3:
            RCC_en_I2C3(rcc);
            break;
        default:
            return;
    }
    return;
}

I2C_t* I2C_init(I2C_instance_t instance, I2C_mode_t mode, 
GPIO_port_t sda_port, GPIO_Pin_t sda_pin, GPIO_port_t scl_port, 
GPIO_Pin_t scl_pin, uint32_t clock_speed_Mhz, RCC_t* rcc){
	if(mode != I2C_MODE_STANDARD){
		// fast mode not implemented yet
		// the rest of the function doesn't care about this fact
		// because of future proofing
		return NULL;
	}

	RCC_en_I2C1(rcc);

	I2C_t* i2c = I2C_get_instance(instance);
	i2c->sda = GPIO_init(sda_port, rcc);
	i2c->scl = GPIO_init(scl_port, rcc);
	i2c->scl_pin = scl_pin;
	i2c->sda_pin = sda_pin;

	I2C_conf_gpio(i2c->scl, scl_pin);
	I2C_conf_gpio(i2c->sda, sda_pin);

	I2C_en_reset(i2c->driver);

	I2C_en_interrupts(i2c->driver);
	I2C_en_errors(i2c->driver);
	I2C_en_buffer(i2c->driver);
    I2C_en_ack(i2c->driver)

	uint32_t clock_spd_hz = clock_speed_Mhz*MHZ_TO_HZ_MULTIPLIER;
	uint32_t i2c_freq = I2C_get_freq(mode);
	uint32_t ccr_val = clock_spd_hz/(2*i2c_freq);
	uint32_t trise_val = clock_speed_Mhz + 1;

    I2C_set_freq_bits(i2c->driver, clock_speed_Mhz);
    I2C_set_ccr(i2c->driver, ccr_val);
    I2C_set_trise(i2c->driver, trise_val);


    I2C_en_peripheral(i2c->driver);
}

#define ADDR_BIT 1UL
#define ADDR_BIT_MSK (1UL<<ADDR_BIT)

#define TxE_BIT 7UL
#define TxE_BIT_MSK (1UL<<TxE_BIT)

void I2C1_EV_IRQHandler(){
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    
    I2C_driver_t* driver = I2C1_instance;

    if(driver->SR1 & ADDR_BIT_MSK){
        // address for the slave matched
        uint32_t dummy_read = driver->SR1;
        dummy_read = driver->SR2;
    }
    if(driver->SR1 & TxE_BIT_MSK)

    __set_PRIMASK(primask);
}
