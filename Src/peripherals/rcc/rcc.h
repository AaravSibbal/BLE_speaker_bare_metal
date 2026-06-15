#ifndef RCC_H
#define RCC_H

#include "../../def.h"

typedef struct RCC RCC_t;

typedef enum PLLI2SR{
    RCC_PLLI2SR_2 = 0x2,
    RCC_PLLI2SR_3 = 0x3,
    RCC_PLLI2SR_4 = 0x4,
    RCC_PLLI2SR_5 = 0x5,
    RCC_PLLI2SR_6 = 0x6,
    RCC_PLLI2SR_7 = 0x7
} PLLI2SR_t;

RCC_t* init_RCC(void);
void RCC_en_GPIO(RCC_t* self, GPIO_port_t port);
void RCC_dis_GPIO(RCC_t* self, GPIO_port_t port);
void RCC_en_SYSCFG(RCC_t* self); 
void RCC_dis_SYSCFG(RCC_t* self); 
void RCC_en_I2C1(RCC_t* self);
void RCC_en_I2C2(RCC_t* self);
void RCC_en_I2C3(RCC_t* self);
void RCC_reset_I2C1(RCC_t* self);
void RCC_en_PLLI2S(RCC_t* self);
uint32_t RCC_get_PLLI2S_ready(RCC_t* self);
void RCC_set_PLLI2SR(RCC_t* self, PLLI2SR_t val);
/**
    val needs to be:
    50<= val <= 432
*/
__bool RCC_set_PLLI2SN(RCC_t* self, uint32_t val);
void RCC_en_SPI2(RCC_t* self);
void RCC_en_SPI3(RCC_t* self);
#endif