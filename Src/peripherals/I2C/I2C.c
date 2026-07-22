#include "I2C.h"

#include "../../assert.h"
#include "Src/arm/arm.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include "Src/peripherals/rcc/rcc.h"

#define I2C1 (0x40005400)
#define I2C2 (0x40005800)
#define I2C3 (0x40005C00)
#define MHZ_TO_HZ_MULTIPLIER (1000000)

const GPIO_port_t i2c1_sda_gpio_port = GPIO_PORT_B;
const GPIO_port_t i2c1_scl_gpio_port = GPIO_PORT_B;
const GPIO_Pin_t i2c1_scl_gpio_pin = GPIO_PIN_6;
const GPIO_Pin_t i2c1_sda_gpio_pin = GPIO_PIN_9;

I2C_t i2c1;
I2C_t i2c2;
I2C_t i2c3;

static const GPIO_AFx_t I2C_GPIO_ALT_FUNC_MAPPING = AF4;
static const GPIO_MODER_t I2C_GPIO_MODE = GPIO_MODE_ALT;
static const GPIO_OTYPER_t I2C_GPIO_TYPE = GPIO_TYPE_OPEN_DRAIN;
static const GPIO_PUPDR_t I2C_GPIO_PUPDR_VAL = PULL_UP;



I2C_t* I2C_get_instance(const I2C_instance_t instance){
    I2C_t* i2c;
    switch(instance){
        case I2C_1:
            i2c = &i2c1;
            i2c->driver = ((I2C_driver_t *) (I2C1));
            return i2c;
        case I2C_2:
            i2c = &i2c2;
            i2c->driver = ((I2C_driver_t *) (I2C2));
            return i2c;
        case I2C_3:
            i2c = &i2c3;
            i2c->driver = ((I2C_driver_t *) (I2C3));
            return i2c;
        default:
            return NULL;
    } 
}

__STATIC_INLINE void I2C_conf_gpio(GPIO_t* gpio, GPIO_Pin_t pin){
    GPIO_set_otyper(gpio, pin, I2C_GPIO_TYPE);
    GPIO_set_alt_func(gpio, pin, I2C_GPIO_ALT_FUNC_MAPPING);
    GPIO_set_pupdr(gpio, pin, I2C_GPIO_PUPDR_VAL);
    GPIO_set_moder(gpio, pin, I2C_GPIO_MODE);
}


__STATIC_INLINE void I2C_en_clock(I2C_instance_t instance, RCC_t* rcc){
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

static void I2C_bus_recovery(I2C_t* i2c, GPIO_Pin_t scl_pin, GPIO_Pin_t sda_pin) {
    // 1. Configure BOTH as Output Open-Drain
    GPIO_set_moder(i2c->scl, scl_pin, GPIO_MODE_OUTPUT);
    GPIO_set_otyper(i2c->scl, scl_pin, GPIO_TYPE_OPEN_DRAIN);
    GPIO_set_moder(i2c->sda, sda_pin, GPIO_MODE_OUTPUT);
    GPIO_set_otyper(i2c->sda, sda_pin, GPIO_TYPE_OPEN_DRAIN);

    // 2. Drive both High (Bus Idle)
    GPIO_set_odr(i2c->scl, scl_pin, 1);
    GPIO_set_odr(i2c->sda, sda_pin, 1);
    for(volatile int d = 0; d < 1000; d++);

    // 3. Clear External Slave (9-Clock Pulse)
    for (int i = 0; i < 9; i++) {
        GPIO_set_odr(i2c->scl, scl_pin, 0);
        for(volatile int d = 0; d < 1000; d++);
        GPIO_set_odr(i2c->scl, scl_pin, 1);
        for(volatile int d = 0; d < 1000; d++);
    }

    // 4. Clear STM32 Internal State Machine (Manual START)
    GPIO_set_odr(i2c->sda, sda_pin, 0); // SDA goes Low while SCL is High
    for(volatile int d = 0; d < 1000; d++);
    GPIO_set_odr(i2c->scl, scl_pin, 0); // SCL goes Low
    for(volatile int d = 0; d < 1000; d++);

    // 5. Clear STM32 Internal State Machine (Manual STOP)
    GPIO_set_odr(i2c->scl, scl_pin, 1); // SCL goes High
    for(volatile int d = 0; d < 1000; d++);
    GPIO_set_odr(i2c->sda, sda_pin, 1); // SDA goes High while SCL is High
    for(volatile int d = 0; d < 1000; d++);
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

	I2C_t* i2c = I2C_get_instance(instance);

	i2c->sda = GPIO_init(sda_port, rcc);
	i2c->scl = GPIO_init(scl_port, rcc);
	i2c->scl_pin = scl_pin;
	i2c->sda_pin = sda_pin;

    I2C_bus_recovery(i2c, scl_pin, sda_pin);

    I2C_en_clock(instance, rcc);
    __DSB();

    RCC_reset_I2C1(rcc);
    __DSB();

    I2C_dis_peripheral(i2c->driver);
    __DSB();

	I2C_conf_gpio(i2c->scl, scl_pin);
	I2C_conf_gpio(i2c->sda, sda_pin);

	I2C_en_reset(i2c->driver);
    __DSB();
    
	I2C_en_interrupts(i2c->driver);
	I2C_en_errors(i2c->driver);
    I2C_en_ack(i2c->driver);

	uint32_t clock_spd_hz = clock_speed_Mhz*MHZ_TO_HZ_MULTIPLIER;
	uint32_t i2c_freq = I2C_get_freq(mode);
	uint32_t ccr_val = clock_spd_hz/(2*i2c_freq);
	uint32_t trise_val = clock_speed_Mhz + 1;

    I2C_set_freq_bits(i2c->driver, clock_speed_Mhz);
    I2C_set_ccr(i2c->driver, ccr_val);
    I2C_set_trise(i2c->driver, trise_val);


    I2C_en_peripheral(i2c->driver);
    __DSB();
    return i2c;
}
