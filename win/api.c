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
#include <stdarg.h>
#include <string.h>

#include "api.h"

int
vmlatency_printm(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vDbgPrintExWithPrefix("[vmlatency] ", DPFLTR_IHVDRIVER_ID,
                          DPFLTR_INFO_LEVEL, fmt, va);
    va_end(va);
    return 0;
}
