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

#include "vmx.h"
#include "api.h"
#include "asm-inlines.h"
#include "cpu-defs.h"

extern void guest_code(void);

typedef struct {
    descriptor_t gdt;
    descriptor_t idt;
} host_state_t;

static void
save_host_state(host_state_t *s)
{
        __get_gdt(&s->gdt);
        __get_idt(&s->idt);
}

static void
restore_host_state(host_state_t *s)
{
        __set_gdt(&s->gdt);
        __set_idt(&s->idt);
}

static inline int
allocate_memory(vm_monitor_t *vmm)
{
        int cnt = 0;
        if (allocate_vmpage(&vmm->vmxon_region) == 0) cnt++; else return -cnt;
        if (allocate_vmpage(&vmm->vmcs) == 0) cnt++; else return -cnt;
        if (allocate_vmpage(&vmm->io_bitmap_a) == 0) cnt++; else return -cnt;
        if (allocate_vmpage(&vmm->io_bitmap_b) == 0) cnt++; else return -cnt;
        if (allocate_vmpage(&vmm->msr_bitmap) == 0) cnt++; else return -cnt;

        return cnt;
}

static inline void
free_memory(vm_monitor_t *vmm, int cnt)
{
        if (cnt == 5) { free_vmpage(&vmm->msr_bitmap); cnt--; }
        if (cnt == 4) { free_vmpage(&vmm->io_bitmap_b); cnt--; }
        if (cnt == 3) { free_vmpage(&vmm->io_bitmap_a); cnt--; }
        if (cnt == 2) { free_vmpage(&vmm->vmxon_region); cnt--; }
        if (cnt == 1) { free_vmpage(&vmm->vmcs); cnt--; }
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

        feature_control = __rdmsr(IA32_FEATURE_CONTROL);
        if (!(feature_control & FEATURE_CONTROL_LOCK_BIT) ||
            !(feature_control & FEATURE_CONTROL_VMX_OUTSIDE_SMX_ENABLE_BIT)) {
                vmlatency_printk("VMX is not enabled in BIOS\n");
                return false;
        }

        return true;
}

static inline u32
get_vmcs_revision_identifier(void)
{
        return 0x8fffffff & __rdmsr(IA32_VMX_BASIC);
}

static inline void
vmxon_setup_revision_id(vm_monitor_t *vmm)
{
        ((u32 *)vmm->vmxon_region.p)[0] = get_vmcs_revision_identifier();
}

static inline void
vmcs_setup_revision_id(vm_monitor_t *vmm)
{
        ((u32 *)vmm->vmcs.p)[0] = get_vmcs_revision_identifier();
}

static inline int
do_vmxon(vm_monitor_t *vmm)
{
        int ret;
        u64 old_cr4 = __get_cr4();
        vmm->old_vmxe = old_cr4 & CR4_VMXE;

        /* Set CR4.VMXE if necessary */
        if (!vmm->old_vmxe)
                __set_cr4(old_cr4 | CR4_VMXE);

        ret = __vmxon(vmm->vmxon_region.pa);
        if (ret == -1) {
                vmlatency_printk("VMXON failed\n");
                return -1;
        }

        vmm->our_vmxon = ret == 0;
        return 0;
}

static inline void
do_vmxoff(vm_monitor_t *vmm)
{
        if (vmm->our_vmxon && (__vmxoff() != 0))
                vmlatency_printk("VMXOFF failed\n");

        /* Clear CR4.VMXE if necessary */
        if (!vmm->old_vmxe)
                __set_cr4(__get_cr4() & ~CR4_VMXE);
}

static inline int
do_vmptrld(vm_monitor_t *vmm)
{
        if (__vmptrld(vmm->vmcs.pa) != 0){
                vmlatency_printk("VMPTRLD failed\n");
                return -1;
        }

        return 0;
}

static inline int
do_vmclear(vm_monitor_t *vmm)
{
        if (__vmclear(vmm->vmcs.pa) != 0){
                vmlatency_printk("VMCLEAR failed\n");
                return -1;
        }

        return 0;
}

static u64
get_segment_ar(u16 seg)
{
        return (__lar(seg) >> 0x8) & 0xf0ff; /* Clear undefined bits */
}

