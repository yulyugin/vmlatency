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

typedef struct vmpage {
        struct page *page;
        char *p;
        uintptr_t pa;
} vmpage_t;

typedef unsigned long irq_flags_t;

int allocate_vmpage(vmpage_t *p);
void free_vmpage(vmpage_t *p);

void vmlatency_preempt_disable(irq_flags_t *irq_flags);
void vmlatency_preempt_enable(irq_flags_t irq_flags);

int vmlatency_printm(const char *fmt, ...);

#define vmlatency_printk(...) vmlatency_printm("[vmlatency] " __VA_ARGS__)

#endif /* __VMM_API_H__*/
