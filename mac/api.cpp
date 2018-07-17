/*
 * Copyright (c) 2018 Evgeny Yulyugin
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

#include <sys/systm.h>
#include <IOKit/IOBufferMemoryDescriptor.h>

#include "api.h"

int
allocate_vmpage(vmpage_t *p)
{
        IOOptionBits options = kIODirectionInOut | kIOMemoryMapperNone
                             | kIOMemoryPhysicallyContiguous;
        p->page = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,
                                                              options, 0x1000,
                                                              0x1000);

        if (!p->page)
                return -1;

        IOReturn IORet = p->page->prepare();
        if (IORet != kIOReturnSuccess) {
                p->page->release();
                return -1;
        }

        p->p = (char *)p->page->getBytesNoCopy();
        memset(p->p, 0, 0x1000);
        p->pa = p->page->getPhysicalAddress();
        return 0;
}

void
free_vmpage(vmpage_t *p)
{
        p->page->complete();
        p->page->release();
        p->page = NULL;
        p->p = NULL;
        p->pa = 0;
}

int
vmlatency_printm(const char *fmt, ...)
{
        int ret;
        va_list va;
        va_start(va, fmt);
        ret = vprintf(fmt, va);
        va_end(va);
        return ret;
}

void
vmlatency_preempt_disable(irq_flags_t *irq_flags)
{
}

void
vmlatency_preempt_enable(irq_flags_t irq_flags)
{
}
