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
allocate_vmxon_region(vm_monitor_t *vmm)
{
        vmm->vmxon_region = kmalloc(0x1000, GFP_KERNEL);
        if (!vmm->vmxon_region) {
                vmlatency_printk("Can't allocate vmxon region\n");
                return -1;
        }

        vmm->vmxon_region_pa = virt_to_phys(vmm->vmxon_region);
        return 0;
}

static inline void
free_vmxon_region(vm_monitor_t *vmm)
{
        kfree(vmm->vmxon_region);
        vmm->vmxon_region = NULL;
        vmm->vmxon_region_pa = 0;
}

static inline int
allocate_vmcs(vm_monitor_t *vmm)
{
        vmm->vmcs = kmalloc(0x1000, GFP_KERNEL);
        if (!vmm->vmcs) {
                vmlatency_printk("Can't allocate vmcs region\n");
                return -1;
        }

        vmm->vmcs_pa = virt_to_phys(vmm->vmcs);
        return 0;
}

static inline void
free_vmcs(vm_monitor_t *vmm)
{
        kfree(vmm->vmcs);
        vmm->vmcs = NULL;
        vmm->vmcs_pa = 0;
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

        if (allocate_vmxon_region(&vmm) != 0)
                return;
        vmxon_setup_revision_id(&vmm);

        if (allocate_vmcs(&vmm) != 0)
                goto out1;
        vmcs_setup_revision_id(&vmm);

        /* Disable interrupts */
        local_irq_disable();

        if (do_vmxon(&vmm) != 0)
                goto out2;

        if (do_vmptrld(&vmm) != 0)
                goto out3;

        do_vmclear(&vmm);

out3:
        do_vmxoff(&vmm);

out2:
        /* Enable interrupts */
        local_irq_enable();

        free_vmcs(&vmm);
out1:
        free_vmxon_region(&vmm);
}
