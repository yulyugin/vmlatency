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

#include <linux/module.h>

#include "vmx.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Evgeny Yulyugin <yulyugin@gmail.com>");
MODULE_DESCRIPTION("vmlatency");

static int __init
vmlatency_init(void)
{
        if (!vmx_enabled())
                return 0;

        print_vmx_info();

        measure_vmlatency();
        return 0;
}

static void __exit
vmlatency_exit(void)
{
}

module_init(vmlatency_init);
module_exit(vmlatency_exit);
