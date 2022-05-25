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

#include <linux/slab.h>
#include <linux/types.h>
#include <linux/highmem.h>
#include <asm/io.h>

#include "api.h"

int
allocate_vmpage(vmpage_t *p)
{
        p->page = alloc_page(GFP_KERNEL | __GFP_ZERO);
        if (!p->page)
                return -1;

        p->p = kmap(p->page);
        p->pa = page_to_phys(p->page);
        return 0;
}

void
free_vmpage(vmpage_t *p)
{
        kunmap(p->page);
        __free_page(p->page);
        p->page = NULL;
        p->p = NULL;
        p->pa = 0;
}

void
vmlatency_printm(const char *fmt, ...)
{
        va_list va;
        va_start(va, fmt);
        vprintk(fmt, va);
        va_end(va);
}

void
vmlatency_preempt_disable(unsigned long *irq_flags)
{
        preempt_disable();
        local_irq_save(*irq_flags);
}

void
vmlatency_preempt_enable(unsigned long *irq_flags)
{
        local_irq_restore(*irq_flags);
        preempt_enable();
}
