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

#ifdef CONFIG_STACK_VALIDATION
#include <asm/unwind_hints.h>
#endif

#include "cpu-defs.h"

.text

.macro vmentry_prepare
        pushf  /* save rflags on stack */

        mov $VMCS_HOST_RSP, %r11  /* save stack pointer */
        vmwrite %rsp, %r11
.endm

.globl do_vmlaunch
do_vmlaunch:
        vmentry_prepare
        vmlaunch
        jmp entry_error

.globl do_vmresume
do_vmresume:
        vmentry_prepare
        vmresume
        /* fall through */

entry_error:
        mov $1, %rax
        jmp vmx_return

.globl vmx_exit
vmx_exit:
        xor %rax, %rax  /* return 0 */

vmx_return:
#ifdef CONFIG_STACK_VALIDATION
        UNWIND_HINT_FUNC
#endif
        popf  /* restore rflags */
        ret
.type do_vmlaunch @function
.type do_vmresume @function
.type vmx_exit @function
