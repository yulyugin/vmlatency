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

#ifndef __CPU_DEFS_H__
#define __CPU_DEFS_H__

#define BIT(n) (1ull << n)

/* MSR numbers */
#define MSR_IA32_FEATURE_CONTROL 0x3a

/* Fields of IA32_FEATURE_CONTROL MSR */
#define FEATURE_CONTROL_LOCK_BIT                   BIT(0)
#define FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT BIT(2)

/* CPUID bits */
#define CPUID_1_ECX_VMX BIT(5)

#endif /* __CPU_DEFS_H__ */