/* Initialize guest state to match host state */
static inline void
initialize_vmcs(vm_monitor_t *vmm)
{
        extern char vmx_exit[];  /* assembly export */
        u16 val16, tr, tr_limit;
        descriptor_t gdtr, idtr;
        u64 fs_base, gs_base;
        u64 trbase, trdesc_lo, trdesc_hi;
        u32 ia32_sysenter_cs;
        u64 ia32_sysenter_esp, ia32_sysenter_eip;
        u64 cr0, cr3, cr4;
        u64 rflags;

        /* Segment registers */
        val16 = __get_es();
        __vmwrite(VMCS_HOST_ES, val16 & ~7);
        __vmwrite(VMCS_GUEST_ES, val16);
        __vmwrite(VMCS_GUEST_ES_BASE, 0);
        __vmwrite(VMCS_GUEST_ES_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS, UNUSABLE_AR);

        val16 = __get_cs();
        __vmwrite(VMCS_HOST_CS, val16 & ~7);
        __vmwrite(VMCS_GUEST_CS, val16);
        __vmwrite(VMCS_GUEST_CS_BASE, 0);
        __vmwrite(VMCS_GUEST_CS_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_CS_ACCESS_RIGHTS, get_segment_ar(val16));

        val16 = __get_ss();
        __vmwrite(VMCS_HOST_SS, val16 & ~7);
        __vmwrite(VMCS_GUEST_SS, val16);
        __vmwrite(VMCS_GUEST_SS_BASE, 0);
        __vmwrite(VMCS_GUEST_SS_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_SS_ACCESS_RIGHTS, get_segment_ar(val16));

        val16 = __get_ds();
        __vmwrite(VMCS_HOST_DS, val16 & ~7);
        __vmwrite(VMCS_GUEST_DS, val16);
        __vmwrite(VMCS_GUEST_DS_BASE, 0);
        __vmwrite(VMCS_GUEST_DS_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_DS_ACCESS_RIGHTS, UNUSABLE_AR);

        val16 = __get_fs();
        __vmwrite(VMCS_HOST_FS, val16 & ~7);
        __vmwrite(VMCS_GUEST_FS, val16);
        fs_base = __rdmsr(IA32_FS_BASE);
        __vmwrite(VMCS_GUEST_FS_BASE, fs_base);
        __vmwrite(VMCS_HOST_FS_BASE, fs_base);
        __vmwrite(VMCS_GUEST_FS_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_FS_ACCESS_RIGHTS, UNUSABLE_AR);

        val16 = __get_gs();
        __vmwrite(VMCS_HOST_GS, val16 & ~7);
        __vmwrite(VMCS_GUEST_GS, val16);
        gs_base = __rdmsr(IA32_GS_BASE);
        __vmwrite(VMCS_GUEST_GS_BASE, gs_base);
        __vmwrite(VMCS_HOST_GS_BASE, gs_base);
        __vmwrite(VMCS_GUEST_GS_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_GS_ACCESS_RIGHTS, UNUSABLE_AR);

        val16 = __sldt();
        __vmwrite(VMCS_GUEST_LDTR, val16);
        __vmwrite(VMCS_GUEST_LDTR_BASE, 0);
        __vmwrite(VMCS_GUEST_LDTR_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, UNUSABLE_AR);

        __get_gdt(&gdtr);
        __vmwrite(VMCS_GUEST_GDTR_LIMIT, gdtr.limit);
        __vmwrite(VMCS_GUEST_GDTR_BASE, gdtr.base);
        __vmwrite(VMCS_HOST_GDTR_BASE, gdtr.base);

        __get_idt(&idtr);
        __vmwrite(VMCS_GUEST_IDTR_LIMIT, idtr.limit);
        __vmwrite(VMCS_GUEST_IDTR_BASE, idtr.base);
        __vmwrite(VMCS_HOST_IDTR_BASE, idtr.base);

        tr = __str();
        tr_limit = __lsl(tr);
        __vmwrite(VMCS_GUEST_TR, tr & ~7);
        __vmwrite(VMCS_HOST_TR, tr);
        __vmwrite(VMCS_GUEST_TR_LIMIT, tr_limit);
        __vmwrite(VMCS_GUEST_TR_ACCESS_RIGHTS, get_segment_ar(tr));
        /* Extracting TR.base from GDT */
        trdesc_lo = ((u64*)(gdtr.base + tr))[0];
        trbase = ((trdesc_lo >> 16) & 0xffffff)
               | (((trdesc_lo >> 56) & 0xff) << 24);
        trdesc_hi = ((u64*)(gdtr.base + tr))[1];
        trbase |= trdesc_hi << 32;
        __vmwrite(VMCS_GUEST_TR_BASE, trbase);
        __vmwrite(VMCS_HOST_TR_BASE, trbase);

        /* 64-bit control fields */
        __vmwrite(VMCS_IO_BITMAP_A_ADDR, vmm->io_bitmap_a.pa);
        __vmwrite(VMCS_IO_BITMAP_B_ADDR, vmm->io_bitmap_b.pa);
        __vmwrite(VMCS_EXEC_VMCS_PTR, 0);
        __vmwrite(VMCS_TSC_OFFSET, 0);

        /* 64-bit guest state */
        __vmwrite(VMCS_VMCS_LINK_PTR, 0xffffffffffffffffull);
        __vmwrite(VMCS_GUEST_IA32_DEBUGCTL, 0);

        /* 32-bit control fields */
        __vmwrite(VMCS_PIN_BASED_VM_CTLS, vmm->pinbased_allowed0 &
                                          vmm->pinbased_allowed1);
        /* Secondary controls are not activated */
        __vmwrite(VMCS_PROC_BASED_VM_CTLS, vmm->procbased_allowed0 &
                                           vmm->procbased_allowed1);
        __vmwrite(VMCS_EXCEPTION_BITMAP, 0xffffffff);
        __vmwrite(VMCS_PF_ECODE_MASK, 0);
        __vmwrite(VMCS_PF_ECODE_MATCH, 0);
        __vmwrite(VMCS_CR3_TARGET_CNT, 0);
        __vmwrite(VMCS_VMEXIT_CTLS, (vmm->exit_ctls_allowed0 &
                                     vmm->exit_ctls_allowed1) |
                                    VMCS_VMEXIT_CTL_HOST_ADDR_SPACE_SIZE);
        __vmwrite(VMCS_VMEXIT_MSR_STORE_CNT, 0);
        __vmwrite(VMCS_VMEXIT_MSR_LOAD_CNT, 0);
        __vmwrite(VMCS_VMENTRY_CTLS, (vmm->entry_ctls_allowed0 &
                                      vmm->entry_ctls_allowed1) |
                                     VMCS_VMENTRY_CTL_IA32E_MODE_GUEST);
        __vmwrite(VMCS_VMENTRY_MSR_LOAD_CNT, 0);
        __vmwrite(VMCS_VMENTRY_INT_INFO, 0);
        __vmwrite(VMCS_VMENTRY_ECODE, 0);
        __vmwrite(VMCS_VMENTRY_INSTR_LEN, 0);

        __vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE, 0);
        __vmwrite(VMCS_GUEST_ACTIVITY_STATE, 0);
        __vmwrite(VMCS_GUEST_SMBASE, 0);

        /* 32-bit guest state*/
        ia32_sysenter_cs = (u32)__rdmsr(IA32_SYSENTER_CS);
        __vmwrite(VMCS_GUEST_IA32_SYSENTER_CS, ia32_sysenter_cs);
        __vmwrite(VMCS_HOST_IA32_SYSENTER_CS, ia32_sysenter_cs);

        /* Control registers */
        cr0 = __get_cr0();
        __vmwrite(VMCS_GUEST_CR0, cr0);
        __vmwrite(VMCS_HOST_CR0, cr0);
        __vmwrite(VMCS_CR0_GUEST_HOST_MASK, 0);
        __vmwrite(VMCS_CR0_READ_SHADOW, 0);

        cr4 = __get_cr4();
        __vmwrite(VMCS_GUEST_CR4, cr4);
        __vmwrite(VMCS_HOST_CR4, cr4);
        __vmwrite(VMCS_CR4_GUEST_HOST_MASK, 0);
        __vmwrite(VMCS_CR4_READ_SHADOW, 0);

        cr3 = __get_cr3();
        __vmwrite(VMCS_GUEST_CR3, cr3);
        __vmwrite(VMCS_HOST_CR3, cr3);
        __vmwrite(VMCS_CR3_TARGET_VALUE_0, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_1, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_2, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_3, 0);

        /* Natural-width guest/host state */
        __vmwrite(VMCS_GUEST_DR7, 0x400); /* Initial value */

        /* Don't care about guest stack, because we exit immediately */
        __vmwrite(VMCS_GUEST_RSP, 0);
        /* Host stack pointer is saved just before vmentry */

        __vmwrite(VMCS_GUEST_RIP, (uintptr_t)&guest_code);
        __vmwrite(VMCS_HOST_RIP, (uintptr_t)vmx_exit);

        rflags = __get_rflags();
        __vmwrite(VMCS_GUEST_RFLAGS, rflags);

        __vmwrite(VMCS_GUEST_PENDING_DBG_EXCEPTION, 0);

        ia32_sysenter_esp = __rdmsr(IA32_SYSENTER_ESP);
        __vmwrite(VMCS_GUEST_IA32_SYSENTER_ESP, ia32_sysenter_esp);
        __vmwrite(VMCS_HOST_IA32_SYSENTER_ESP, ia32_sysenter_esp);

        ia32_sysenter_eip = __rdmsr(IA32_SYSENTER_EIP);
        __vmwrite(VMCS_GUEST_IA32_SYSENTER_EIP, ia32_sysenter_esp);
        __vmwrite(VMCS_HOST_IA32_SYSENTER_EIP, ia32_sysenter_eip);
}

