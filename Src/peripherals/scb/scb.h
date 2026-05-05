#ifndef SCB_H
#define SCB_H

#include "../../def.h"
#include "../../services/print/printf.h"

#define SCB_BASE (0xE000ED00)
#define SCB_AIRCR_KEY ((uint32_t) 0x5FA)

#define USGFAULTENA_MSK (1UL << 18)
#define BUSFAULTENA_MSK (1UL << 17)
#define MEMFAULTENA_MSK (1UL << 16)

#define STKALIGN_MSK (1UL<<9)
#define DIV_0_TRP_MSK (1UL<<4)

// BUS FAULT ERR MSKS
#define CFSR_BFARVALID_MSK (1UL<<15)
#define CFSR_LSPERR_MSK (1UL<<13)
#define CFSR_STKERR_MSK (1UL<<12)
#define CFSR_UNSTKERR_MSK (1UL<<11)
#define CFSR_IMPRECISEERR_MSK (1UL<<10)
#define CFSR_PRECISEERR_MSK (1UL<<9)
#define CFSR_IBUSERR_MSK (1UL<<8)

// MEMORY MANAGEMENT ERR MSKS
#define CFSR_IACCVIOL_MSK (1UL<<0)
#define CFSR_DACCVIOL_MSK (1UL<<1)
#define CFSR_MUNSTKERR_MSK (1UL<<3)
#define CFSR_MSTKERR_MSK (1UL<<4)
#define CFSR_MLSPERR_MSK (1UL<<5)
#define CFSR_MMARVALID_MSK (1UL<<7)

// USAGE FAULTS ERR MSKS
#define CFSR_UNDEFINSTR_MSK (1UL<<16)
#define CFSR_INVSTATE_MSK (1UL<<17)
#define CFSR_INVPC_MSK (1UL<<18)
#define CFSR_NOCP_MSK (1UL<<19)
#define CFSR_UNALIGNED_MSK (1UL<<24)
#define CFSR_DIVBYZERO_MSK (1UL<<25)

#define ONES_16 ((1UL<<16)-1)
#define AIRCR_PRIORITY_GROUPING_MSK ((ONES_16 << 16)|(7UL<<8))



typedef struct SCB{
    __IO uint32_t SCB_CPUID;
    __IO uint32_t SCB_ICSR;
    __IO uint32_t SCB_VTOR;
    __IO uint32_t SCB_AIRCR;
    __IO uint32_t SCB_SCR;
    __IO uint32_t SCB_CCR;
    __IO uint32_t SCB_SHPR[3];
    __IO uint32_t SCB_SHCSR;
    __IO uint32_t SCB_CFSR;
    __IO uint32_t SCB_HFSR;
    __IO uint32_t SCB_MMAR;
    __IO uint32_t SCB_BFAR;
    __IO uint32_t SCB_AFSR;
} SCB_t;

#define SCB_ENGINE ((SCB_t*) SCB_BASE)

typedef enum PriorityGroup{
    PRIGROUP_4PRE_0SUB = 0x03,
    PRIGROUP_3PRE_1SUB = 0x04,
    PRIGROUP_2PRE_2SUB = 0X05,
    PRIGROUP_1PRE_3SUB = 0x06,
    PRIGROUP_0PRE_4SUB = 0x07 
}PriorityGroup_t;


#define SHCSR_SYSTICK_IS_ACTIVE_MSK(self) (((self)->SCB_SHCSR >> 11UL) & 1UL)
#define SHCSR_USGFAULT_IS_ACTIVE_MSK(self) (((self)->SCB_SHCSR >> 3UL) & 1UL)
#define SHCSR_BUSFAULT_IS_ACTIVE_MSK(self) (((self)->SCB_SHCSR >> 1UL) & 1UL)
#define SHCSR_MEMFAULT_IS_ACTIVE_MSK(self) (((self)->SCB_SHCSR >> 0UL) & 1UL)

#define ICSR_SYSTICK_IS_PENDING_MSK(self) (((self)->SCB_ICSR >> 26UL) & 1UL)
#define ICSR_SYSTICK_SET_PENDING_MSK ((1UL<<26UL))
#define ICSR_SYSTICK_CLR_PENDING_MSK ((1UL<<25UL))

#define SHCSR_MEMFAULT_PENDED_BIT ((13UL))
#define SHCSR_USGFAULT_PENDED_BIT ((12UL))
#define SHCSR_BUSFAULT_PENDED_BIT ((14UL))

#define SHCSR_USGFAULT_IS_PENDING_MSK(self) (((self)->SCB_SHCSR >> SHCSR_USGFAULT_PENDED_BIT ) & 1UL)
#define SHCSR_USGFAULT_SET_PENDING_MSK ((1UL<<SHCSR_USGFAULT_PENDED_BIT ))
#define SHCSR_USGFAULT_CLEAR_PENDING_MSK (~(1UL<<SHCSR_USGFAULT_PENDED_BIT ))
#define SHCSR_BUSFAULT_IS_PENDING_MSK(self) (((self)->SCB_SHCSR >> SHCSR_BUSFAULT_PENDED_BIT ) & 1UL)
#define SHCSR_BUSFAULT_SET_PENDING_MSK ((1UL<<SHCSR_BUSFAULT_PENDED_BIT ))
#define SHCSR_BUSFAULT_CLEAR_PENDING_MSK (~(1UL<<SHCSR_BUSFAULT_PENDED_BIT ))
#define SHCSR_MEMFAULT_IS_PENDING_MSK(self) (((self)->SCB_SHCSR >>SHCSR_MEMFAULT_PENDED_BIT  ) & 1UL)
#define SHCSR_MEMFAULT_SET_PENDING_MSK ((1UL<<SHCSR_MEMFAULT_PENDED_BIT ))
#define SHCSR_MEMFAULT_CLEAR_PENDING_MSK (~(1UL<<SHCSR_MEMFAULT_PENDED_BIT))

/**
    enables bus, mem and usage faults
*/
void SCB_init(SCB_t * const self);

void HardFault_Handler(void);

void BusFault_Handler(void);
void MemManage_Handler(void);
void UsageFault_Handler(void);
void SCB_write_priority_grouping(SCB_t * const self, PriorityGroup_t pg);


void SCB_enable_IRQ(SCB_t * const self, IRQn_t IRQn);
void SCB_disable_IRQ(SCB_t * const self, IRQn_t IRQn);
void SCB_set_pending_IRQ(SCB_t * const self, IRQn_t IRQn);
void SCB_clear_pending_IRQ(SCB_t * const self, IRQn_t IRQn);
uint32_t  SCB_get_pending_IRQ(SCB_t * const self, IRQn_t IRQn);
uint32_t SCB_get_active(SCB_t * const self, IRQn_t IRQn);
void SCB_set_priority(SCB_t * const self, IRQn_t IRQn, uint32_t priority);
uint32_t SCB_get_priority(SCB_t * const self, IRQn_t IRQn);

#endif