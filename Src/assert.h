#ifndef ASSERT_H
#define ASSERT_H

#include "services/print/printf.h"

#ifdef DEBUG

#define BARE_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            printf_("ASSERT FAILED: %s\nFile: %s\nLine: %d\n", \
                             #expr, __FILE__, __LINE__); \
            __asm__ volatile ("BKPT #0"); \
            while (1) { __asm__ volatile ("WFI"); } \
        } \
    } while(0)

#else

// In release mode, the assert evaluates to a void expression.
// This compiles to absolutely zero machine instructions but 
// prevents compiler warnings about empty statements.
#define BARE_ASSERT(expr) ((void)0)

#endif // DEBUG

#endif