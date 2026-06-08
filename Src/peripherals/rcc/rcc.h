#ifndef RCC_H
#define RCC_H

#include "../../def.h"

typedef struct RCC RCC_t;

RCC_t* init_RCC(void);
void RCC_en_GPIO(RCC_t* self, GPIO_port_t port);
void RCC_dis_GPIO(RCC_t* self, GPIO_port_t port);
void RCC_en_SYSCFG(RCC_t* self); 
void RCC_dis_SYSCFG(RCC_t* self); 
void RCC_en_I2C1(RCC_t* self);
void RCC_en_I2C2(RCC_t* self);
void RCC_en_I2C3(RCC_t* self);
void RCC_reset_I2C1(RCC_t* self);


// #define CCR_OFFSET (0x00)
// #define AHB1_ENR_OFFSET (0x30)
// #define APB2_ENR_OFFSET (0x44)

// #define RCC_CCR (RCC_BASE + CCR_OFFSET)
// #define RCC_AHB1_ENR (*(__IO uint32_t *)((RCC_BASE) + (AHB1_ENR_OFFSET))) 
// #define RCC_APB2_ENR (*(__IO uint32_t *)((RCC_BASE) + (APB2_ENR_OFFSET)))

// #define GPIOA_BIT 0
// #define GPIOD_BIT 3
// #define GPIOB_BIT 1
// #define SYSCFG_BIT 14
// #define RCC_AHB1_GPIOD_EN (1UL<<GPIOD_BIT)
// #define RCC_AHB1_GPIOA_EN (1UL<<GPIOA_BIT)
// #define RCC_AHB1_GPIOB_EN (1UL<<GPIOB_BIT)
// #define RCC_APB2_SYSCFG_EN (1UL<<SYSCFG_BIT)

#endif