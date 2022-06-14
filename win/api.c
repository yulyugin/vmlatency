/*
 * Copyright (c) 2018 Evgenii Iuliugin
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

#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdarg.h>
#include <string.h>

#include "api.h"
#include "messages.h"

#define MAX_MSG_SZ     (ERROR_LOG_MAXIMUM_SIZE - sizeof(IO_ERROR_LOG_PACKET))
#define MAX_MSG_CH     (MAX_MSG_SZ / sizeof(WCHAR))

#ifndef _M_IX86
extern void _disable(void);
#endif

void
vmlatency_printm(const char *fmt, ...)
{
        va_list va;
        va_start(va, fmt);
        vDbgPrintExWithPrefix("", DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
                              fmt, va);

        if (KeGetCurrentIrql() == PASSIVE_LEVEL) {
                extern PDEVICE_OBJECT g_Device;
                PIO_ERROR_LOG_PACKET entry = IoAllocateErrorLogEntry(
                        g_Device, ERROR_LOG_MAXIMUM_SIZE);
                char msg[MAX_MSG_CH];

                if (!entry)
                        return;

                entry->ErrorCode = VMLATENCY_LOG;
                entry->NumberOfStrings = 1;
                entry->StringOffset = FIELD_OFFSET(IO_ERROR_LOG_PACKET,
                                                   DumpData);
                RtlStringCbVPrintfA(msg, sizeof(msg), fmt, va);

                RtlUTF8ToUnicodeN((PWSTR)entry->DumpData, MAX_MSG_SZ, NULL,
                                  msg, sizeof(msg));
                IoWriteErrorLogEntry(entry);
        }

        va_end(va);
}

void
vmlatency_preempt_disable(irq_flags_t *irq_flags)
{
        irq_flags->irql = KeRaiseIrqlToDpcLevel();
        irq_flags->eflags = __readeflags();
        _disable();
}

void
vmlatency_preempt_enable(irq_flags_t *irq_flags)
{
        __writeeflags(irq_flags->eflags);
        KeLowerIrql(irq_flags->irql);
}

int
allocate_vmpage(vmpage_t *p)
{
        PHYSICAL_ADDRESS Low, High, Skip;
        Low.QuadPart = 0;
        High.QuadPart = ~0ULL;
        Skip.QuadPart = 0;
        p->mdl = MmAllocatePagesForMdlEx(Low, High, Skip, PAGE_SIZE,
                                         MmCached, 0);
        if (!p->mdl)
                return -1;

        p->p = MmMapLockedPagesSpecifyCache(p->mdl, KernelMode, MmCached, NULL,
                                            FALSE, NormalPagePriority);
        if (!p->p) {
                MmFreePagesFromMdl(p->mdl);
                return -1;
        }

        p->pa = MmGetMdlPfnArray(p->mdl)[0] << PAGE_SHIFT;
        return 0;
}

void
free_vmpage(vmpage_t *p)
{
        MmUnmapLockedPages(p->p, p->mdl);
        MmFreePagesFromMdl(p->mdl);
        p->mdl = NULL;
        p->p = NULL;
        p->pa = 0;
}
