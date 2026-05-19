#include "exti.h"
#include "../../assert.h"
#include "../../services/print/printf.h"
#include "../../services/interrupts/interrupt.h"
#include "Src/def.h"
#include "Src/services/interrupts/interrupt.h"

struct EXTI{
    __IO uint32_t IMR;
    __IO uint32_t EMR;
    __IO uint32_t RTSR;
    __IO uint32_t FTSR;
    __IO uint32_t SWIER;
    __IO uint32_t PR;
};

#define EXTI_BASE (0x40013C00)
#define EXTI_ENGINE ((EXTI_t *)(EXTI_BASE))

EXTI_t* EXTI_init(void){
    return EXTI_ENGINE;
}

EXTI_error_t EXTI_config_line(EXTI_t* self, SYSCFG_t* syscfg, EXTI_line_t line, EXTI_config_t* config){
    BARE_ASSERT(self!= NULL);
    BARE_ASSERT(syscfg != NULL);
    BARE_ASSERT(config!= NULL);

    if(line >= 16){
        printf_("Haven't implemented exti line >= 16 %d\n", line);
        return EXTI_CONFIG_FAIL;
    }

    switch(config->mode){
        case EXTI_MODE_INTERRUPT:
            EXTI_set_IMR(self, line, EXTI_SET);
            EXTI_set_EMR(self, line, EXTI_CLEAR);
            break;
        case EXTI_MODE_EVENT:
            EXTI_set_IMR(self, line, EXTI_CLEAR);
            EXTI_set_EMR(self, line, EXTI_SET);
            break;
        case EXTI_MODE_BOTH:
            EXTI_set_IMR(self, line, EXTI_SET);
            EXTI_set_EMR(self, line, EXTI_SET);
            break;
        case EXTI_MODE_NONE:
            EXTI_set_IMR(self, line, EXTI_CLEAR);
            EXTI_set_EMR(self, line, EXTI_CLEAR);
            break;
        default:
            printf_("someone didn't use the exti_mode_t enum :(, val: %d\n", config->mode); 
            return EXTI_BAD_CONFIG_MODE;
    }

    switch(config->trigger){
        case EXTI_NONE_TRIGGER:
            EXTI_set_RTSR(self, line, EXTI_CLEAR);
            EXTI_set_FTSR(self, line, EXTI_CLEAR);
            break;
        case EXTI_RISING_TRIGGER:
            EXTI_set_RTSR(self, line, EXTI_SET);
            EXTI_set_FTSR(self, line, EXTI_CLEAR);
            break;
        case EXTI_FALLING_TRIGGER:
            EXTI_set_RTSR(self, line, EXTI_CLEAR);
            EXTI_set_FTSR(self, line, EXTI_SET);
            break;
        case EXTI_BOTH_TRIGGER:
            EXTI_set_RTSR(self, line, EXTI_SET);
            EXTI_set_FTSR(self, line, EXTI_SET);
            break;
        default:
            printf_("someone didn't use exti_trigger_t enum :(, val: %d\n", config->trigger);
            return EXTI_BAD_CONFIG_TRIGGER;
    }

    SYSCFG_enable_EXTI(syscfg, config->port, line);

    if(config->mode != EXTI_MODE_BOTH && config->mode != EXTI_MODE_INTERRUPT){
        return EXTI_SUCCESS;
    }
    // enable interrupt
    switch(line){
        case EXTI_LINE_0:
            enable_IRQ(EXTI0_IRQn);
            break;
        case EXTI_LINE_1:
            enable_IRQ(EXTI1_IRQn);
            break;
        case EXTI_LINE_2:
            enable_IRQ(EXTI2_IRQn);
            break;
        case EXTI_LINE_3:
            enable_IRQ(EXTI3_IRQn);
            break;
        case EXTI_LINE_4:
            enable_IRQ(EXTI4_IRQn);
            break;
        case EXTI_LINE_5:
        case EXTI_LINE_6:
        case EXTI_LINE_7:
        case EXTI_LINE_8:
        case EXTI_LINE_9:
            enable_IRQ(EXTI9_5_IRQn);
            break;
        case EXTI_LINE_10: 
        case EXTI_LINE_11: 
        case EXTI_LINE_12: 
        case EXTI_LINE_13: 
        case EXTI_LINE_14: 
        case EXTI_LINE_15: 
            enable_IRQ(EXTI15_10_IRQn);
            break;
        default:
            return EXTI_BAD_LINE;
    }
    return EXTI_SUCCESS;
}

void EXTI_set_IMR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val){
    BARE_ASSERT(self!= NULL);
    bit_band_write((uint32_t)&self->IMR, line, val);
}

void EXTI_set_EMR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val){
    BARE_ASSERT(self!= NULL);
    bit_band_write((uint32_t)&self->EMR, line, val);
}

void EXTI_set_RTSR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val){
    BARE_ASSERT(self!= NULL);
    bit_band_write((uint32_t)&self->RTSR, line, val);
}

void EXTI_set_FTSR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val){
    BARE_ASSERT(self!= NULL);
    bit_band_write((uint32_t)&self->FTSR, line, val);
}

void EXTI_set_SWIER(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val){
    BARE_ASSERT(self!= NULL);
    bit_band_write((uint32_t)&self->SWIER, line, val);
}

__STATIC_INLINE uint32_t EXTI_clear_PR_msk(const EXTI_line_t line){
    return ((uint32_t)((uint32_t)1 << (uint32_t)line));
}

void EXTI_clear_PR(EXTI_t* self, const EXTI_line_t line){
    BARE_ASSERT(self != NULL);
    self->PR = EXTI_clear_PR_msk(line);
}