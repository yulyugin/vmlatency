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

#ifndef __VMX_H__
#define __VMX_H__

#include "types.h"

typedef struct vm_monitor {
        char *vmxon_region;
        uintptr_t vmxon_region_pa;

        char *vmcs;
        uintptr_t vmcs_pa;

        u64 old_vmxe;
} vm_monitor_t;

int vmlatency_printk(const char *fmt, ...);

bool vmx_enabled(void);

void print_vmx_info(void);

void measure_vmlatency(void);

#endif /* __VMX_H__ */
