#include "rcc.h"

typedef struct RCC{
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t RESERVED_0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t RESERVED_1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t RESERVED_2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
    uint32_t RESERVED_3[2];
    __IO uint32_t AHB1LPENR;
    __IO uint32_t AHB2LPENR;
    __IO uint32_t AHB3LPENR;
    uint32_t RESERVED_4;
    __IO uint32_t APB1LPENR;
    __IO uint32_t APB2LPENR;
    uint32_t RESERVED_5[2];
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    uint32_t RESERVED_6[2];
    __IO uint32_t SSCGR;
	__IO uint32_t PLLI2SCFGR;
} RCC_t;

#define RCC_BASE (0x40023800UL)

RCC_t* init_RCC(void){
    return ((RCC_t*) RCC_BASE);
}

#define ENABLE 1UL
#define DISABLE 0UL

void RCC_en_GPIO(RCC_t* self, GPIO_port_t port){
    uint32_t port_uint = ((uint32_t)port);
    uint32_t addr = (uint32_t)(&self->AHB1ENR);
    bit_band_write(addr, port_uint, ENABLE);
}

void RCC_dis_GPIO(RCC_t* self, GPIO_port_t port){
    uint32_t port_uint = ((uint32_t)port);
    uint32_t addr = (uint32_t)(&self->AHB1ENR);
    bit_band_write(addr, port_uint, DISABLE);
}

#define SYSCFG_BIT 14UL

void RCC_en_SYSCFG(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB2ENR);
    bit_band_write(addr, SYSCFG_BIT, ENABLE);
}

void RCC_dis_SYSCFG(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB2ENR);
    bit_band_write(addr, SYSCFG_BIT, DISABLE);
}