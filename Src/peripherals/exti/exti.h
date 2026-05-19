#ifndef EXTI_H
#define EXTI_H

#include "../../def.h"
#include "../syscfg/syscfg.h"

typedef struct EXTI EXTI_t;

typedef enum EXTI_line{
    EXTI_LINE_0 = 0,
    EXTI_LINE_1,
    EXTI_LINE_2,
    EXTI_LINE_3,
    EXTI_LINE_4,
    EXTI_LINE_5,
    EXTI_LINE_6,
    EXTI_LINE_7,
    EXTI_LINE_8,
    EXTI_LINE_9,
    EXTI_LINE_10,
    EXTI_LINE_11,
    EXTI_LINE_12,
    EXTI_LINE_13,
    EXTI_LINE_14,
    EXTI_LINE_15,
    EXTI_LINE_PVD,
    EXTI_LINE_RTC_ALARM,
    EXTI_LINE_USB_FS_WAKEUP,
    EXTI_LINE_ETH_WAKEUP,
    EXTI_LINE_USB_HS_WAKEUP,
    EXTI_LINE_RTC_TAMPER,
    EXTI_LINE_RTC_WAKEUP,
} EXTI_line_t;

typedef enum EXTI_mode{
    EXTI_MODE_NONE = 0,
    EXTI_MODE_INTERRUPT = 1,
    EXTI_MODE_EVENT = 2,
    EXTI_MODE_BOTH = 3
}EXTI_mode_t;

typedef enum EXTI_trigger{
    EXTI_RISING_TRIGGER = 0,
    EXTI_FALLING_TRIGGER = 1,
    EXTI_BOTH_TRIGGER = 2,
    EXTI_NONE_TRIGGER = 3
}EXTI_trigger_t;

typedef struct EXTI_config{
    EXTI_mode_t mode;
    EXTI_trigger_t trigger;
    GPIO_port_t port;
}EXTI_config_t;

typedef enum EXTI_state{
    EXTI_CLEAR = 0,
    EXTI_SET = 1
}EXTI_state_t;

typedef enum EXTI_error_t{
    EXTI_SUCCESS = 0,
    EXTI_CONFIG_FAIL = 1,
    EXTI_BAD_CONFIG_MODE = 2, 
    EXTI_BAD_CONFIG_TRIGGER = 3,
    EXTI_BAD_LINE = 4
}EXTI_error_t;

EXTI_t* EXTI_init(void);
EXTI_error_t EXTI_config_line(EXTI_t* self, SYSCFG_t* syscfg, EXTI_line_t line, EXTI_config_t* config);
void EXTI_set_IMR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val);
void EXTI_set_EMR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val);
void EXTI_set_RTSR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val);
void EXTI_set_FTSR(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val);
void EXTI_set_SWIER(EXTI_t* self, const EXTI_line_t line, const EXTI_state_t val);
void EXTI_clear_PR(EXTI_t* self, const EXTI_line_t line);


#endif