#define PRINT_VMXCAP_MSR(name) do {                   \
        vmlatency_printk("%-30s (%#x): %#018llx\n",   \
                         #name, name, __rdmsr(name)); \
} while (0)

void
print_vmx_info()
{
        char brand_string[48];
        bool has_true_ctls, has_secondary_ctls, has_ept, has_vmfunc;
        int i;
        for (i = 0; i < 3; i++) {
                __cpuid_all(0x80000002 + i, 0, (u32*)&brand_string[0 + 16 * i],
                            (u32*)&brand_string[4 + 16 * i],
                            (u32*)&brand_string[8 + 16 * i],
                            (u32*)&brand_string[12 + 16 * i]);
        }
        vmlatency_printk("%s\n", brand_string);

        has_true_ctls = !!(__rdmsr(IA32_VMX_BASIC) & __BIT(55));

        PRINT_VMXCAP_MSR(IA32_VMX_BASIC);
        PRINT_VMXCAP_MSR(IA32_VMX_PINBASED_CTLS);
        PRINT_VMXCAP_MSR(IA32_VMX_PROCBASED_CTLS);
        PRINT_VMXCAP_MSR(IA32_VMX_EXIT_CTLS);
        PRINT_VMXCAP_MSR(IA32_VMX_ENTRY_CTLS);
        PRINT_VMXCAP_MSR(IA32_VMX_MSR_MISC);
        PRINT_VMXCAP_MSR(IA32_VMX_CR0_FIXED0);
        PRINT_VMXCAP_MSR(IA32_VMX_CR0_FIXED1);
        PRINT_VMXCAP_MSR(IA32_VMX_CR4_FIXED0);
        PRINT_VMXCAP_MSR(IA32_VMX_CR4_FIXED1);
        PRINT_VMXCAP_MSR(IA32_VMX_VMCS_ENUM);

        has_secondary_ctls = !!(__rdmsr(IA32_VMX_PROCBASED_CTLS)
                                & (VMX_PROC_CTL_ACTIVATE_SECONDARY_CTLS << 32));

        if (has_secondary_ctls) {
                PRINT_VMXCAP_MSR(IA32_VMX_PROCBASED_CTLS2);
                has_ept = !!(__rdmsr(IA32_VMX_PROCBASED_CTLS2)
                             & (VMX_PROC_CTL2_ENABLE_EPT << 32));
                if (has_ept)
                        PRINT_VMXCAP_MSR(IA32_VMX_EPT_VPID_CAP);
        }

        if (has_true_ctls) {
                PRINT_VMXCAP_MSR(IA32_VMX_TRUE_PINBASED_CTLS);
                PRINT_VMXCAP_MSR(IA32_VMX_TRUE_PROCBASED_CTLS);
                PRINT_VMXCAP_MSR(IA32_VMX_TRUE_EXIT_CTLS);
                PRINT_VMXCAP_MSR(IA32_VMX_TRUE_ENTRY_CTLS);
        }

        if (has_secondary_ctls) {
                has_vmfunc = !!(__rdmsr(IA32_VMX_PROCBASED_CTLS2)
                                & (VMX_PROC_CTL2_ENABLE_VMFUNC << 32));
                if (has_vmfunc)
                        PRINT_VMXCAP_MSR(IA32_VMX_VMFUNC);
        }
}

