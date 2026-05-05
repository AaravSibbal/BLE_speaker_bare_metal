#include "scb.h"

static inline void SCB_enable_errors(SCB_t * const self){
    self->SCB_SHCSR |= (BUSFAULTENA_MSK) | (USGFAULTENA_MSK) | (MEMFAULTENA_MSK);
}

static inline void SCB_en_stack_align(SCB_t * const self){
    self->SCB_CCR |= (STKALIGN_MSK);
}

static inline void SCB_en_div0_trap(SCB_t * const self){
    self->SCB_CCR |= (DIV_0_TRP_MSK);
}

void SCB_init(SCB_t * const self){
    SCB_enable_errors(self);
    SCB_en_stack_align(self);
    SCB_en_div0_trap(self);
}

static inline void SCB_clr_cfsr(SCB_t * const self, uint32_t old_cfsr){
    self->SCB_CFSR = old_cfsr;
}


static void print_stack(const uint32_t * const fault_stack){
    uint32_t r0 = fault_stack[0];
    uint32_t r1 = fault_stack[1];
    uint32_t r2 = fault_stack[2];
    uint32_t r3 = fault_stack[3];
    uint32_t r12 = fault_stack[4];
    uint32_t lr = fault_stack[5];
    uint32_t pc = fault_stack[6];
    uint32_t psr = fault_stack[7];

    /**
    r0-r3: are the first four function arguments that ran
    it can be felpful to catch a null pointer, will show up
    as 0x00000000 
    r12: not very useful
    psr: bits 0-8 give the irq number for the interrupt we are in
    0 means main loop.
    pc: this is the program counter, and shows the exact point the
    program is in when the fault happens
    lr: this is the link register and given the return address
    for the current function that we are on. 
    */
    printf("r0: 0x%08lX", r0);
    printf("r1: 0x%08lX", r1);
    printf("r2: 0x%08lX", r2);
    printf("r3: 0x%08lX", r3);
    printf("r12: 0x%08lX", r12);
    printf("lr: 0x%08lX", lr);
    printf("pc: 0x%08lX", pc);
    printf("psr: 0x%08lX", psr);
}

__attribute__((naked)) void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4 \n"         // Check which stack pointer was in use (MSP or PSP)
        "ite eq \n"             // If/Then/Else block
        "mrseq r0, msp \n"      // If MSP was used, move MSP into R0
        "mrsne r0, psp \n"      // If PSP was used, move PSP into R0
        "b HardFault_Decoder \n" // Branch to your C function, passing R0 as the first argument
    );
}


__attribute__((used)) static void HardFault_Decoder(const uint32_t * const fault_stack){
    printf("[HARD FAULT HANDLER]\n");
    print_stack(fault_stack);
    printf ("BFAR = 0x%08lX\n", SCB_ENGINE->SCB_BFAR);
    printf ("CFSR = 0x%08lX\n", SCB_ENGINE->SCB_CFSR);
    printf ("HFSR = 0x%08lX\n", SCB_ENGINE->SCB_HFSR);
    while(1){
        // loop forever
        __asm volatile ("nop");
    }
}

__attribute__((naked)) void BusFault_Handler(void){
    __asm volatile (
        "tst lr, #4 \n"         // Check which stack pointer was in use (MSP or PSP)
        "ite eq \n"             // If/Then/Else block
        "mrseq r0, msp \n"      // If MSP was used, move MSP into R0
        "mrsne r0, psp \n"      // If PSP was used, move PSP into R0
        "b BusFault_Decoder \n" // Branch to your C function, passing R0 as the first argument       
    );
}

