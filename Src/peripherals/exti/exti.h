#ifndef EXTI_H
#define EXTI_H

#include "../../def.h"

#define EXTI_BASE (0x40013C00)

typedef struct EXTI{
    __IO uint32_t EXTI_IMR;
    __IO uint32_t EXTI_EMR;
    __IO uint32_t EXTI_RTSR;
    __IO uint32_t EXTI_FTSR;
    __IO uint32_t EXTI_SWIER;
    __IO uint32_t EXTI_PR;
} EXTI_t;

#define EXTI_ENGINE ((EXTI_t *)(EXTI_BASE))

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
} EXTI_Line_t;

void EXTI_set_IMR(EXTI_Line_t line);
void EXTI_set_RTSR(EXTI_Line_t line);
void EXTI_clear_PR(EXTI_Line_t line);


#endif