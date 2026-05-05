#ifndef DEMCR_H
#define DEMCR_H

#include "../../def.h"

#define DEMCR_ENGINE (*(__IO uint32_t *) (0xE000EDFCU))
#define DEMCR_TRACE_EN_MSK (1UL<<24)


void DEMCR_enable_trace(void);


#endif