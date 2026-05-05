#ifndef DBGMCU_H
#define DBGMCU_H

#include "../../def.h"

#define DBGMCU_CR (*(__IO uint32_t *) 0xE0042004)
#define DBGMCU_TRACE_EN_MSK (1UL<<5)
#define DBGMCU_TRACE_MODE_MSK ((1<<2)-1)


typedef enum Trace_Mode{
    DBGMCU_ASYNC = 0,
    DBGMCU_SYNC_1,
    DBGMCU_SYNC_2,
    DBGMCU_SYNC_4,
} trace_mode_t;

void DBGMCU_debug_enable(trace_mode_t mode);

#endif