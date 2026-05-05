#include "tpiu.h"

void TPIU_set_proto(pin_protocol_t proto){
    uint32_t proto_32 = (uint32_t)proto;
    TPIU_SELECTED_PIN_PROTO &= ~(TPIU_SELECTED_PIN_PROTO_MSK<<0);
    TPIU_SELECTED_PIN_PROTO |= (proto_32 << 0);
}
