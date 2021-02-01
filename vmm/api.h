/*
 * Copyright (c) 2017 Evgenii Iuliugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __VMM_API_H__
#define __VMM_API_H__

#include "types.h"

#ifdef __APPLE__
#include <IOKit/IOLocks.h>
#ifdef __cplusplus
#include <IOKit/IOBufferMemoryDescriptor.h>
#else
/* Define IOBufferMemoryDescriptor for C code.
 * API is implemented in C++, but vm_page_t is used in C.
 * Define IOBufferMemoryOperation as void for C to make it compilable. */
typedef void IOBufferMemoryDescriptor;
#endif
#endif

typedef struct vmpage {
        /* public fields */
        char *p;
        uintptr_t pa;

        /* private fields */
#ifdef __linux__
    struct page *page;
#elif defined(__APPLE__)
        IOBufferMemoryDescriptor *page;
#else  /* Windows */
#endif
} vmpage_t;

#ifdef __linux__
typedef unsigned long irq_flags_t;
#elif defined(__APPLE__)
typedef struct irq_flags {
        IOSimpleLock *lock;
        IOInterruptState interrupt_state;
} irq_flags_t;
#else  /* Windows */
typedef struct irq_flags {
        unsigned __int64 eflags;
        KIRQL irql;
} irq_flags_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int allocate_vmpage(vmpage_t *p);
void free_vmpage(vmpage_t *p);

void vmlatency_preempt_disable(irq_flags_t *irq_flags);
void vmlatency_preempt_enable(irq_flags_t irq_flags);

int vmlatency_printm(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#define vmlatency_printk(...) vmlatency_printm("[vmlatency] " __VA_ARGS__)

#endif /* __VMM_API_H__*/
