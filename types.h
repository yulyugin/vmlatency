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

#ifndef __TYPES_H__
#define __TYPES_H__

#include <linux/types.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
typedef __SIZE_TYPE__ uintptr_t;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) && !defined(HAS_BOOL)
typedef _Bool bool;
#define true 1
#define false 0
#endif

#endif /* __TYPES_H__ */
