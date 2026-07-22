
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
#include "peripherals/itm/itm.h"
#include "peripherals/rcc/rcc.h"
#include "peripherals/spi/spi_driver.h"
#include "peripherals/timers/systick/systick.h"
#include "services/interrupts/interrupt.h"
#include "arm/arm.h"
#include "peripherals/i2c/i2c.h"
#include "devices/dac/dac.h"
#include "services/audio_engine/audio_engine.h"
#include <stdint.h>


int main(void){
    // setting up priority
    set_priority_grouping(PRIGROUP_4PRE_0SUB);
    set_priority(DMA1_Stream5_IRQn, 1);
    // set_priority(DMA1_Stream5_IRQn, 1);
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
    enable_IRQ(DMA1_Stream5_IRQn);

    __DSB();
    __ISB();

    RCC_t* rcc = init_RCC();
    RCC_en_GPIO(rcc, BUTTON_GPIO_PORT);
    RCC_en_GPIO(rcc, GPIO_PORT_D);

    ITM_init(rcc);

    GPIO_t* green_led_gpio = GPIO_init(GPIO_PORT_D, rcc);
    LED_t* green_led = LED_init(LED_GREEN, green_led_gpio);

    GPIO_set_moder(green_led_gpio, GPIO_PIN_4, GPIO_MODE_OUTPUT);
    GPIO_set_otyper(green_led_gpio, GPIO_PIN_4, GPIO_TYPE_PUSH_PULL);
    Systick_t* systick = Systick_init(16000000, AHB);
    Systick_start_clock(systick);

    // --- CPU CALIBRATION PHASE ---
    uint32_t max_idle_count = 0;
    uint32_t calib_start = Systick_get_ticks();
    while((Systick_get_ticks() - calib_start) < 1000) {
        max_idle_count++;
    }

    audio_engine_t* audio_engine_obj = audio_engine_init(ENGINE_MODE_TESTING, rcc);


    uint32_t ms = 0;
    uint32_t last_ticks = Systick_get_ticks();
    uint32_t cpu_last_ticks = Systick_get_ticks();
    uint32_t curr_ticks = 0;
    uint32_t curr_idle_count = 0;
    printf_("something\n");
    while(1){
        curr_ticks = Systick_get_ticks();
        if(button_history == 0x00){
            ms = 1000;
        }else if(button_history == 0xFF){
            ms = 100;
        }        
        if((curr_ticks - last_ticks) >= ms){
            LED_toggle(green_led);
            last_ticks = curr_ticks;
        }
        audio_engine_processing(audio_engine_obj);

        curr_idle_count++;
        if((curr_ticks - cpu_last_ticks) >= 1000){
            // Calculate percentage using integer math to avoid pulling in FPU/floats
            uint32_t load = 100 - ((curr_idle_count * 100) / max_idle_count);
            
            // Cast to uint32_t/unsigned long if printf_ expects it for %u or %lu
            printf_("CPU Load: %u%%\n", load);
            
            curr_idle_count = 0;
            cpu_last_ticks = curr_ticks;
        }
        // delay_ms(timer, 1000);
        // __WFI();
    }
}