static void
cache_vmx_capabilities(vm_monitor_t *vmm)
{
        vmm->ia32_vmx_basic = __rdmsr(IA32_VMX_BASIC);
        vmm->vmcs_revision_id = vmm->ia32_vmx_basic & 0x8fffffff;
        vmm->has_true_ctls = !!(vmm->ia32_vmx_basic & __BIT(55));

        vmm->ia32_vmx_pinbased_ctls = __rdmsr(IA32_VMX_PINBASED_CTLS);
        vmm->ia32_vmx_procbased_ctls = __rdmsr(IA32_VMX_PROCBASED_CTLS);
        vmm->ia32_vmx_exit_ctls = __rdmsr(IA32_VMX_EXIT_CTLS);
        vmm->ia32_vmx_entry_ctls = __rdmsr(IA32_VMX_ENTRY_CTLS);

        if (vmm->has_true_ctls) {
                vmm->ia32_vmx_true_pinbased_ctls =
                        __rdmsr(IA32_VMX_TRUE_PINBASED_CTLS);
                vmm->ia32_vmx_true_procbased_ctls =
                        __rdmsr(IA32_VMX_PROCBASED_CTLS);
                vmm->ia32_vmx_true_exit_ctls = __rdmsr(IA32_VMX_TRUE_EXIT_CTLS);
                vmm->ia32_vmx_true_entry_ctls =
                        __rdmsr(IA32_VMX_TRUE_ENTRY_CTLS);
        }

        vmm->pinbased_allowed0 = 0xffffffff &
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_pinbased_ctls
                                    : vmm->ia32_vmx_pinbased_ctls);
        vmm->pinbased_allowed1 =
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_pinbased_ctls
                                    : vmm->ia32_vmx_pinbased_ctls) >> 32;

        vmm->procbased_allowed0 = 0xffffffff &
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_procbased_ctls
                                    : vmm->ia32_vmx_procbased_ctls);
        vmm->procbased_allowed1 =
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_procbased_ctls
                                    : vmm->ia32_vmx_procbased_ctls) >> 32;

        vmm->exit_ctls_allowed0 = 0xffffffff &
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_exit_ctls
                                    : vmm->ia32_vmx_exit_ctls);
        vmm->exit_ctls_allowed1 =
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_exit_ctls
                                    : vmm->ia32_vmx_exit_ctls) >> 32;

        vmm->entry_ctls_allowed0 = 0xffffffff &
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_entry_ctls
                                    : vmm->ia32_vmx_entry_ctls);
        vmm->entry_ctls_allowed1 =
                (vmm->has_true_ctls ? vmm->ia32_vmx_true_entry_ctls
                                    : vmm->ia32_vmx_entry_ctls) >> 32;
}

