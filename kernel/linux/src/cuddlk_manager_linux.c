/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cross-platform user-space device driver layer Linux kernel manager impl.
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
 * Linux kernel module implementation for device management.
 *
 * This code implements both Linux UIO and Xenomai UDD functionality (based
 * on the ``CUDDLK_USE_UDD`` *#define*).
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <cuddl/kernel.h>

static struct cuddlk_manager *manager;

int cuddlk_manager_add(struct cuddlk_device *dev)
{
	printk("Cuddl managing %s %s %d\n",
	       dev->group, dev->name, dev->instance);

	return 0;
}
EXPORT_SYMBOL_GPL(cuddlk_manager_add);

int cuddlk_manager_remove(struct cuddlk_device *dev)
{
	printk("Cuddl releasing %s %s %d\n",
	       dev->group, dev->name, dev->instance);

	return 0;
}
EXPORT_SYMBOL_GPL(cuddlk_manager_remove);

int cuddlk_manage_device(struct cuddlk_device *dev)
{
	int ret;

	ret = cuddlk_register_device(dev);
	if (ret) {
		goto fail_register;
	}

	ret = cuddlk_manager_add(dev);
	if (ret) {
		goto fail_manager_add;
	}

	return 0;

fail_manager_add:
	cuddlk_unregister_device(dev);
fail_register:
	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_manage_device);

int cuddlk_release_device(struct cuddlk_device *dev)
{
	int ret;

	ret = cuddlk_manager_remove(dev);
	ret = cuddlk_unregister_device(dev);
	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_release_device);

static int __init cuddlk_manager_init(void)
{
	int err;

	manager = kzalloc(sizeof(struct cuddlk_manager), GFP_KERNEL);
	if (!manager) {
		err = -ENOMEM;
		printk("%s: kzalloc failed: %d\n", __func__, err);
		goto fail_kzalloc;
	}
	return 0;

fail_kzalloc:
	manager = NULL;
	return err;
}

static void __exit cuddlk_manager_exit(void)
{
	kfree(manager);
}

module_init(cuddlk_manager_init)
module_exit(cuddlk_manager_exit)
MODULE_LICENSE("GPL");
