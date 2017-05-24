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

static inline u64 __rdmsr(u32 msr_num)
{
        u32 eax, edx;
        __asm__ __volatile__(
                "rdmsr"
                :"=a"(eax), "=d"(edx)
                :"c"(msr_num));
        return ((u64)edx << 32) | eax;
}

#endif /* __ASM_INLINES_H__ */
