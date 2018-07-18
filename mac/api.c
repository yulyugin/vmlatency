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

#include "api.h"

int
allocate_vmpage(vmpage_t *p)
{
        return 0;
}

void
free_vmpage(vmpage_t *p)
{
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
