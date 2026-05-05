#ifndef TPIU_H
#define TPIU_H

#include "../../def.h"

#define TPIU_SELECTED_PIN_PROTO (*(__IO uint32_t *) 0xE00400F0)
#define TPIU_SELECTED_PIN_PROTO_MSK ((1UL<<2)-1)

typedef enum pin_protocol {
    TPIU_SYNC_TRACE_PORT = 0,
    TPIU_SWO_MANCHESTER,
    TPIU_SWO_NRZ
}pin_protocol_t;

void TPIU_set_proto(pin_protocol_t proto);

#endif