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

#ifndef __ASM_INLINES_H__
#define __ASM_INLINES_H__

#include "types.h"
#include "cpu-defs.h"

#include <linux/version.h>

#define SAVE_RFLAGS(rflags) \
        "pushfq;"           \
        "popq %0;"          \
        :"=r"(rflags)

static inline void
__cpuid_all(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
        __asm__ __volatile__(
                "cpuid"
                :"=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                :"a"(leaf), "c"(subleaf));
}

static inline u32
__cpuid_ecx(u32 leaf, u32 subleaf)
{
        u32 eax, ebx, ecx, edx;
        __cpuid_all(leaf, subleaf, &eax, &ebx, &ecx, &edx);
        return ecx;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
static inline u64 __rdmsr(u32 msr_num)
{
        u32 eax, edx;
        __asm__ __volatile__(
                "rdmsr"
                :"=a"(eax), "=d"(edx)
                :"c"(msr_num));
        return ((u64)edx << 32) | eax;
}
#else
#include <asm/msr.h>
#endif

static inline u64
__get_cr0(void)
{
        u64 cr0;
        __asm__ __volatile__(
                "movq %%cr0, %0"
                :"=r"(cr0):);
        return cr0;
}

static inline u64
__get_cr3(void)
{
        u64 cr3;
        __asm__ __volatile__(
                "movq %%cr3, %0"
                :"=r"(cr3):);
        return cr3;
}

static inline u64
__get_cr4(void)
{
        u64 cr4;
        __asm__ __volatile__(
                "movq %%cr4, %0"
                :"=r"(cr4):);
        return cr4;
}

static inline void
__set_cr4(u64 cr4)
{
        __asm__ __volatile__(
                "movq %0, %%cr4"
                ::"r"(cr4));
}

static inline u64
__lar(u16 seg)
{
        u64 attrs;
        __asm__ __volatile__("lar %1, %0":"=r"(attrs):"r"(seg));
        return attrs;
}

static inline int
__vmxon(uintptr_t vmxon_region_pa)
{
        u64 rflags;
        __asm__ __volatile__(
                "vmxon %1;"
                SAVE_RFLAGS(rflags)
                :"m"(vmxon_region_pa));
        if (rflags & (RFLAGS_CF | RFLAGS_ZF))
                return -1;
        return 0;
}

static inline int
__vmxoff(void)
{
        u64 rflags;
        __asm__ __volatile__(
                "vmxoff;"
                SAVE_RFLAGS(rflags));
        if (rflags & (RFLAGS_CF | RFLAGS_ZF))
                return -1;
        return 0;
}

static inline int
__vmptrld(uintptr_t vmcs_pa)
{
        u64 rflags;
        __asm__ __volatile__(
                "vmptrld %1;"
                SAVE_RFLAGS(rflags)
                :"m"(vmcs_pa));
        if (rflags & (RFLAGS_CF | RFLAGS_ZF))
                return -1;
        return 0;
}

static inline int
__vmclear(uintptr_t vmcs_pa)
{
        u64 rflags;
        __asm__ __volatile__(
                "vmclear %1;"
                SAVE_RFLAGS(rflags)
                :"m"(vmcs_pa));
        if (rflags & (RFLAGS_CF | RFLAGS_ZF))
                return -1;
        return 0;
}

static inline int
__vmlaunch(void)
{
        u64 rflags;
        __asm__ __volatile__(
                "vmlaunch;"
                SAVE_RFLAGS(rflags));
        if (rflags & (RFLAGS_CF | RFLAGS_ZF))
                return -1;
        return 0;
}

static inline void
__vmwrite(u64 field, u64 value)
{
        __asm__ __volatile__(
                "vmwrite %1, %0;"
                ::"r"(field), "m"(value));
}

static inline u64
__vmread(u64 field)
{
        u64 ret;
        __asm__ __volatile__(
                "vmread %1, %0"
                :"=r"(ret): "r"(field));
        return ret;
}

#endif /* __ASM_INLINES_H__ */
