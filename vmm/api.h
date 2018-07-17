/*
 * Copyright (c) 2017 Evgeny Yulyugin
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

/* Define IOBufferMemoryDescriptor for C code.
 * API is implemented in C++, but vm_page_t is used in C.
 * Let's define IOBufferMemoryOperation as void for C to make it compilable. */
#ifdef __APPLE__
#ifdef __cplusplus
#include <IOKit/IOBufferMemoryDescriptor.h>
#else
typedef void IOBufferMemoryDescriptor;
#endif
#endif

typedef struct vmpage {
        char *p;
        uintptr_t pa;
#ifdef __linux__
    struct page *page;
#else
#ifdef __APPLE__
    IOBufferMemoryDescriptor *page;
#else  /* Windows */
#endif
#endif
} vmpage_t;

typedef unsigned long irq_flags_t;

CLINKAGE int allocate_vmpage(vmpage_t *p);
CLINKAGE void free_vmpage(vmpage_t *p);

CLINKAGE void vmlatency_preempt_disable(irq_flags_t *irq_flags);
CLINKAGE void vmlatency_preempt_enable(irq_flags_t irq_flags);

CLINKAGE int vmlatency_printm(const char *fmt, ...);

#define vmlatency_printk(...) vmlatency_printm("[vmlatency] " __VA_ARGS__)

#endif /* __VMM_API_H__*/
