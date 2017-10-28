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
#include <linux/slab.h>

#include "vmx.h"
#include "asm-inlines.h"
#include "cpu-defs.h"

#define REGISTER_PAGE(name)                                                 \
static inline int                                                           \
allocate_##name(vm_monitor_t *vmm)                                          \
{                                                                           \
        if (((vmm)->name = kmalloc(0x1000, GFP_KERNEL)) == NULL) return -1; \
        memset((vmm)->name, 0, 0x1000);                                     \
        (vmm)->name##_pa = virt_to_phys((vmm)->name);                       \
        return 0;                                                           \
}                                                                           \
                                                                            \
static inline void                                                          \
free_##name(vm_monitor_t *vmm)                                              \
{                                                                           \
        kfree(vmm->name);                                                   \
        vmm->name = NULL;                                                   \
        vmm->name##_pa = 0;                                                 \
}

REGISTER_PAGE(vmxon_region)
REGISTER_PAGE(io_bitmap_a)
REGISTER_PAGE(io_bitmap_b)
REGISTER_PAGE(vmcs)

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

static inline int
allocate_memory(vm_monitor_t *vmm)
{
        int cnt = 0;
        if (allocate_vmxon_region(vmm) == 0) cnt++; else return -cnt;
        if (allocate_vmcs(vmm) == 0) cnt++; else return -cnt;
        if (allocate_io_bitmap_a(vmm) == 0) cnt++; else return -cnt;
        if (allocate_io_bitmap_b(vmm) == 0) cnt++; else return -cnt;

        return cnt;
}

static inline void
free_memory(vm_monitor_t *vmm, int cnt)
{
        if (cnt == 4) { free_io_bitmap_b(vmm); cnt--; }
        if (cnt == 3) { free_io_bitmap_a(vmm); cnt--; }
        if (cnt == 2) { free_vmxon_region(vmm); cnt--; }
        if (cnt == 1) { free_vmcs(vmm); cnt--; }
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

        vmlatency_printk("VMX is supported by CPU\n");
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
        ((u32 *)vmm->vmxon_region)[0] = get_vmcs_revision_identifier();
}

static inline void
vmcs_setup_revision_id(vm_monitor_t *vmm)
{
        ((u32 *)vmm->vmcs)[0] = get_vmcs_revision_identifier();
}

static inline int
do_vmxon(vm_monitor_t *vmm)
{
        u64 old_cr4 = __get_cr4();
        vmm->old_vmxe = old_cr4 & CR4_VMXE;

        /* Set CR4.VMXE if necessary */
        if (!vmm->old_vmxe)
                __set_cr4(old_cr4 | CR4_VMXE);

        if (__vmxon(vmm->vmxon_region_pa) != 0) {
                vmlatency_printk("VMXON failed\n");
                return -1;
        }

        vmlatency_printk("VMXON succeeded\n");
        return 0;
}

static inline void
do_vmxoff(vm_monitor_t *vmm)
{
        if (__vmxoff() != 0)
                vmlatency_printk("VMXOFF failed\n");

        /* Clear CR4.VMXE if necessary */
        if (!vmm->old_vmxe)
                __set_cr4(__get_cr4() & ~CR4_VMXE);

        vmlatency_printk("VMXOFF succeeded\n");
}

static inline int
do_vmptrld(vm_monitor_t *vmm)
{
        if (__vmptrld(vmm->vmcs_pa) != 0){
                vmlatency_printk("VMPTRLD failed\n");
                return -1;
        }

        return 0;
}

static inline int
do_vmclear(vm_monitor_t *vmm)
{
        if (__vmclear(vmm->vmcs_pa) != 0){
                vmlatency_printk("VMCLEAR failed\n");
                return -1;
        }

        return 0;
}

