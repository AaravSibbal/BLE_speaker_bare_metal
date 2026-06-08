
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
#include "peripherals/timers/systick/systick.h"
#include "services/interrupts/interrupt.h"
#include "arm/arm.h"
#include "peripherals/i2c/i2c.h"
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

    enable_IRQ(SysTick_IRQn);
    enable_IRQ(I2C1_ER_IRQn);
    enable_IRQ(I2C1_EV_IRQn);

    __DSB();
    __ISB();

    RCC_t* rcc = init_RCC();
    RCC_en_GPIO(rcc, BUTTON_GPIO_PORT);
    RCC_en_GPIO(rcc, GPIO_PORT_D);

    GPIO_t* green_led_gpio = GPIO_init(GPIO_PORT_D, rcc);
    LED_t* green_led = LED_init(LED_GREEN, green_led_gpio);

    // --- NEW DAC WAKEUP CODE ---
    // Configure PD4 as General Purpose Output to control DAC Reset
    GPIO_set_moder(green_led_gpio, GPIO_PIN_4, GPIO_MODE_OUTPUT);
    GPIO_set_otyper(green_led_gpio, GPIO_PIN_4, GPIO_TYPE_PUSH_PULL);
    
    // Drive PD4 HIGH to bring the CS43L22 DAC out of reset
    GPIO_set_odr(green_led_gpio, GPIO_PIN_4, 1);

    // Give the DAC a few milliseconds to internally boot before talking to it
    for(volatile int i = 0; i < 50000; i++);
    // GPIO_t* button_gpio = GPIO_init_empty(GPIO_PORT_A, GPIO_PIN_0);

    I2C_t* i2c1_device = I2C_init(
        I2C_1, 
        I2C_MODE_STANDARD, 
        i2c1_sda_gpio_port, 
        i2c1_sda_gpio_pin,
        i2c1_scl_gpio_port,
        i2c1_scl_gpio_pin,
        16, 
        rcc
    );
    
    if(i2c1_device == NULL){
        __BKPT(0);
    }



    queue_t i2c1_rx_queue;
    queue_t i2c1_tx_queue;
    uint8_t rx_buffer[1];
    uint8_t tx_buffer[1];
    queue_t* i2c1_rx_queue_ptr = queue_init(
        &i2c1_rx_queue, 
        rx_buffer, 
        QUEUE_CAP_1
    );
    queue_t* i2c1_tx_queue_ptr = queue_init(
        &i2c1_tx_queue, 
        tx_buffer, 
        QUEUE_CAP_1
    );

    const uint8_t DAC_write_addr = 0x94;

    I2C_handle_t* i2c1_handle = I2C_handle_init(
        i2c1_device,
        I2C_1,
        i2c1_rx_queue_ptr, 
        i2c1_tx_queue_ptr, 
        DAC_write_addr,
        NULL
    );

    const uint8_t dac_chip_id = 0x01;

    __bool result = queue_enqueue(i2c1_handle->tx_queue, dac_chip_id);
    if(result == FALSE){
        printf_("queue failed here");
        __BKPT(0);
    }

    I2C_write(i2c1_handle);
    while(i2c1_handle->state != I2C_STATE_DONE){
        // __WFI();
    }
    while(I2C_get_SR2(i2c1_device->driver) & 0x02){}
    I2C_read(i2c1_handle, 1);
    while(i2c1_handle->state != I2C_STATE_DONE){
        // __WFI();
    }
    uint8_t val;
    queue_dequeue(i2c1_handle->rx_queue, &val);
    printf_("val: %X", val);
    
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