__attribute__((used)) static void BusFault_Decoder(const uint32_t * const fault_stack){
    uint32_t cfsr = SCB_ENGINE->SCB_CFSR;
    uint32_t bfar = SCB_ENGINE->SCB_BFAR;
    printf("[BUS FAULT HANDLER]\n");
    print_stack(fault_stack);

    if(cfsr & (CFSR_IBUSERR_MSK)){
        printf("Instruction bus error. The processor detects the instruction bus error on prefetching an instruction\n");
    }
    if(cfsr & (CFSR_PRECISEERR_MSK)){
        printf("Precise error: The PC value stacked for the exception return points to the instruction that caused the fault.\n");
    }
    if(cfsr & (CFSR_IMPRECISEERR_MSK)){
        printf("Imprecise error: The return address in the stack frame is not related to the instruction that caused the error.\n");
    }
    if(cfsr & (CFSR_UNSTKERR_MSK)){
        printf("Unstack error: Unstack for an exception return has caused one or more bus faults.\n");
    }
    if(cfsr & (CFSR_STKERR_MSK)){
        printf("Stack error: Stacking for an exception entry has caused one or more bus faults\n");
    }
    if(cfsr & (CFSR_LSPERR_MSK)){
        printf("Floating point lazy state prevention error.\n");
    }
    if(cfsr & (CFSR_BFARVALID_MSK)){
        printf("We have a valid address for the fault.\n");
        printf("Address: 0x%08lX\n", bfar);
    }

    // clear cfsr
    SCB_clr_cfsr(SCB_ENGINE, cfsr);

    while(1){
        __asm volatile ("nop");
    }
}

__attribute__((naked)) void MemManage_Handler(void){
    __asm volatile (
        "tst lr, #4 \n"         // Check which stack pointer was in use (MSP or PSP)
        "ite eq \n"             // If/Then/Else block
        "mrseq r0, msp \n"      // If MSP was used, move MSP into R0
        "mrsne r0, psp \n"      // If PSP was used, move PSP into R0
        "b MemManage_Decoder \n" // Branch to your C function, passing R0 as the first argument       
    );
}

__attribute__((used)) static void MemManage_Decoder(const uint32_t * const fault_stack){
    uint32_t mmfar = SCB_ENGINE->SCB_MMAR;
    uint32_t cfsr = SCB_ENGINE->SCB_CFSR;
    printf("[MEMORY MANAGEMENT FAULT DETECTED]\n");
    print_stack(fault_stack);
    if(cfsr & (CFSR_IACCVIOL_MSK)){
        printf("Instruction access violation: The processor attempted an instruction fetch from a location that does not permit execution.\n");
    }
    if(cfsr & (CFSR_DACCVIOL_MSK)){
        printf("Data access violation: The processor attempted a load or store at a location that does not permit the operation.\n");
    }
    if(cfsr & (CFSR_MUNSTKERR_MSK)){
        printf("Unstacking error on a return from exception: Unstack for an exception return has caused one or more access violations.\n");
    }
    if(cfsr & (CFSR_MSTKERR_MSK)){
        printf("Memory manager fault on stacking for exception entry: Stacking for an exception entry has caused one or more access violations.\n");
    }   
    if(cfsr & (CFSR_MLSPERR_MSK)){
        printf("Floating point lazy prevention error: A MemManage fault occurred during floating-point lazy state preservation\n");
    }
    if(cfsr & (CFSR_MMARVALID_MSK)){
        printf("We found a valid memory address for the fault.\n");
        printf("Address: 0x%08lX", mmfar);
    }

    SCB_clr_cfsr(SCB_ENGINE, cfsr);
    while(1){
        __asm volatile ("nop");
    }
}

__attribute__((naked)) void UsageFault_Handler(void){
    __asm volatile (
        "tst lr, #4 \n"         // Check which stack pointer was in use (MSP or PSP)
        "ite eq \n"             // If/Then/Else block
        "mrseq r0, msp \n"      // If MSP was used, move MSP into R0
        "mrsne r0, psp \n"      // If PSP was used, move PSP into R0
        "b UsageFault_Decoder \n" // Branch to your C function, passing R0 as the first argument       
    );
}

__attribute__((used)) static void UsageFault_Decoder(const uint32_t * const fault_stack){
    uint32_t cfsr = SCB_ENGINE->SCB_CFSR;
    printf("[USAGE FAULT DETECTED]\n");
    print_stack(fault_stack);
    if(cfsr & (CFSR_DIVBYZERO_MSK)){
        printf("Div by 0 error\n");
    }   
    if(cfsr & (CFSR_UNALIGNED_MSK)){
        printf("Unaligned access usage fault: the processor has made an unaligned memory access.\n");
    }
    if(cfsr & (CFSR_NOCP_MSK)){
        printf("The processor has attempted to access a coprocessor. The proccessor does not support a coprosossor\n");
    }
    if(cfsr & (CFSR_INVPC_MSK)){
        printf("Invalid PC load usage fault, caused by an invalid PC load by EXC_RETURN\n");
    }
    if(cfsr & (CFSR_INVSTATE_MSK)){
        printf("Invalid state usage fault: The processor has attempted to execute an instruction that makes illegal use of the EPSR.\n");
    }
    if(cfsr & (CFSR_UNDEFINSTR_MSK)){
        printf("The processor has attempted to execute an undefined instruction.\n");
    }

    SCB_clr_cfsr(SCB_ENGINE, cfsr);

    while(1){
        __asm volatile ("nop");
    }
}

