/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cross-platform user-space device driver layer Linux kernel implementation.
 *
 * Copyright (C) 2022 Jeff Webb <jeff.webb@codecraftsmen.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Linux kernel module implementation.
 *
 * This code implements both Linux UIO and Xenomai UDD functionality (based
 * on the ``CUDDLK_USE_UDD`` *#define*).
 */

#include <linux/module.h>
#include <linux/slab.h> /* kzalloc */
#include <cuddl/kernel.h>

int cuddlk_register_device(struct cuddlk_device *dev)
{
	return -1;
}
EXPORT_SYMBOL_GPL(cuddlk_register_device);

static int __init cuddl_init(void)
{
	int ret;
	struct cuddlk_device *dev;

	dev = kzalloc(sizeof(struct cuddlk_device), GFP_KERNEL);
	ret = cuddlk_register_device(dev);
	kfree(dev);
	return ret;
}

static void __exit cuddl_exit(void)
{
}

module_init(cuddl_init)
module_exit(cuddl_exit)
MODULE_LICENSE("GPL");
