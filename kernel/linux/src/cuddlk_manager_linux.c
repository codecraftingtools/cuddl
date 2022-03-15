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
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <cuddl/kernel.h>
#include <cuddl/common_impl_linux_ioctl.h>

/* Export symbols from cuddlk_manager_common.c */
EXPORT_SYMBOL_GPL(cuddlk_manager_find_device_matching);
EXPORT_SYMBOL_GPL(cuddlk_manager_find_device);
EXPORT_SYMBOL_GPL(cuddlk_manager_find_empty_slot);
EXPORT_SYMBOL_GPL(cuddlk_next_available_instance_id_for);
EXPORT_SYMBOL_GPL(cuddlk_manager_add_device);
EXPORT_SYMBOL_GPL(cuddlk_manager_remove_device);
EXPORT_SYMBOL_GPL(cuddlk_device_manage);
EXPORT_SYMBOL_GPL(cuddlk_device_release);

static dev_t cuddlk_manager_dev;
static struct cdev cuddlk_manager_cdev;
static struct class *cuddlk_manager_class;
static struct device *cuddlk_manager_device;

static long cuddlk_manager_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	struct cuddl_memregion_claim_ioctl_data data;

	printk("cuddlk_manager_ioctl\n");
	printk("  cmd:  %u\n", cmd);
	printk("  dir:  %u\n", _IOC_DIR(cmd));
	printk("  type: %u\n", _IOC_TYPE(cmd));
	printk("  nr:   %u\n", _IOC_NR(cmd));
	printk("  size: %u\n", _IOC_SIZE(cmd));
	printk("  arg:  %lu\n", arg);

	switch(cmd) {
	case CUDDL_MEMREGION_CLAIM_IOCTL:
		printk("CUDDL_MEMREGION_CLAIM_IOCTL called\n");
		if (copy_from_user(&data, (void*)arg, sizeof(data))) {
			printk("copy_from_user failed\n");
			break;
		}
		printk("  %s %s %s %d\n", data.id.group, data.id.device,
		       data.id.resource, data.id.instance);
		data.info.pa_len = 1;
		data.info.start_offset = 2;
		data.info.len = 1;
		data.info.flags = 3;
		if (copy_to_user((void*)arg, &data, sizeof(data))) {
			printk("copy_to_user failed\n");
			break;
		}
		printk("  success\n");
		break;
	default:
		printk("Unknown Cuddl IOCTL\n");
	}
	
	return 0;
}

const struct file_operations cuddlk_manager_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = cuddlk_manager_ioctl,
};

enum cuddlk_manager_init_failure {
	CUDDLK_MGR_FAIL_ALLOC_MANAGER,
	CUDDLK_MGR_FAIL_ALLOC_CHRDEV,
	CUDDLK_MGR_FAIL_CDEV_ADD,
	CUDDLK_MGR_FAIL_CLASS_CREATE,
	CUDDLK_MGR_FAIL_DEVICE_CREATE,
	CUDDLK_MGR_NO_FAILURE,
};

static int cuddlk_manager_cleanup(enum cuddlk_manager_init_failure failure)
{
	int ret = 0;

	switch(failure) {
	case CUDDLK_MGR_NO_FAILURE:
		device_destroy(cuddlk_manager_class, cuddlk_manager_dev);
		/* FALLTHROUGH */
	case CUDDLK_MGR_FAIL_DEVICE_CREATE:
		class_destroy(cuddlk_manager_class);
		/* FALLTHROUGH */
	case CUDDLK_MGR_FAIL_CLASS_CREATE:
		cdev_del(&cuddlk_manager_cdev);
		/* FALLTHROUGH */
	case CUDDLK_MGR_FAIL_CDEV_ADD:
		unregister_chrdev_region(cuddlk_manager_dev, 1);
		/* FALLTHROUGH */
	case CUDDLK_MGR_FAIL_ALLOC_CHRDEV:
		kfree(cuddlk_global_manager_ptr);
		/* FALLTHROUGH */
	case CUDDLK_MGR_FAIL_ALLOC_MANAGER:
		/* FALLTHROUGH */
	default:
		break;
	}

	cuddlk_global_manager_ptr = NULL;
	
	return ret;
}

static int __init cuddlk_manager_init(void)
{
	int ret;
	enum cuddlk_manager_init_failure failure;
	
	cuddlk_global_manager_ptr = kzalloc(
		sizeof(struct cuddlk_manager), GFP_KERNEL);
	if (!cuddlk_global_manager_ptr) {
		ret = -ENOMEM;
		failure = CUDDLK_MGR_FAIL_ALLOC_MANAGER;
		printk("%s: kzalloc failed: %d\n", __func__, ret);
		goto handle_failure;
	}

	ret = alloc_chrdev_region(&cuddlk_manager_dev, 0, 1, "cuddl");
	if (ret < 0) {
		failure = CUDDLK_MGR_FAIL_ALLOC_CHRDEV;
		goto handle_failure;
	}

	cdev_init(&cuddlk_manager_cdev, &cuddlk_manager_fops);
	ret = cdev_add(&cuddlk_manager_cdev, cuddlk_manager_dev, 1);
	if (ret < 0) {
		failure = CUDDLK_MGR_FAIL_CDEV_ADD;
		goto handle_failure;
	}

	/* Create sysfs class node */
	cuddlk_manager_class = class_create(THIS_MODULE, "cuddl");
	if (IS_ERR(cuddlk_manager_class)) {
		ret = -ENODEV;
		failure = CUDDLK_MGR_FAIL_CLASS_CREATE;
		goto handle_failure;
	}

	/* Create sysfs device node */
	cuddlk_manager_device = device_create(
		cuddlk_manager_class, NULL, cuddlk_manager_dev, NULL,
		"cuddl");
	if (IS_ERR(cuddlk_manager_device)) {
		ret = -ENODEV;
		failure = CUDDLK_MGR_FAIL_DEVICE_CREATE;
		goto handle_failure;
	}

	return 0;

handle_failure:
	cuddlk_manager_cleanup(failure);
	return ret;
}

static void __exit cuddlk_manager_exit(void)
{
	cuddlk_manager_cleanup(CUDDLK_MGR_NO_FAILURE);
}

module_init(cuddlk_manager_init)
module_exit(cuddlk_manager_exit)
MODULE_LICENSE("GPL");