static inline void
handle_early_exit(void)
{
        vmlatency_printk("VM instruciton error: %#lx\n",
                         __vmread(VMCS_VM_INSTRUCTION_ERROR));
}

static inline int
handle_vmexit(void)
{
        u32 exit_reason = (u32)__vmread(VMCS_EXIT_REASON);
        int basic_exit_reason = exit_reason & 0xffff;
        if (basic_exit_reason != VMEXIT_CPUID) {
                vmlatency_printk("Error: VM exit is not caused by CPUID."
                                 " Basic exit reason %d\n", basic_exit_reason);
                return -1;
        }
        return 0;
}

void
measure_vmlatency()
{
        vm_monitor_t vmm = {0};
        int cnt;  /* error counter for memory allocation */
        int i, n;  /* loop counters */
        irq_flags_t irq_flags;
        u64 start, end;
        host_state_t  hs;

        cache_vmx_capabilities(&vmm);

        cnt = allocate_memory(&vmm);
        if (cnt <= 0)
                goto out1;

        vmxon_setup_revision_id(&vmm);
        vmcs_setup_revision_id(&vmm);

        /* Disable interrupts */
        vmlatency_preempt_disable(&irq_flags);

        if (do_vmxon(&vmm) != 0)
                goto out2;

        if (do_vmptrld(&vmm) != 0)
                goto out3;

        initialize_vmcs(&vmm);
        save_host_state(&hs);

        if (do_vmlaunch() != 0) {
                vmlatency_printk("VMLAUNCH failed\n");
                handle_early_exit();
                goto out4;
        }

        handle_vmexit();

        for (n = 1; n < __BIT(20); n *= 2) {
                start = __get_tsc();
                for (i = 0; i < n; i++) {
                        do_vmresume();
                }
                end = __get_tsc();
                vmlatency_printk("%6d - %lld\n", n, (end - start) / n);
        }

out4:
        restore_host_state(&hs);
        do_vmclear(&vmm);
out3:
        do_vmxoff(&vmm);
out2:
        /* Enable interrupts */
        vmlatency_preempt_enable(&irq_flags);
out1:
        free_memory(&vmm, cnt);
}
