#include "rcc.h"
#include "Src/def.h"

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
    (void)self->AHB1ENR;
}

void RCC_dis_GPIO(RCC_t* self, GPIO_port_t port){
    uint32_t port_uint = ((uint32_t)port);
    uint32_t addr = (uint32_t)(&self->AHB1ENR);
    bit_band_write(addr, port_uint, DISABLE);
    (void)self->AHB1ENR;
}

#define SYSCFG_BIT 14UL

void RCC_en_SYSCFG(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB2ENR);
    bit_band_write(addr, SYSCFG_BIT, ENABLE);
    (void)self->APB2ENR;
}

void RCC_dis_SYSCFG(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB2ENR);
    bit_band_write(addr, SYSCFG_BIT, DISABLE);
    (void)self->APB2ENR;
}

#define I2C1_BIT 21

void RCC_en_I2C1(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB1ENR);
    bit_band_write(addr, I2C1_BIT, ENABLE);
    (void)self->APB1ENR;
}

#define I2C2_BIT 22

void RCC_en_I2C2(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB1ENR);
    bit_band_write(addr, I2C2_BIT, ENABLE);
    (void)self->APB1ENR;
}

#define I2C3_BIT 23

void RCC_en_I2C3(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB1ENR);
    bit_band_write(addr, I2C3_BIT, ENABLE);
    (void)self->APB1ENR;
}

#define I2C1_APB1RSTR_BIT 21UL

void RCC_reset_I2C1(RCC_t* self){
    uint32_t addr = (uint32_t)(&self->APB1RSTR);
    bit_band_write(addr, I2C1_APB1RSTR_BIT, 1);
    (void)self->APB1RSTR;
    bit_band_write(addr, I2C1_APB1RSTR_BIT, 0);
    (void)self->APB1RSTR;
}