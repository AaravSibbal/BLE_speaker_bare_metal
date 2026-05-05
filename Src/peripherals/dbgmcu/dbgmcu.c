#include "dbgmcu.h"

static void DBGMCU_en_trace(void){
    DBGMCU_CR |= DBGMCU_TRACE_EN_MSK;
}

static void DBGMCU_set_trace_mode(trace_mode_t mode){
    uint32_t mode_32 = (uint32_t)mode;
    DBGMCU_CR &= ~(DBGMCU_TRACE_MODE_MSK<<6);
    DBGMCU_CR |= (mode_32 << 6);
}


void DBGMCU_debug_enable(trace_mode_t mode){
    DBGMCU_en_trace();
    DBGMCU_set_trace_mode(mode);
}