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

#include "cpu-defs.h"

.text

.macro vmentry_prepare
        // save stack pointer
        mov     $(VMCS_HOST_RSP), %rax
        vmwrite %rsp, %rax
.endm

.globl _do_vmlaunch
_do_vmlaunch:
        vmentry_prepare
        vmlaunch
        jmp entry_error

.globl _do_vmresume
_do_vmresume:
        vmentry_prepare
        vmresume
        /* fall through */

entry_error:
        mov $1, %rax  /* return 1 */
        jmp vmx_return

.globl _vmx_exit
_vmx_exit:
        xor %rax, %rax  /* return 0 */

vmx_return:
        ret
