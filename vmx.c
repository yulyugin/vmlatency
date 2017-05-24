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

#include <linux/printk.h>

#include "vmx.h"
#include "asm-inlines.h"
#include "cpu-defs.h"

int
vmlatency_printk(const char *fmt, ...)
{
        int ret;
        va_list va;
        va_start(va, fmt);
        ret = vprintk(fmt, va);
        va_end(va);
        return ret;
}

static inline bool
has_vmx(void)
{
        u32 ecx = __cpuid_ecx(1, 0);
        return ecx & CPUID_1_ECX_VMX;
}

bool
vmx_enabled()
{
        u64 feature_control;
        if (!has_vmx()) {
                vmlatency_printk("VMX is not supported\n");
                return false;
        }

        feature_control = __rdmsr(MSR_IA32_FEATURE_CONTROL);
        if (!(feature_control & FEATURE_CONTROL_LOCK_BIT) ||
            !(feature_control & FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT)) {
                vmlatency_printk("VMX is not enabled in BIOS\n");
                return false;
        }

        return true;
}