void SCB_write_priority_grouping(SCB_t * const self, PriorityGroup_t pg){
    uint32_t pg_32 = pg;
    uint32_t aircr_clear = (self->SCB_AIRCR) & ~(AIRCR_PRIORITY_GROUPING_MSK);
    self->SCB_AIRCR = aircr_clear | (SCB_AIRCR_KEY<<16) | (pg_32<<8);
}


void SCB_enable_IRQ(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return;
    }

    switch(IRQn) {
        case SysTick_IRQn:
            printf("I haven't made the systick driver yet! could not enable systick\n");
            break;
        case UsageFault_IRQn:
            self->SCB_SHCSR |= USGFAULTENA_MSK;
            break;
        case BusFault_IRQn:
            self->SCB_SHCSR |= BUSFAULTENA_MSK;
            break;
        case MemoryManagement_IRQn:
            self->SCB_SHCSR |= MEMFAULTENA_MSK;
            break;
        default:
            printf("SCB: couldn't the enable IRQn, IRQn not supported: %d\n", IRQn);
    }
}

void SCB_disable_IRQ(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return;
    }

    switch (IRQn) {
        case SysTick_IRQn:
            printf("I haven't made the systick driver yet, could not disable systick interrupt\n");
            break;
        case UsageFault_IRQn:
            self->SCB_SHCSR &= ~(USGFAULTENA_MSK);
            break;
        case BusFault_IRQn:
            self->SCB_SHCSR &= ~(BUSFAULTENA_MSK);
            break;
        case MemoryManagement_IRQn:
            self->SCB_SHCSR &= ~(MEMFAULTENA_MSK);
            break;
        default:
            printf("SCB: couldn't the disable IRQn, IRQn not supported: %d\n", IRQn);
    }
}

void SCB_set_pending_IRQ(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return;
    }
    switch (IRQn) {
        case SysTick_IRQn:
            self->SCB_ICSR = ICSR_SYSTICK_SET_PENDING_MSK;
            break;
        case UsageFault_IRQn:
            self->SCB_SHCSR |= SHCSR_USGFAULT_SET_PENDING_MSK;
            break;
        case BusFault_IRQn:
            self->SCB_SHCSR |= SHCSR_BUSFAULT_SET_PENDING_MSK;
            break;
        case MemoryManagement_IRQn:
            self->SCB_SHCSR |= SHCSR_MEMFAULT_SET_PENDING_MSK;
            break;
        default:
            printf("SCB: couldn't the pending bit, IRQn not supported: %d\n", IRQn);
    }
}

void SCB_clear_pending_IRQ(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return;
    }

    switch(IRQn){
        case SysTick_IRQn:
            self->SCB_ICSR = ICSR_SYSTICK_CLR_PENDING_MSK;
            break;
        case UsageFault_IRQn:
            self->SCB_SHCSR &= SHCSR_USGFAULT_CLEAR_PENDING_MSK;
            break;
        case BusFault_IRQn:
            self->SCB_SHCSR &= SHCSR_BUSFAULT_CLEAR_PENDING_MSK;
            break;
        case MemoryManagement_IRQn:
            self->SCB_SHCSR &= SHCSR_MEMFAULT_CLEAR_PENDING_MSK;
            break;
        default:
            printf("SCB: couldn't clear the pending bit, IRQn not supported: %d\n", IRQn);
    }
}

uint32_t SCB_get_pending_IRQ(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return 0;
    }

    switch(IRQn){
        case SysTick_IRQn:
            return ((uint32_t) ICSR_SYSTICK_IS_PENDING_MSK(self));            
        case UsageFault_IRQn:
            return ((uint32_t) SHCSR_USGFAULT_IS_PENDING_MSK(self));
        case BusFault_IRQn:
            return ((uint32_t) SHCSR_BUSFAULT_IS_PENDING_MSK(self));
        case MemoryManagement_IRQn:
            return ((uint32_t) SHCSR_MEMFAULT_IS_PENDING_MSK(self));
        default:
            printf("SCB: couldn't get the pending bit, IRQn not supported: %d\n", IRQn);
    }
    return 0;
}

