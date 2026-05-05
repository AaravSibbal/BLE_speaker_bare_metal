#include "demcr.h"

void DEMCR_enable_trace(void){
    DEMCR_ENGINE |= (DEMCR_TRACE_EN_MSK);
}