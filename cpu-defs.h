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
#define IA32_FEATURE_CONTROL         0x3a

#define IA32_SYSENTER_CS             0x174
#define IA32_SYSENTER_ESP            0x175
#define IA32_SYSENTER_EIP            0x176

#define IA32_VMX_BASIC               0x480
#define IA32_VMX_PINBASED_CTLS       0x481
#define IA32_VMX_PROCBASED_CTLS      0x482
#define IA32_VMX_EXIT_CTLS           0x483
#define IA32_VMX_ENTRY_CTLS          0x484
#define IA32_VMX_MSR_MISC            0x485
#define IA32_VMX_CR0_FIXED0          0x486
#define IA32_VMX_CR0_FIXED1          0x487
#define IA32_VMX_CR4_FIXED0          0x488
#define IA32_VMX_CR4_FIXED1          0x489
#define IA32_VMX_VMCS_ENUM           0x48a
#define IA32_VMX_PROCBASED_CTLS2     0x48b
#define IA32_VMX_EPT_VPID_CAP        0x48c
#define IA32_VMX_TRUE_PINBASED_CTLS  0x48d
#define IA32_VMX_TRUE_PROCBASED_CTLS 0x48e
#define IA32_VMX_TRUE_EXIT_CTLS      0x48f
#define IA32_VMX_TRUE_ENTRY_CTLS     0x490
#define IA32_VMX_VMFUNC              0x491

/* Fields of IA32_FEATURE_CONTROL MSR */
#define FEATURE_CONTROL_LOCK_BIT                   __BIT(0)
#define FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT __BIT(2)

/* Pin-based VM-execution controls */
#define VMX_PIN_CTL_EXT_INTERRUPT_EXITING  __BIT(0)
#define VMX_PIN_CTL_NMI_EXITING            __BIT(3)
#define VMX_PIN_CTL_VIRTUAL_NMIS           __BIT(5)
#define VMX_PIN_CTL_ACTIVATE_PTIMER        __BIT(6)
#define VMX_PIN_CTL_PROC_POSTED_INTERRUPTS __BIT(7)

/* Primary proc-based VM-execution controls */
#define VMX_PROC_CTL_INTERRUPT_WINDOW_EXITING __BIT(2)
#define VMX_PROC_CTL_USE_TSC_OFFSETTING       __BIT(3)
#define VMX_PROC_CTL_HLT_EXITING              __BIT(7)
#define VMX_PROC_CTL_INVLPG_EXITING           __BIT(9)
#define VMX_PROC_CTL_MWAIT_EXITING            __BIT(10)
#define VMX_PROC_CTL_RDPMC_EXITING            __BIT(11)
#define VMX_PROC_CTL_RDTSC_EXITING            __BIT(12)
#define VMX_PROC_CTL_CR3_LOAD_EXITING         __BIT(15)
#define VMX_PROC_CTL_CR3_STORE_EXITING        __BIT(16)
#define VMX_PROC_CTL_CR8_LOAD_EXITING         __BIT(19)
#define VMX_PROC_CTL_CR8_STORE_EXITING        __BIT(20)
#define VMX_PROC_CTL_USE_TPR_SHADOW           __BIT(21)
#define VMX_PROC_CTL_NMI_WINDOW_EXITING       __BIT(22)
#define VMX_PROC_CTL_MOV_DR_EXITING           __BIT(23)
#define VMX_PROC_CTL_UNCONDITIONAL_IO_EXITING __BIT(24)
#define VMX_PROC_CTL_USE_IO_BITMAPS           __BIT(25)
#define VMX_PROC_CTL_MONITOR_TRAP_FLAG        __BIT(27)
#define VMX_PROC_CTL_USE_MSR_BITMAPS          __BIT(28)
#define VMX_PROC_CTL_MONITOR_EXITING          __BIT(29)
#define VMX_PROC_CTL_PAUSE_EXITING            __BIT(30)
#define VMX_PROC_CTL_ACTIVATE_SECONDARY_CTLS  __BIT(31)

