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

#define __BIT(n) (1ull << n)

/* MSR numbers */
#define IA32_FEATURE_CONTROL 0x3a

#define IA32_VMX_BASIC       0x480

/* Fields of IA32_FEATURE_CONTROL MSR */
#define FEATURE_CONTROL_LOCK_BIT                   __BIT(0)
#define FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT __BIT(2)

/* CPUID bits */
#define CPUID_1_ECX_VMX __BIT(5)

/* Control registers */
#define CR4_VMXE __BIT(13)

/* RFLAGS fields */
#define RFLAGS_CF __BIT(0)
#define RFLAGS_ZF __BIT(6)

/* VMCS guest state */
#define VMCS_GUEST_ES     0x0800
#define VMCS_GUEST_CS     0x0802
#define VMCS_GUEST_SS     0x0804
#define VMCS_GUEST_DS     0x0806
#define VMCS_GUEST_FS     0x0808
#define VMCS_GUEST_GS     0x080a
#define VMCS_GUEST_LDTR   0x080c
#define VMCS_GUEST_TR     0x080e
#define VMCS_GUEST_RIP    0x681e

/* VMCS host state */
#define VMCS_HOST_ES      0x0c00
#define VMCS_HOST_CS      0x0c02
#define VMCS_HOST_SS      0x0c04
#define VMCS_HOST_DS      0x0c06
#define VMCS_HOST_FS      0x0c08
#define VMCS_HOST_GS      0x0c0a
#define VMCS_HOST_TR      0x0c0c
#define VMCS_HOST_RIP     0x6c16

/* 64-bit control fields */
#define VMCS_IO_BITMAP_A_ADDR 0x2000
#define VMCS_IO_BITMAP_B_ADDR 0x2002
#define VMCS_EXEC_VMCS_PTR    0x200c
#define VMCS_TSC_OFFSET       0x2010

/* 64-bit guest state */
#define VMCS_VMCS_LINK_PTR       0x2800
#define VMCS_GUEST_IA32_DEBUGCTL 0x2802

/* 32-bit control fields */
#define VMCS_PIN_BASED_VM_CTRL    0x4000
#define VMCS_PROC_BASED_VM_CTRL   0x4002
#define VMCS_EXCEPTION_BITMAP     0x4004
#define VMCS_PF_ECODE_MASK        0x4006
#define VMCS_PF_ECODE_MATCH       0x4008
#define VMCS_CR3_TARGET_CNT       0x400a
#define VMCS_VMEXIT_CTRL          0x400c
#define VMCS_VMEXIT_MSR_STORE_CNT 0x400e
#define VMCS_VMEXIT_MSR_LOAD_CNT  0x4010
#define VMCS_VMENTRY_CTL          0x4012
#define VMCS_VMENTRY_MSR_LOAD_CNT 0x4014
#define VMCS_VMENTRY_INT_INFO     0x4016
#define VMCS_VMENTRY_ECODE        0x4018
#define VMCS_VMENTRY_INSTR_LEN    0x401a

#endif /* __CPU_DEFS_H__ */
