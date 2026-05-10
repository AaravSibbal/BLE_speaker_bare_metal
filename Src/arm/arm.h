#ifndef ARM_H
#define ARM_H

// https://github.com/ARM-software/CMSIS_4/blob/master/CMSIS/Include/cmsis_gcc.h

__attribute__( ( always_inline ) ) static inline void __enable_irq(void)
{
  __asm volatile ("cpsie i" : : : "memory");
}

__attribute__( ( always_inline ) ) static inline void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}

__attribute__( ( always_inline ) ) static inline uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

__attribute__( ( always_inline ) ) static inline void __set_PRIMASK(uint32_t priMask)
{
  __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

__attribute__((always_inline)) static inline void __WFI(void)
{
  __asm volatile ("wfi");
}

__attribute__((always_inline)) static inline void __WFE(void)
{
  __asm volatile ("wfe");
}

__attribute__((always_inline)) static inline void __ISB(void)
{
  __asm volatile ("isb 0xF":::"memory");
}

__attribute__((always_inline)) static inline void __DSB(void)
{
  __asm volatile ("dsb 0xF":::"memory");
}

__attribute__((always_inline)) static inline void __DMB(void)
{
  __asm volatile ("dmb 0xF":::"memory");
}

#endif