/* Secondary proc-based VM-execution controls */
#define VMX_PROC_CTL2_VIRTUALIZE_APIC         __BIT(0)
#define VMX_PROC_CTL2_ENABLE_EPT              __BIT(1)
#define VMX_PROC_CTL2_DESCR_TABLE_EXITING     __BIT(2)
#define VMX_PROC_CTL2_ENABLE_RDTSCP           __BIT(3)
#define VMX_PROC_CTL2_VIRTUALIZE_X2APIC       __BIT(4)
#define VMX_PROC_CTL2_ENABLE_VPID             __BIT(5)
#define VMX_PROC_CTL2_WBINVD_EXITING          __BIT(6)
#define VMX_PROC_CTL2_URESTRICTED_GUEST       __BIT(7)
#define VMX_PROC_CTL2_APIC_REGISTER_VIRT      __BIT(8)
#define VMX_PROC_CTL2_VIRTUAL_INTERRUPTS      __BIT(9)
#define VMX_PROC_CTL2_PAUSE_LOOP_EXITING      __BIT(10)
#define VMX_PROC_CTL2_RDRAND_EXITING          __BIT(11)
#define VMX_PROC_CTL2_ENABLE_INVPCID          __BIT(12)
#define VMX_PROC_CTL2_ENABLE_VMFUNC           __BIT(13)
#define VMX_PROC_CTL2_VMCS_SHADOWING          __BIT(14)
#define VMX_PROC_CTL2_ENCLS_EXITING           __BIT(15)
#define VMX_PROC_CTL2_RDSEED_EXITING          __BIT(16)
#define VMX_PROC_CTL2_ENABLE_PML              __BIT(17)
#define VMX_PROC_CTL2_EPT_VIOLATION_VE        __BIT(18)
#define VMX_PROC_CTL2_CONCEAL_NONROOT_FROM_PT __BIT(19)
#define VMX_PROC_CTL2_ENABLE_XSAVES_XRSTORS   __BIT(20)
#define VMX_PROC_CTL2_MODE_BASED_EPT_CONTROL  __BIT(22)
#define VMX_PROC_CTL2_USE_TSC_SCALING         __BIT(25)

/* VM exit controls */
#define VMCS_VMEXIT_CTL_SAVE_DBG_CTLS              __BIT(2)
#define VMCS_VMEXIT_CTL_HOST_ADDR_SPACE_SIZE       __BIT(9)
#define VMCS_VMEXIT_CTL_LOAD_IA32_PERF_GLOBAL_CTRL __BIT(12)
#define VMCS_VMEXIT_CTL_ACK_INTERRUPT_ON_EXIT      __BIT(15)
#define VMCS_VMEXIT_CTL_SAVE_IA32_PAT              __BIT(18)
#define VMCS_VMEXIT_CTL_LOAD_IA32_PAT              __BIT(19)
#define VMCS_VMEXIT_CTL_SAVE_IA32_EFER             __BIT(20)
#define VMCS_VMEXIT_CTL_LOAD_IA32_EFER             __BIT(21)
#define VMCS_VMEXIT_CTL_SAVE_PTIMER_VAL            __BIT(22)
#define VMCS_VMEXIT_CTL_CLEAR_IA32_BNDCFGS         __BIT(23)
#define VMCS_VMEXIT_CTL_CONCEAL_VMEXIT_FROM_PT     __BIT(24)

