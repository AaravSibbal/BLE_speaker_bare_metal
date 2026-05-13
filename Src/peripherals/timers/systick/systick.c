#include "systick.h"
#include "Src/arm/arm.h"
#include "Src/def.h"
#include "Src/services/interrupts/interrupt.h"
#include "assert.h"

typedef struct Systick_driver{
    __IO uint32_t ctrl;
    __IO uint32_t load;
    __IO uint32_t val;
    __IO uint32_t calib;
} Systick_driver_t;

static volatile uint32_t ticks;
static Systick_t systick_obj;


struct Systick{
    Timer_t timer;
    Systick_driver_t* driver;
    uint32_t ticks;
};

#define SYSTICK_BASE_ADDRESS ((uint32_t) 0xE000E010)
#define SYSTICK_ENGINE ((Systick_driver_t*) SYSTICK_BASE_ADDRESS)
#define CTRL_INTER_EN_MSK (1UL<<1UL)
#define MAX_RELOAD_VAL (0x00FFFFFFUL)
#define RESET_VAL (0x01UL)
#define CLKSOURCE_BIT (2UL)
#define CLKSOURCE_SET_MSK (1UL<<CLKSOURCE_BIT)
#define CTRL_COUNTER_EN_BIT (0)
#define CTRL_COUNTER_EN_MSK (1UL<<CTRL_COUNTER_EN_BIT)


uint32_t Systick_get_ticks(){
    return ticks;
}

__STATIC_INLINE void Systick_disable_clock(Systick_t* self){
    assert(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    self->driver->ctrl &= ~((uint32_t)1<<0);
    __set_PRIMASK(primask);
}

void Systick_stop(void *self){
    assert(self != NULL);
    Systick_t* systick_device = (Systick_t*)self;
    Systick_disable_clock(systick_device);
}

void Systick_reset(void* self){
    assert(self != NULL);
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    Systick_t *systick_device = (Systick_t *)self;
    systick_device->ticks = 0;
    __set_PRIMASK(primask);
}

__INLINE void Systick_enable_interrupt(){
    SYSTICK_ENGINE->ctrl |= CTRL_INTER_EN_MSK;
}

__INLINE void Systick_disable_interrupt(){
    SYSTICK_ENGINE->ctrl &= ~(CTRL_INTER_EN_MSK); 
}

static inline void Systick_set_reload_val(Systick_t* self, uint32_t clock_speed_HZ){
    assert(self != NULL);
    // example I get 16,000,000
    uint32_t reload_value = (clock_speed_HZ/1000) - 1;
    reload_value &= MAX_RELOAD_VAL;
    self->driver->load = reload_value;
}

static inline void Systick_reset_counter(void* self){
    assert(self != NULL);
    Systick_t *systick_device = (Systick_t *)self;
    systick_device->driver->val = RESET_VAL;
}

static inline void Systick_start_clock(void* self){
    assert(self != NULL);
    Systick_t* systick_device = (Systick_t *)self;
    systick_device->driver->ctrl &= ~(CTRL_COUNTER_EN_MSK);
    systick_device->driver->ctrl |= CTRL_COUNTER_EN_MSK;
}

static inline void Systick_select_clock_src(Systick_t* self, Clck_src_t clck_src){
    assert(self != NULL);
    self->driver->ctrl &= ~(CLKSOURCE_SET_MSK);
    if(clck_src == AHB){
        self->driver->ctrl |= (CLKSOURCE_SET_MSK);   
    }
}

static inline void Systick_setup(Systick_t* self, uint32_t clck_speed_hz, Clck_src_t clck_src){
    assert(self != NULL);
    Systick_set_reload_val(self, clck_speed_hz);
    Systick_reset_counter(self);
    Systick_select_clock_src(self, clck_src);
    Systick_enable_interrupt(self);
}

void SysTick_Handler(void){
    clear_pending_IRQ(SysTick_IRQn);
    ticks++;
}



Systick_t* Systick_init(uint32_t clck_speed_hz, Clck_src_t clck_src){
    Systick_t* self = &systick_obj;
    Systick_setup(self, clck_speed_hz, clck_src);
    self->timer.get_ticks = Systick_get_ticks;
    self->timer.start = Systick_start_clock;
    self->timer.reset = Systick_reset_counter;
    self->timer.stop = Systick_stop;
    self->driver = ((Systick_driver_t *)(SYSTICK_BASE_ADDRESS));
    self->ticks = ticks;
    return self;
}