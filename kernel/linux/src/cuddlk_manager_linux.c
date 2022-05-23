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
static struct cuddlk_manager *cuddlk_global_manager_ptr;

struct cuddlk_manager *cuddlk_manager_get(void)
{
	return cuddlk_global_manager_ptr;
}
EXPORT_SYMBOL_GPL(cuddlk_manager_get);

static long cuddlk_manager_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	int slot;
	int mslot;
	int eslot;
	int rt = 0;
	int ret = 0;
	struct cuddlk_device *dev;
	struct cuddl_memregion_claim_ioctl_data *mdata;
	struct cuddl_eventsrc_claim_ioctl_data *edata;
	struct cuddl_memregion_release_ioctl_data *mrdata;
	struct cuddl_eventsrc_release_ioctl_data *erdata;
	struct cuddl_device_get_id_ioctl_data *get_id_data;

	cuddlk_debug("cuddlk_manager_ioctl\n");
	cuddlk_debug("  cmd:  %u\n", cmd);
	cuddlk_debug("  dir:  %u\n", _IOC_DIR(cmd));
	cuddlk_debug("  type: %u\n", _IOC_TYPE(cmd));
	cuddlk_debug("  nr:   %u\n", _IOC_NR(cmd));
	cuddlk_debug("  size: %u\n", _IOC_SIZE(cmd));
	cuddlk_debug("  arg:  %lu\n", arg);

	mdata = kzalloc(
		sizeof(struct cuddl_memregion_claim_ioctl_data),
		GFP_KERNEL);
	if (!mdata) {
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	edata = kzalloc(
		sizeof(struct cuddl_eventsrc_claim_ioctl_data),
		GFP_KERNEL);
	if (!edata) {
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	mrdata = kzalloc(
		sizeof(struct cuddl_memregion_release_ioctl_data),
		GFP_KERNEL);
	if (!mrdata) {
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	erdata = kzalloc(
		sizeof(struct cuddl_eventsrc_release_ioctl_data),
		GFP_KERNEL);
	if (!erdata) {
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	get_id_data = kzalloc(
		sizeof(struct cuddl_device_get_id_ioctl_data),
		GFP_KERNEL);
	if (!get_id_data) {
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	switch(cmd) {
	case CUDDL_MEMREGION_CLAIM_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDL_MEMREGION_CLAIM_UIO_IOCTL:
		cuddlk_debug("CUDDL_MEMREGION_CLAIM_*_IOCTL called\n");
		if (copy_from_user(mdata, (void*)arg, sizeof(*mdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}

		cuddlk_debug("  %s %s %s %d\n",
			     mdata->id.group, mdata->id.device,
			     mdata->id.resource, mdata->id.instance);
		slot = cuddlk_manager_find_device_matching(
			cuddlk_global_manager_ptr,
			mdata->id.group, mdata->id.device, mdata->id.resource,
			mdata->id.instance, CUDDLK_RESOURCE_MEMREGION, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		mslot = cuddlk_device_find_memregion(dev, mdata->id.resource);
		if (mslot < 0) {
			ret = mslot;
			break;
		}
		cuddlk_debug("  found mslot: %d\n", mslot);

		mdata->info.priv.token.device_index = slot;
		mdata->info.priv.token.resource_index = mslot;
		mdata->info.priv.pa_len = dev->mem[mslot].pa_len;
		mdata->info.priv.start_offset = dev->mem[mslot].start_offset;
		mdata->info.len = dev->mem[mslot].len;
		mdata->info.flags = 0;
		if (dev->mem[mslot].flags && CUDDLK_MEMF_SHARED)
			mdata->info.flags |= CUDDL_MEMF_SHARED;
		if (rt) {
			mdata->info.priv.pa_mmap_offset = 0;
			snprintf(mdata->info.priv.device_name,
				 CUDDL_MAX_STR_LEN,
				 "/dev/rtdm/%s,mapper%d",
				 dev->priv.unique_name,
				 mslot);
		} else {
			mdata->info.priv.pa_mmap_offset = \
				mslot * CUDDLK_PAGE_SIZE;
			snprintf(mdata->info.priv.device_name,
				 CUDDL_MAX_STR_LEN,
				 "/dev/uio%d",
				 dev->priv.uio.uio_dev->minor);
		}
		if (copy_to_user((void*)arg, mdata, sizeof(*mdata))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -1;
			break;
		}
		dev->mem[mslot].kernel.ref_count += 1;
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->mem[mslot].kernel.ref_count);
		break;

	case CUDDL_EVENTSRC_CLAIM_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDL_EVENTSRC_CLAIM_UIO_IOCTL:
		cuddlk_debug("CUDDL_EVENTSRC_CLAIM_*_IOCTL called\n");
		if (copy_from_user(edata, (void*)arg, sizeof(*edata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}

		cuddlk_debug("  %s %s %s %d\n",
			     edata->id.group, edata->id.device,
			     edata->id.resource, edata->id.instance);
		slot = cuddlk_manager_find_device_matching(
			cuddlk_global_manager_ptr,
			edata->id.group, edata->id.device, edata->id.resource,
			edata->id.instance, CUDDLK_RESOURCE_EVENTSRC, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		eslot = cuddlk_device_find_eventsrc(dev, edata->id.resource);
		if (eslot < 0) {
			ret = eslot;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);

		edata->info.priv.token.device_index = slot;
		edata->info.priv.token.resource_index = eslot;
		edata->info.flags = 0;
		edata->info.flags |= CUDDL_EVENTSRCF_WAITABLE;
		if (rt) {
			snprintf(edata->info.priv.device_name,
				 CUDDL_MAX_STR_LEN,
				 "/dev/rtdm/%s",
				 dev->priv.unique_name);
		} else {
			snprintf(edata->info.priv.device_name,
				 CUDDL_MAX_STR_LEN,
				 "/dev/uio%d",
				 dev->priv.uio.uio_dev->minor);
		}
		if (copy_to_user((void*)arg, edata, sizeof(*edata))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -1;
			break;
		}
		dev->events[eslot].kernel.ref_count += 1;
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->events[eslot].kernel.ref_count);
		break;

	case CUDDL_MEMREGION_RELEASE_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDL_MEMREGION_RELEASE_UIO_IOCTL:
		cuddlk_debug("CUDDL_MEMREGION_RELEASE_*_IOCTL called\n");
		if (copy_from_user(mrdata, (void*)arg, sizeof(*mrdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}
		slot = mrdata->token.device_index;
		mslot = mrdata->token.resource_index;
		cuddlk_debug("  token: %d %d\n", slot, mslot);

		if ((slot >= CUDDLK_MAX_MANAGED_DEVICES) || (slot < 0)) {
			ret = -1;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			ret = -1;
			break;
		}
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		if ((mslot >= CUDDLK_MAX_DEV_MEM_REGIONS) || (mslot < 0)) {
			ret = -1;
			break;
		}
		cuddlk_debug("  found mslot: %d\n", mslot);

		dev->mem[mslot].kernel.ref_count -= 1;
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->mem[mslot].kernel.ref_count);
		break;

	case CUDDL_EVENTSRC_RELEASE_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDL_EVENTSRC_RELEASE_UIO_IOCTL:
		cuddlk_debug("CUDDL_EVENTSRC_RELEASE_*_IOCTL called\n");
		if (copy_from_user(erdata, (void*)arg, sizeof(*erdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}
		slot = erdata->token.device_index;
		eslot = erdata->token.resource_index;
		cuddlk_debug("  token: %d %d\n", slot, eslot);

		if ((slot >= CUDDLK_MAX_MANAGED_DEVICES) || (slot < 0)) {
			ret = -1;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			ret = -1;
			break;
		}
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		if ((eslot >= CUDDLK_MAX_DEV_EVENTS) || (eslot < 0)) {
			ret = -1;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);

		dev->events[eslot].kernel.ref_count -= 1;
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->events[eslot].kernel.ref_count);
		break;

	case CUDDL_MANAGER_GET_MAX_DEVICES_IOCTL:
		cuddlk_debug("CUDDL_MANAGER_GET_MAX_DEVICES_IOCTL called\n");
		ret = CUDDLK_MAX_MANAGED_DEVICES;
		cuddlk_debug("  success\n");
		break;

	case CUDDL_DEVICE_GET_MAX_MEM_REGIONS_IOCTL:
		cuddlk_debug(
			"CUDDL_DEVICE_GET_MAX_MEM_REGIONS_IOCTL called\n");
		ret = CUDDLK_MAX_DEV_MEM_REGIONS;
		cuddlk_debug("  success\n");
		break;

	case CUDDL_DEVICE_GET_MAX_EVENTS_IOCTL:
		cuddlk_debug("CUDDL_DEVICE_GET_MAX_EVENTS_IOCTL called\n");
		ret = CUDDLK_MAX_DEV_EVENTS;
		cuddlk_debug("  success\n");
		break;

	case CUDDL_DEVICE_GET_MEM_REGION_ID_IOCTL:
		cuddlk_debug("CUDDL_DEVICE_GET_MEM_REGION_ID_IOCTL called\n");
		if (copy_from_user(
			    get_id_data, (void*)arg, sizeof(*get_id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}

		slot = get_id_data->device_slot;
		mslot = get_id_data->resource_slot;
		cuddlk_debug("  device_slot: %d, resource_slot: %d\n",
			     slot, mslot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -2;
			break;
		}
		if ((mslot < 0) || (mslot >= CUDDLK_MAX_DEV_MEM_REGIONS)) {
			cuddlk_debug("mem region slot out of range\n");
			ret = -3;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -4;
			break;
		}
		if(dev->mem[mslot].type == CUDDLK_MEMT_NONE) {
			cuddlk_debug("empty mem region slot\n");
			ret = -5;
			break;
		}

		memcpy(get_id_data->id.group, dev->group, CUDDL_MAX_STR_LEN);
		memcpy(get_id_data->id.device, dev->name, CUDDL_MAX_STR_LEN);
		memcpy(get_id_data->id.resource, dev->mem[mslot].name,
		       CUDDL_MAX_STR_LEN);
		get_id_data->id.instance = dev->instance;
		if (copy_to_user(
			    (void*)arg, get_id_data, sizeof(*get_id_data))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -6;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDL_DEVICE_GET_EVENT_ID_IOCTL:
		cuddlk_debug("CUDDL_DEVICE_GET_EVENT_ID_IOCTL called\n");
		if (copy_from_user(
			    get_id_data, (void*)arg, sizeof(*get_id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -1;
			break;
		}

		slot = get_id_data->device_slot;
		eslot = get_id_data->resource_slot;
		cuddlk_debug("  device_slot: %d, resource_slot: %d\n",
			     slot, eslot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -2;
			break;
		}
		if ((eslot < 0) || (eslot >= CUDDLK_MAX_DEV_EVENTS)) {
			cuddlk_debug("event slot out of range\n");
			ret = -3;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -4;
			break;
		}
		if(dev->events[eslot].intr.irq == CUDDLK_IRQ_NONE) {
			cuddlk_debug("empty event slot\n");
			ret = -5;
			break;
		}

		memcpy(get_id_data->id.group, dev->group, CUDDL_MAX_STR_LEN);
		memcpy(get_id_data->id.device, dev->name, CUDDL_MAX_STR_LEN);
		memcpy(get_id_data->id.resource, dev->events[eslot].name,
		       CUDDL_MAX_STR_LEN);
		get_id_data->id.instance = dev->instance;
		if (copy_to_user(
			    (void*)arg, get_id_data, sizeof(*get_id_data))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -6;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	default:
		cuddlk_print("Unknown Cuddl IOCTL\n");
		ret = -1;
	}

	kfree(get_id_data);
	kfree(erdata);
	kfree(mrdata);
	kfree(edata);
	kfree(mdata);
	return ret;
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
		cuddlk_print("%s: kzalloc failed: %d\n", __func__, ret);
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
