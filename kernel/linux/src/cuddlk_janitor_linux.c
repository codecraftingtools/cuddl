/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cross-platform user-space device driver layer Linux kernel janitor impl.
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
 * Linux kernel module implementation for device cleanup.
 *
 * This code implements both Linux UIO and Xenomai UDD functionality (based
 * on the ``CUDDLK_USE_UDD`` *#define*).
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <cuddlk.h>
#include <cuddl/common_impl_linux_ioctl.h>

static dev_t cuddlk_janitor_dev;
static struct cdev cuddlk_janitor_cdev;
static struct class *cuddlk_janitor_class;
static struct device *cuddlk_janitor_device;

/* From cuddlk_manager_linux.c */
extern void cuddlk_manager_free_refs_for_pid(pid_t pid);

static int cuddlk_janitor_open(struct inode *inode, struct file *file)
{
	cuddlk_debug("janitor open\n");
	return 0;
}

static long cuddlk_janitor_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	pid_t pid;

	cuddlk_debug("cuddlk_janitor_ioctl\n");
	cuddlk_debug("  cmd:  %u\n", cmd);
	cuddlk_debug("  dir:  %u\n", _IOC_DIR(cmd));
	cuddlk_debug("  type: %u\n", _IOC_TYPE(cmd));
	cuddlk_debug("  nr:   %u\n", _IOC_NR(cmd));
	cuddlk_debug("  size: %u\n", _IOC_SIZE(cmd));
	cuddlk_debug("  arg:  %lu\n", arg);

	switch(cmd) {
	case CUDDL_JANITOR_REGISTER_PID_IOCTL:
		cuddlk_debug("CUDDL_JANITOR_REGISTER_PID_IOCTL called\n");
		if (copy_from_user(
			    &pid, (void*)arg, sizeof(pid))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		cuddlk_debug("  pid: %d\n", pid);

		file->private_data = kzalloc(sizeof(pid), GFP_KERNEL);
		if (!file->private_data) {
			cuddlk_print("kzalloc for pid failed\n");
			ret = -ENOMEM;
			break;
		}
		*((pid_t*) file->private_data) = pid;
		cuddlk_debug("  success\n");
		break;

	default:
		cuddlk_print("Unknown Cuddl janitor IOCTL\n");
		ret = -ENOSYS;
	}

	return ret;
}

static int cuddlk_janitor_release(struct inode *inode, struct file *file)
{
	pid_t pid;

	cuddlk_debug("janitor close\n");

	if (file->private_data) {
		pid = *((pid_t *) file->private_data);
		cuddlk_manager_free_refs_for_pid(pid);
		kfree(file->private_data);
	}

	return 0;
}

const struct file_operations cuddlk_janitor_fops = {
    .owner = THIS_MODULE,
    .open = cuddlk_janitor_open,
    .unlocked_ioctl = cuddlk_janitor_ioctl,
    .release = cuddlk_janitor_release,
};

enum cuddlk_janitor_init_failure {
	CUDDLK_JANITOR_FAIL_ALLOC_CHRDEV,
	CUDDLK_JANITOR_FAIL_CDEV_ADD,
	CUDDLK_JANITOR_FAIL_CLASS_CREATE,
	CUDDLK_JANITOR_FAIL_DEVICE_CREATE,
	CUDDLK_JANITOR_NO_FAILURE,
};

static int cuddlk_janitor_cleanup(enum cuddlk_janitor_init_failure failure)
{
	int ret = 0;

	switch(failure) {
	case CUDDLK_JANITOR_NO_FAILURE:
		device_destroy(cuddlk_janitor_class, cuddlk_janitor_dev);
		/* FALLTHROUGH */
	case CUDDLK_JANITOR_FAIL_DEVICE_CREATE:
		class_destroy(cuddlk_janitor_class);
		/* FALLTHROUGH */
	case CUDDLK_JANITOR_FAIL_CLASS_CREATE:
		cdev_del(&cuddlk_janitor_cdev);
		/* FALLTHROUGH */
	case CUDDLK_JANITOR_FAIL_CDEV_ADD:
		unregister_chrdev_region(cuddlk_janitor_dev, 1);
		/* FALLTHROUGH */
	case CUDDLK_JANITOR_FAIL_ALLOC_CHRDEV:
		/* FALLTHROUGH */
	default:
		break;
	}

	return ret;
}

static int __init cuddlk_janitor_init(void)
{
	int ret;
	enum cuddlk_janitor_init_failure failure;
	
	ret = alloc_chrdev_region(&cuddlk_janitor_dev, 0, 1, "cuddl_janitor");
	if (ret < 0) {
		failure = CUDDLK_JANITOR_FAIL_ALLOC_CHRDEV;
		goto handle_failure;
	}

	cdev_init(&cuddlk_janitor_cdev, &cuddlk_janitor_fops);
	ret = cdev_add(&cuddlk_janitor_cdev, cuddlk_janitor_dev, 1);
	if (ret < 0) {
		failure = CUDDLK_JANITOR_FAIL_CDEV_ADD;
		goto handle_failure;
	}

	/* Create sysfs class node */
	cuddlk_janitor_class = class_create(THIS_MODULE, "cuddl_janitor");
	if (IS_ERR(cuddlk_janitor_class)) {
		ret = -ENODEV;
		failure = CUDDLK_JANITOR_FAIL_CLASS_CREATE;
		goto handle_failure;
	}

	/* Create sysfs device node */
	cuddlk_janitor_device = device_create(
		cuddlk_janitor_class, NULL, cuddlk_janitor_dev, NULL,
		"cuddl_janitor");
	if (IS_ERR(cuddlk_janitor_device)) {
		ret = -ENODEV;
		failure = CUDDLK_JANITOR_FAIL_DEVICE_CREATE;
		goto handle_failure;
	}

	return 0;

handle_failure:
	cuddlk_janitor_cleanup(failure);
	return ret;
}

static void __exit cuddlk_janitor_exit(void)
{
	cuddlk_janitor_cleanup(CUDDLK_JANITOR_NO_FAILURE);
}

module_init(cuddlk_janitor_init)
module_exit(cuddlk_janitor_exit)
MODULE_LICENSE("GPL");