uint32_t SCB_get_active(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return 0;
    }

    switch(IRQn){
        case SysTick_IRQn:
            return ((uint32_t) SHCSR_SYSTICK_IS_ACTIVE_MSK(self));
            break;
        case UsageFault_IRQn:
            return ((uint32_t) SHCSR_USGFAULT_IS_ACTIVE_MSK(self));
            break;
        case BusFault_IRQn:
            return ((uint32_t) SHCSR_BUSFAULT_IS_ACTIVE_MSK(self));
            break;
        case MemoryManagement_IRQn:
            return ((uint32_t) SHCSR_MEMFAULT_IS_ACTIVE_MSK(self));
            break;
        default:
            printf("SCB: couldn't get the active bit, IRQn not supported: %d\n", IRQn);
            return 0;
    }
}

static inline void set_systick_priority(SCB_t * const self, uint32_t priority){
    self->SCB_SHPR[2] &= ~(0xFUL << 28);
    self->SCB_SHPR[2] |= (priority<<28);
}

static inline void set_usage_fault_priority(SCB_t * const self, uint32_t priority){
    self->SCB_SHPR[0] &= ~(0xFUL<<20);
    self->SCB_SHPR[0] |= (priority<<20);
}

static inline void set_bus_fault_priority(SCB_t * const self, uint32_t priority){
    self->SCB_SHPR[0] &= ~(0xFUL<<12);
    self->SCB_SHPR[0] |= (priority<<12);
}

static inline void set_mem_manage_priority(SCB_t * const self, uint32_t priority){
    self->SCB_SHPR[0] &= ~(0xFUL<<4);
    self->SCB_SHPR[0] |= (priority<<4);
}

void SCB_set_priority(SCB_t * const self, IRQn_t IRQn, uint32_t priority){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return;
    }
    if(priority > 15){
        printf("SCB: could not set the priority for IRQn: %d, becauase the priority is too high, Priority: %d\n", IRQn, priority);
        return;
    }
    
    switch (IRQn) {
        case SysTick_IRQn:
            set_systick_priority(self, priority);
            break;
        case UsageFault_IRQn:
            set_usage_fault_priority(self, priority);
            break;
        case BusFault_IRQn:
            set_bus_fault_priority(self, priority);
            break;
        case MemoryManagement_IRQn:
            set_mem_manage_priority(self, priority);
            break;
        default:
            printf("SCB: could not set the priority for IRQn: %d, not supported\n", IRQn);
    }   
}

static inline uint32_t get_systick_priority(SCB_t * const self){
    uint32_t priority = (self->SCB_SHPR[2] >> 28UL) & 0x0F;
    return priority; 
}

static inline uint32_t get_usage_fault_priority(SCB_t * const self){
    uint32_t priority = ((self->SCB_SHPR[0] >> 20) & 0x0FUL);
    return priority;
}

static inline uint32_t get_mem_manage_priority(SCB_t * const self){
    uint32_t priority = ((self->SCB_SHPR[0] >> 4) & 0x0FUL);
    return priority;
}

static inline uint32_t get_bus_fault_priority(SCB_t * const self){
    uint32_t priority = ((self->SCB_SHPR[0] >> 12) & 0x0FUL);
    return priority;
}

uint32_t SCB_get_priority(SCB_t * const self, IRQn_t IRQn){
    if(IRQn >= 0){
        printf("SCB: IRQn greater than 0, check failed, IRQn: %d\n", IRQn);
        return 0;
    }
    
    switch (IRQn) {
        case SysTick_IRQn:
            return get_systick_priority(self);
        case UsageFault_IRQn:
            return get_usage_fault_priority(self);
        case BusFault_IRQn:
            return get_bus_fault_priority(self);
        case MemoryManagement_IRQn:
            return get_mem_manage_priority(self);
        default:
            printf("SCB: could not get the priority for IRQn: %d, not supported\n", IRQn);
            return 0;
    }   
}