/* VM entry controls */
#define VMCS_VMENTRY_CTL_LOAD_DBG_CTLS                      __BIT(2)
#define VMCS_VMENTRY_CTL_IA32E_MODE_GUEST                   __BIT(9)
#define VMCS_VMENTRY_CTL_ENTRY_TO_SMM                       __BIT(10)
#define VMCS_VMENTRY_CTL_DEACTIVATE_DUAL_MONITOR_TREATEMENT __BIT(11)
#define VMCS_VMENTRY_CTL_LOAD_IA32_PERF_GLOBAL_CTRL         __BIT(13)
#define VMCS_VMENTRY_CTL_LOAD_IA32_PAT                      __BIT(14)
#define VMCS_VMENTRY_CTL_LOAD_IA32_EFER                     __BIT(15)
#define VMCS_VMENTRY_CTL_LOAD_IA32_BNDCFGS                  __BIT(16)
#define VMCS_VMENTRY_CTL_CONCEAL_VMENTRY_FROM_PT            __BIT(17)

/* CPUID bits */
#define CPUID_1_ECX_VMX __BIT(5)

/* Control registers */
#define CR4_VMXE __BIT(13)

/* RFLAGS fields */
#define RFLAGS_CF __BIT(0)
#define RFLAGS_ZF __BIT(6)

/* VMCS guest state */

/* 16-bit guest state */
#define VMCS_GUEST_ES     0x0800
#define VMCS_GUEST_CS     0x0802
#define VMCS_GUEST_SS     0x0804
#define VMCS_GUEST_DS     0x0806
#define VMCS_GUEST_FS     0x0808
#define VMCS_GUEST_GS     0x080a
#define VMCS_GUEST_LDTR   0x080c
#define VMCS_GUEST_TR     0x080e

/* 64-bit guest state */
#define VMCS_VMCS_LINK_PTR       0x2800
#define VMCS_GUEST_IA32_DEBUGCTL 0x2802

/* 32-bit guest state */
#define VMCS_GUEST_ES_LIMIT               0x4800
#define VMCS_GUEST_CS_LIMIT               0x4802
#define VMCS_GUEST_SS_LIMIT               0x4804
#define VMCS_GUEST_DS_LIMIT               0x4806
#define VMCS_GUEST_FS_LIMIT               0x4808
#define VMCS_GUEST_GS_LIMIT               0x480a
#define VMCS_GUEST_LDTR_LIMIT             0x480c
#define VMCS_GUEST_TR_LIMIT               0x480e
#define VMCS_GUEST_GDTR_LIMIT             0x4810
#define VMCS_GUEST_IDTR_LIMIT             0x4812
#define VMCS_GUEST_ES_ACCESS_RIGHTS       0x4814
#define VMCS_GUEST_CS_ACCESS_RIGHTS       0x4816
#define VMCS_GUEST_SS_ACCESS_RIGHTS       0x4818
#define VMCS_GUEST_DS_ACCESS_RIGHTS       0x481a
#define VMCS_GUEST_FS_ACCESS_RIGHTS       0x481c
#define VMCS_GUEST_GS_ACCESS_RIGHTS       0x481e
#define VMCS_GUEST_LDTR_ACCESS_RIGHTS     0x4820
#define VMCS_GUEST_TR_ACCESS_RIGHTS       0x4822
#define VMCS_GUEST_INTERRUPTIBILITY_STATE 0x4824
#define VMCS_GUEST_ACTIVITY_STATE         0x4826
#define VMCS_GUEST_SMBASE                 0x4828
#define VMCS_GUEST_IA32_SYSENTER_CS       0x482a