/* Initialize guest state to match host state */
static inline void
initialize_vmcs(vm_monitor_t *vmm)
{
        u16 val16;
        u64 ia32_sysenter_cs;

        /* 16-bit guest/host state */
        __asm__ __volatile__("movw %%es, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_ES, val16);
        __vmwrite(VMCS_HOST_ES, val16);

        __asm__ __volatile__("movw %%cs, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_CS, val16);
        __vmwrite(VMCS_HOST_CS, val16);

        __asm__ __volatile__("movw %%ss, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_SS, val16);
        __vmwrite(VMCS_HOST_SS, val16);

        __asm__ __volatile__("movw %%ds, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_DS, val16);
        __vmwrite(VMCS_HOST_DS, val16);

        __asm__ __volatile__("movw %%fs, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_FS, val16);
        __vmwrite(VMCS_HOST_FS, val16);

        __asm__ __volatile__("movw %%gs, %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_GS, val16);
        __vmwrite(VMCS_HOST_GS, val16);

        __asm__ __volatile__("str %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_TR, val16);
        __vmwrite(VMCS_HOST_TR, val16);

        __asm__ __volatile__("sldt %0" :"=r"(val16));
        __vmwrite(VMCS_GUEST_LDTR, val16);

        /* 64-bit control fields */
        __vmwrite(VMCS_IO_BITMAP_A_ADDR, vmm->io_bitmap_a_pa);
        __vmwrite(VMCS_IO_BITMAP_B_ADDR, vmm->io_bitmap_b_pa);
        __vmwrite(VMCS_EXEC_VMCS_PTR, 0);
        __vmwrite(VMCS_TSC_OFFSET, 0);

        /* 64-bit guest state */
        __vmwrite(VMCS_VMCS_LINK_PTR, 0xffffffffffffffffull);
        __vmwrite(VMCS_GUEST_IA32_DEBUGCTL, 0);

        /* 32-bit control fields */
        //__vmwrite(VMCS_PIN_BASED_VM_CTRL, );
        //__vmwrite(VMCS_PROC_BASED_VM_CTRL, );
        __vmwrite(VMCS_EXCEPTION_BITMAP, 0xffffffff);
        __vmwrite(VMCS_PF_ECODE_MASK, 0);
        __vmwrite(VMCS_PF_ECODE_MATCH, 0);
        __vmwrite(VMCS_CR3_TARGET_CNT, 0);
        //__vmwrite(VMCS_VMEXIT_CRTL, );
        __vmwrite(VMCS_VMEXIT_MSR_STORE_CNT, 0);
        __vmwrite(VMCS_VMEXIT_MSR_LOAD_CNT, 0);
        //__vmwrite(VMCS_VMENTRY_CTRL, );
        __vmwrite(VMCS_VMENTRY_MSR_LOAD_CNT, 0);
        __vmwrite(VMCS_VMENTRY_INT_INFO, 0);
        __vmwrite(VMCS_VMENTRY_ECODE, 0);
        __vmwrite(VMCS_VMENTRY_INSTR_LEN, 0);

        /* 32-bit guest state*/
        __vmwrite(VMCS_GUEST_ES_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_CS_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_CS_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_SS_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_SS_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_DS_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_DS_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_FS_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_FS_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_GS_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_GS_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_LDTR_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_TR_LIMIT, 0xffffffff);
        //__vmwrite(VMCS_GUEST_TR_ACCESS_RIGHTS, );
        __vmwrite(VMCS_GUEST_GDTR_LIMIT, 0xffffffff);
        __vmwrite(VMCS_GUEST_IDTR_LIMIT, 0xffffffff);

        ia32_sysenter_cs = __rdmsr(IA32_SYSENTER_CS);
        __vmwrite(VMCS_GUEST_IA32_SYSENTER_CS, ia32_sysenter_cs);
        __vmwrite(VMCS_HOST_IA32_SYSENTER_CS, ia32_sysenter_cs);

        /* Natural-width control fields */
        __vmwrite(VMCS_CR0_GUEST_HOST_MASK, 0);
        __vmwrite(VMCS_CR0_READ_SHADOW, 0);
        __vmwrite(VMCS_CR4_GUEST_HOST_MASK, 0);
        __vmwrite(VMCS_CR4_READ_SHADOW, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_0, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_1, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_2, 0);
        __vmwrite(VMCS_CR3_TARGET_VALUE_3, 0);
}

void
print_vmx_info()
{
        vmlatency_printk("VMCS revision identifier: %#lx\n",
                         get_vmcs_revision_identifier());
}

void
measure_vmlatency()
{
        vm_monitor_t vmm = {0};

        int cnt = allocate_memory(&vmm);
        if (cnt <= 0)
                goto out1;

        vmxon_setup_revision_id(&vmm);
        vmcs_setup_revision_id(&vmm);

        /* Disable interrupts */
        local_irq_disable();

        if (do_vmxon(&vmm) != 0)
                goto out2;

        if (do_vmptrld(&vmm) != 0)
                goto out3;

        initialize_vmcs(&vmm);

        __vmwrite(VMCS_HOST_RIP, (u64)&&handle_vmexit);
        __vmwrite(VMCS_GUEST_RIP, (u64)&&guest_code);

        if (__vmlaunch() != 0) {
                vmlatency_printk("VMLAUNCH failed\n");
                goto out4;
        }

guest_code:
        __asm__ __volatile__("cpuid"); // Will cause VM exit

handle_vmexit:
        vmlatency_printk("VM exit handled\n");

out4:
        do_vmclear(&vmm);
out3:
        do_vmxoff(&vmm);
out2:
        /* Enable interrupts */
        local_irq_enable();
out1:
        free_memory(&vmm, cnt);
}
