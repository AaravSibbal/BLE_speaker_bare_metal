#include "dac.h"
#include "../../peripherals/i2c/i2c_handle.h"
#include "../../data_structure/queue/queue.h"
#include "../../peripherals/spi/i2s.h"
#include "Src/def.h"
#include "Src/peripherals/gpio/gpio.h"
#include <stdint.h>
#include <stddef.h>


static const uint8_t DAC_I2C_addr = 0x94;

static queue_t i2c1_rx_queue;
static queue_t i2c1_tx_queue;
static uint8_t i2c_rx_buffer[1];
static uint8_t i2c_tx_buffer[2];
static queue_t* i2c1_rx_queue_ptr = NULL;
static queue_t* i2c1_tx_queue_ptr = NULL;

static I2C_t* i2c1_device = NULL;
static I2C_handle_t* i2c1_handle = NULL;
 /**

    initialization sequence:
    PD4 in output to high
    Power clt. 1 (0x02): 0x01 (done)
    (also the default value so don't need to change but good to
    do if dac finds itself in a weird state)
    write to address: 0x00 val: 0x99
    write to address: 0x47 val: 0x80
    
    Note: the next 2 steps would require rwm but because they are back to
    back we can do it with 1 read and 2 writes. 
    write to address: 0x32 val: 1b to bit 7
    write to address: 0x32 val: 0b to bit 7
    write to address: 0x00 val: 0x00

    do section 4.6
    read address 0x06
    modify bit 7: M/S to 0
    write the val to address 0x06

    turn on the i2s peripheral
    write to address: 0x02 val: 0x9E

    done

*/

__STATIC_INLINE void dac_write(uint8_t reg_addr, uint8_t val){
    queue_enqueue(i2c1_tx_queue_ptr, reg_addr);
    queue_enqueue(i2c1_tx_queue_ptr, val);
    I2C_write(i2c1_handle);
    while(i2c1_handle->state != I2C_STATE_DONE){}
    while(I2C_get_SR2(i2c1_device->driver) & 0x02){}
}

void dac_init(RCC_t* rcc){
    i2c1_rx_queue_ptr = queue_init(
        &i2c1_rx_queue, 
        i2c_rx_buffer, 
        QUEUE_CAP_1
    );


    i2c1_tx_queue_ptr = queue_init(
        &i2c1_tx_queue, 
        i2c_tx_buffer, 
        QUEUE_CAP_2
    );


    i2c1_device = I2C_init(
        I2C_1, 
        I2C_MODE_STANDARD, 
        i2c1_sda_gpio_port, 
        i2c1_sda_gpio_pin,
        i2c1_scl_gpio_port,
        i2c1_scl_gpio_pin,
        16, 
        rcc
    );
    
    i2c1_handle = I2C_handle_init(
        i2c1_device,
        I2C_1,
        i2c1_rx_queue_ptr, 
        i2c1_tx_queue_ptr, 
        DAC_I2C_addr,
        NULL
    );
    GPIO_set_odr(GPIO_init(GPIO_PORT_D, rcc), GPIO_PIN_4, GPIO_OUTPUT_HIGH);
    for(volatile int i = 0; i < 50000; i++);

    
    dac_write(0x02, 0x01);
    dac_write(0x00, 0x99);
    dac_write(0x47, 0x80);

    queue_enqueue(i2c1_tx_queue_ptr, 0x32);
    I2C_write(i2c1_handle);
    while(i2c1_handle->state != I2C_STATE_DONE){}
    while(I2C_get_SR2(i2c1_device->driver) & 0x02){}
    I2C_read(i2c1_handle, 1);
    while(i2c1_handle->state != I2C_STATE_DONE){}
    uint8_t val;
    queue_dequeue(i2c1_rx_queue_ptr, &val);
    
    val |= (1<<7);
    dac_write(0x32, val);
    val &= ~(1<<7);
    dac_write(0x32, val);

    dac_write(0x00, 0x00);
    
    
    i2s_init(I2S_INSTANCE_3, rcc, I2S_MODE_DMA_TX);
    // 
    /**
        power control 2:
        address: 0x04
        10 10 11 11
        Hex: 0xAF
        headphone A on
        headphone B on
        speaker A off 
        speaker B off
    */
    dac_write(0x04, 0xAF);

    /**
        https://statics.cirrus.com/pubs/proDatasheet/CS43L22_F2.pdf
        clocking control:
        address: 0x05
        1 01 0 0 00 0
        hex: 0xA0 
        Auto detect: on
        speed mode: 4-50kHz
        32kHz: no
        video clock: no
        ratio: 00 (from serial port clocking pg 29)
        mclk_div_2: 0

    */

    dac_write(0x05, 0xA0);
    /**
        interface control 1:
        address: 0x06
        0 0 0 01 11
        hex: 0x07
        M/S : slave mode
        sclk polarity: not inverted
        dsp mode: off
        dac format: i2s standard 
        audio work len: 16-bits (usually ignored) 
    */
    dac_write(0x06, 0x07);
    
    // we are powered up!
    dac_write(0x02, 0x9E);
}