/* Natural-width guest state */
#define VMCS_GUEST_CR0                   0x6800
#define VMCS_GUEST_CR3                   0x6802
#define VMCS_GUEST_CR4                   0x6804
#define VMCS_GUEST_ES_BASE               0x6806
#define VMCS_GUEST_CS_BASE               0x6808
#define VMCS_GUEST_SS_BASE               0x680a
#define VMCS_GUEST_DS_BASE               0x680c
#define VMCS_GUEST_FS_BASE               0x680e
#define VMCS_GUEST_GS_BASE               0x6810
#define VMCS_GUEST_LDTR_BASE             0x6812
#define VMCS_GUEST_TR_BASE               0x6814
#define VMCS_GUEST_GDTR_BASE             0x6816
#define VMCS_GUEST_IDTR_BASE             0x6818
#define VMCS_GUEST_DR7                   0x681a
#define VMCS_GUEST_RSP                   0x681c
#define VMCS_GUEST_RIP                   0x681e
#define VMCS_GUEST_RFLAGS                0x6820
#define VMCS_GUEST_PENDING_DBG_EXCEPTION 0x6822
#define VMCS_GUEST_IA32_SYSENTER_ESP     0x6824
#define VMCS_GUEST_IA32_SYSENTER_EIP     0x6826

/* VMCS host state */

/* 16-bit host state */
#define VMCS_HOST_ES      0x0c00
#define VMCS_HOST_CS      0x0c02
#define VMCS_HOST_SS      0x0c04
#define VMCS_HOST_DS      0x0c06
#define VMCS_HOST_FS      0x0c08
#define VMCS_HOST_GS      0x0c0a
#define VMCS_HOST_TR      0x0c0c

/* 32-bit host state */
#define VMCS_HOST_IA32_SYSENTER_CS 0x4c00

/* Natural-width host state */
#define VMCS_HOST_CR0               0x6c00
#define VMCS_HOST_CR3               0x6c02
#define VMCS_HOST_CR4               0x6c04
#define VMCS_HOST_FS_BASE           0x6c06
#define VMCS_HOST_GS_BASE           0x6c08
#define VMCS_HOST_TR_BASE           0x6c0a
#define VMCS_HOST_GDTR_BASE         0x6c0c
#define VMCS_HOST_IDTR_BASE         0x6c0e
#define VMCS_HOST_IA32_SYSENTER_ESP 0x6c10
#define VMCS_HOST_IA32_SYSENTER_EIP 0x6c12
#define VMCS_HOST_RSP               0x6c14
#define VMCS_HOST_RIP               0x6c16

/* VMCS controls */

/* 64-bit control fields */
#define VMCS_IO_BITMAP_A_ADDR 0x2000
#define VMCS_IO_BITMAP_B_ADDR 0x2002
#define VMCS_EXEC_VMCS_PTR    0x200c
#define VMCS_TSC_OFFSET       0x2010

/* 32-bit control fields */
#define VMCS_PIN_BASED_VM_CTLS    0x4000
#define VMCS_PROC_BASED_VM_CTLS   0x4002
#define VMCS_EXCEPTION_BITMAP     0x4004
#define VMCS_PF_ECODE_MASK        0x4006
#define VMCS_PF_ECODE_MATCH       0x4008
#define VMCS_CR3_TARGET_CNT       0x400a
#define VMCS_VMEXIT_CTLS          0x400c
#define VMCS_VMEXIT_MSR_STORE_CNT 0x400e
#define VMCS_VMEXIT_MSR_LOAD_CNT  0x4010
#define VMCS_VMENTRY_CTLS         0x4012
#define VMCS_VMENTRY_MSR_LOAD_CNT 0x4014
#define VMCS_VMENTRY_INT_INFO     0x4016
#define VMCS_VMENTRY_ECODE        0x4018
#define VMCS_VMENTRY_INSTR_LEN    0x401a

/* Natural-width control fields */
#define VMCS_CR0_GUEST_HOST_MASK 0x6000
#define VMCS_CR4_GUEST_HOST_MASK 0x6002
#define VMCS_CR0_READ_SHADOW     0x6004
#define VMCS_CR4_READ_SHADOW     0x6006
#define VMCS_CR3_TARGET_VALUE_0  0x6008
#define VMCS_CR3_TARGET_VALUE_1  0x600a
#define VMCS_CR3_TARGET_VALUE_2  0x600c
#define VMCS_CR3_TARGET_VALUE_3  0x600e

#endif /* __CPU_DEFS_H__ */
