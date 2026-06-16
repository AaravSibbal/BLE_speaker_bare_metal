
#include "Src/peripherals/rcc/rcc.h"
#include "Src/services/interrupts/interrupt.h"
#include "Src/services/print/printf.h"
#include "data_structure/queue/queue.h"
#include "def.h"
#include "devices/LED/LED.h"
#include "devices/button/button.h"
#include "peripherals/I2C/I2C.h"
#include "peripherals/gpio/gpio.h"
#include "peripherals/i2c/I2C.h"
#include "peripherals/i2c/i2c_driver.h"
#include "peripherals/i2c/i2c_handle.h"
#include "peripherals/rcc/rcc.h"
#include "peripherals/spi/spi_driver.h"
#include "peripherals/timers/systick/systick.h"
#include "services/interrupts/interrupt.h"
#include "arm/arm.h"
#include "peripherals/i2c/i2c.h"
#include "devices/dac/dac.h"
#include <stdint.h>

// void something(I2C_handle_t* handle, uint16_t transfer_size){
//     I2C_read(handle, transfer_size);
// }

int main(void){
    // setting up priority
    set_priority_grouping(PRIGROUP_4PRE_0SUB);
    set_priority(I2C1_EV_IRQn, 2);
    set_priority(I2C1_ER_IRQn, 3);
    set_priority(SysTick_IRQn,4);
    set_priority(BusFault_IRQn, 0);
    set_priority(MemoryManagement_IRQn, 0);
    set_priority(UsageFault_IRQn, 0);


    enable_IRQ(SysTick_IRQn);
    enable_IRQ(I2C1_ER_IRQn);
    enable_IRQ(I2C1_EV_IRQn);
    enable_IRQ(BusFault_IRQn);
    enable_IRQ(MemoryManagement_IRQn);
    enable_IRQ(UsageFault_IRQn);

    __DSB();
    __ISB();

    const int16_t sine_wave[48] = {
    0, 1045, 2079, 3090, 4067, 5000, 5877, 6691, 
    7431, 8089, 8660, 9135, 9510, 9781, 9945, 10000, 
    9945, 9781, 9510, 9135, 8660, 8089, 7431, 6691, 
    5877, 5000, 4067, 3090, 2079, 1045, 0, -1045, 
    -2079, -3090, -4067, -5000, -5877, -6691, -7431, -8089, 
    -8660, -9135, -9510, -9781, -9945, -10000, -9945, -9781
    };

    RCC_t* rcc = init_RCC();
    RCC_en_GPIO(rcc, BUTTON_GPIO_PORT);
    RCC_en_GPIO(rcc, GPIO_PORT_D);

    GPIO_t* green_led_gpio = GPIO_init(GPIO_PORT_D, rcc);
    LED_t* green_led = LED_init(LED_GREEN, green_led_gpio);

    GPIO_set_moder(green_led_gpio, GPIO_PIN_4, GPIO_MODE_OUTPUT);
    GPIO_set_otyper(green_led_gpio, GPIO_PIN_4, GPIO_TYPE_PUSH_PULL);
    
    dac_init(rcc);

    SPI_driver_t* spi3 = SPI_get_instance(SPI_INSTANCE_3);

    uint32_t sample_index = 0;
    uint8_t channel_toggle = 0;

    while(1) {
        // Check the TXE (Transmit buffer empty) bit in the Status Register
        // TXE is Bit 1 of SPI_SR
        if (SPI_get_SR(spi3) & (1 << 1)) {
            
            // Write a 16-bit zero to the Data Register
            // This instantly forces the STM32 to start generating the SCK and WS clocks
            // spi3->DR = 0x0000; 
            SPI_set_DR(spi3, sine_wave[sample_index]);
            channel_toggle++;
            if(channel_toggle >= 2){
                channel_toggle = 0;
                sample_index++;
                if(sample_index >= 48){
                    sample_index = 0;
                }
            }
            
        }
    }

    Systick_t* systick = Systick_init(16000000, AHB);
    Systick_start_clock(systick);
    uint32_t ms;
    uint32_t last_ticks = Systick_get_ticks();
    uint32_t curr_ticks;
    while(1){
        if(button_history == 0x00){
            ms = 1000;
        }else if(button_history == 0xFF){
            ms = 100;
        }
        curr_ticks = Systick_get_ticks();
        if((curr_ticks - last_ticks) >= ms){
            LED_toggle(green_led);
            last_ticks = curr_ticks;
        }
        // delay_ms(timer, 1000);
        __WFI();
    }
}
