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
#include <cuddlk.h>
#include <cuddl/common_impl_linux_ioctl.h>

/*
 * struct cuddlk_resource_ref_list - List of references to resources.
 *
 * @list: Linux kernel list implementation.
 * @token: Identifies the resource being referenced.
 * @pid: Process id holding the reference (for clean up).
 * 
 * This data structure is reserved for internal use by the Cuddl
 * implementation.
 */
struct cuddlk_resource_ref_list {
	struct list_head list;
	struct cuddlci_token token;
	pid_t pid;
};

/* Export symbols from cuddlk_manager_common.c */
EXPORT_SYMBOL_GPL(cuddlk_manager_find_device_slot_matching);
EXPORT_SYMBOL_GPL(cuddlk_manager_find_device_slot);
EXPORT_SYMBOL_GPL(cuddlk_manager_find_empty_slot);
EXPORT_SYMBOL_GPL(cuddlk_manager_next_available_instance_id);
EXPORT_SYMBOL_GPL(cuddlk_manager_add_device);
EXPORT_SYMBOL_GPL(cuddlk_manager_remove_device);
EXPORT_SYMBOL_GPL(cuddlk_device_manage);
EXPORT_SYMBOL_GPL(cuddlk_device_release);

static dev_t cuddlk_manager_dev;
static struct cdev cuddlk_manager_cdev;
static struct class *cuddlk_manager_class;
static struct cuddlk_manager *cuddlk_global_manager_ptr;

/* These are just for emergency clean-up */
struct cuddlk_resource_ref_list cuddlk_mem_refs;
struct cuddlk_resource_ref_list cuddlk_event_refs;

struct cuddlk_manager *cuddlk_manager_lock(void)
{
	mutex_lock(&cuddlk_global_manager_ptr->priv.global_mutex);
	return cuddlk_global_manager_ptr;
}
EXPORT_SYMBOL_GPL(cuddlk_manager_lock);

void cuddlk_manager_unlock(void)
{
	mutex_unlock(&cuddlk_global_manager_ptr->priv.global_mutex);
}
EXPORT_SYMBOL_GPL(cuddlk_manager_unlock);

static int _eventsrc_claim(struct cuddlk_eventsrc *eventsrc, int hostile)
{
	int failed = 0;

	mutex_lock(&eventsrc->priv.ref_mutex);

	if ((eventsrc->kernel.ref_count > 0) &&
	    !(eventsrc->flags & CUDDLK_EVENTSRCF_SHARED) &&
	    !hostile) {
		cuddlk_print("eventsrc claim failed (busy)\n");
		failed = -EBUSY;
	} else {
		eventsrc->kernel.ref_count += 1;
		if (eventsrc->priv.uio_ptr->uio_dev->owner) {
			try_module_get(
				eventsrc->priv.uio_ptr->uio_dev->owner);
		}
	}

	mutex_unlock(&eventsrc->priv.ref_mutex);

	return failed;
}

static int _eventsrc_decr_ref_count(struct cuddlk_eventsrc *eventsrc)
{
	int failed = 0;

	mutex_lock(&eventsrc->priv.ref_mutex);

	if (eventsrc->kernel.ref_count == 0) {
		cuddlk_print("eventsrc refcount already zero\n");
		failed = -ENOSPC;
	} else {
		eventsrc->kernel.ref_count -= 1;
		if (eventsrc->priv.uio_ptr->uio_dev->owner) {
			module_put(
				eventsrc->priv.uio_ptr->uio_dev->owner);
		}
	}

	mutex_unlock(&eventsrc->priv.ref_mutex);

	return failed;
}

static int _memregion_claim(struct cuddlk_memregion *memregion, int hostile)
{
	int failed = 0;

	mutex_lock(&memregion->priv.ref_mutex);

	if ((memregion->kernel.ref_count > 0) &&
	    !(memregion->flags & CUDDLK_MEMF_SHARED) &&
	    !hostile) {
		cuddlk_print("memregion claim failed (busy)\n");
		failed = -EBUSY;
	} else {
		memregion->kernel.ref_count += 1;
		if (memregion->priv.uio_ptr->uio_dev->owner) {
			try_module_get(
				memregion->priv.uio_ptr->uio_dev->owner);
		}
	}

	mutex_unlock(&memregion->priv.ref_mutex);

	return failed;
}

static int _memregion_decr_ref_count(struct cuddlk_memregion *memregion)
{
	int failed = 0;

	mutex_lock(&memregion->priv.ref_mutex);

	if (memregion->kernel.ref_count == 0) {
		cuddlk_print("memregion refcount already zero\n");
		failed = -ENOSPC;
	} else {
		memregion->kernel.ref_count -= 1;
		if (memregion->priv.uio_ptr->uio_dev->owner) {
			module_put(
				memregion->priv.uio_ptr->uio_dev->owner);
		}
	}

	mutex_unlock(&memregion->priv.ref_mutex);

	return failed;
}

void cuddlk_manager_free_refs_for_pid(pid_t pid)
{
	struct cuddlk_resource_ref_list *pos;
	struct cuddlk_resource_ref_list *tmp;
	struct cuddlk_resource_ref_list *ref_to_free;
	struct cuddlk_device *dev;
	int slot, mslot, eslot;

	cuddlk_manager_lock();

	list_for_each_entry_safe(
		pos, tmp, &cuddlk_mem_refs.list, list) {
		if (pos->pid == pid) {
			slot = pos->token.device_index;
			mslot = pos->token.resource_index;
			cuddlk_print("emergency clean up for pid %d, "
				     "mem slot: %d %d\n", pid, slot, mslot);
			dev = cuddlk_global_manager_ptr->devices[slot];
			_memregion_decr_ref_count(&dev->mem[mslot]);
			ref_to_free = pos;
			list_del(&pos->list);
			kfree(ref_to_free);
		}
	}

	list_for_each_entry_safe(
		pos, tmp, &cuddlk_event_refs.list, list) {
		if (pos->pid == pid) {
			slot = pos->token.device_index;
			eslot = pos->token.resource_index;
			cuddlk_print("emergency clean up for pid %d, "
				     "event slot: %d %d\n", pid, slot, eslot);
			dev = cuddlk_global_manager_ptr->devices[slot];
			_eventsrc_decr_ref_count(&dev->events[eslot]);
			ref_to_free = pos;
			list_del(&pos->list);
			kfree(ref_to_free);
		}
	}

	cuddlk_manager_unlock();
}
EXPORT_SYMBOL_GPL(cuddlk_manager_free_refs_for_pid);

static int _version_code_is_compat(int user_version_code) {
	if (CUDDLK_MAJOR_VERSION_FROM_CODE(user_version_code) ==
	    CUDDLK_MAJOR_VERSION_FROM_CODE(CUDDLK_VERSION_CODE))
		return 1;
	else if (CUDDLK_MAJOR_VERSION_FROM_CODE(user_version_code) == 0 &&
	         CUDDLK_MAJOR_VERSION_FROM_CODE(CUDDLK_VERSION_CODE) == 1)
		return 1;
	else if (CUDDLK_MAJOR_VERSION_FROM_CODE(user_version_code) == 1 &&
	         CUDDLK_MAJOR_VERSION_FROM_CODE(CUDDLK_VERSION_CODE) == 0)
		return 1;
	else
		return 0;
}

static long cuddlk_manager_ioctl(
	struct file *file, unsigned int cmd, unsigned long arg)
{
	int slot;
	int mslot;
	int eslot;
	int rt = 0;
	int claim = 0;
	int decrement = 0;
	int ret = 0;
	int freed_ref = 0;
	struct cuddlk_device *dev;
	struct cuddlci_memregion_claim_ioctl_data *mdata;
	struct cuddlci_eventsrc_claim_ioctl_data *edata;
	struct cuddlci_memregion_release_ioctl_data *mrdata;
	struct cuddlci_eventsrc_release_ioctl_data *erdata;
	struct cuddlci_get_resource_id_ioctl_data *get_id_data;
	struct cuddlci_get_kernel_commit_id_ioctl_data *commit_data;
	struct cuddlci_get_driver_info_ioctl_data *driver_info_data;
	struct cuddlci_void_ioctl_data *void_data;
	struct cuddlci_ref_count_ioctl_data *id_data;
	struct cuddlci_eventsrc_is_enabled_ioctl_data *is_enabled_data;
	struct cuddlk_resource_ref_list *tmp_ref;
	struct cuddlk_resource_ref_list *pos;
	struct cuddlk_resource_ref_list *tmp;
	struct cuddlk_resource_ref_list *ref_to_free;

	cuddlk_debug("cuddlk_manager_ioctl\n");
	cuddlk_debug("  cmd:  %u\n", cmd);
	cuddlk_debug("  dir:  %u\n", _IOC_DIR(cmd));
	cuddlk_debug("  type: %u\n", _IOC_TYPE(cmd));
	cuddlk_debug("  nr:   %u\n", _IOC_NR(cmd));
	cuddlk_debug("  size: %u\n", _IOC_SIZE(cmd));
	cuddlk_debug("  arg:  %lu\n", arg);

	mdata = kzalloc(
		sizeof(struct cuddlci_memregion_claim_ioctl_data),
		GFP_KERNEL);
	if (!mdata) {
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	edata = kzalloc(
		sizeof(struct cuddlci_eventsrc_claim_ioctl_data),
		GFP_KERNEL);
	if (!edata) {
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	mrdata = kzalloc(
		sizeof(struct cuddlci_memregion_release_ioctl_data),
		GFP_KERNEL);
	if (!mrdata) {
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	erdata = kzalloc(
		sizeof(struct cuddlci_eventsrc_release_ioctl_data),
		GFP_KERNEL);
	if (!erdata) {
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	get_id_data = kzalloc(
		sizeof(struct cuddlci_get_resource_id_ioctl_data),
		GFP_KERNEL);
	if (!get_id_data) {
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	commit_data = kzalloc(
		sizeof(struct cuddlci_get_kernel_commit_id_ioctl_data),
		GFP_KERNEL);
	if (!commit_data) {
		kfree(get_id_data);
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	driver_info_data = kzalloc(
		sizeof(struct cuddlci_get_driver_info_ioctl_data),
		GFP_KERNEL);
	if (!driver_info_data) {
		kfree(commit_data);
		kfree(get_id_data);
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	void_data = kzalloc(
		sizeof(struct cuddlci_void_ioctl_data),
		GFP_KERNEL);
	if (!void_data) {
		kfree(driver_info_data);
		kfree(commit_data);
		kfree(get_id_data);
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	id_data = kzalloc(
		sizeof(struct cuddlci_ref_count_ioctl_data),
		GFP_KERNEL);
	if (!id_data) {
		kfree(void_data);
		kfree(driver_info_data);
		kfree(commit_data);
		kfree(get_id_data);
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	is_enabled_data = kzalloc(
		sizeof(struct cuddlci_eventsrc_is_enabled_ioctl_data),
		GFP_KERNEL);
	if (!is_enabled_data) {
		kfree(id_data);
		kfree(void_data);
		kfree(driver_info_data);
		kfree(commit_data);
		kfree(get_id_data);
		kfree(erdata);
		kfree(mrdata);
		kfree(edata);
		kfree(mdata);
		cuddlk_print("kzalloc failed\n");
		return -ENOMEM;
	}

	cuddlk_manager_lock();

	switch(cmd) {
	case CUDDLCI_MEMREGION_CLAIM_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDLCI_MEMREGION_CLAIM_UIO_IOCTL:
		claim = 1;
		/* FALLTHROUGH */
	case CUDDLCI_GET_MEMREGION_INFO_IOCTL:
		cuddlk_debug("CUDDLCI_MEMREGION_CLAIM_*/INFO_IOCTL called\n");
		if (copy_from_user(mdata, (void*)arg, sizeof(*mdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(mdata->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		cuddlk_debug("  %s %s %s %d (pid: %d)\n",
			     mdata->id.group, mdata->id.device,
			     mdata->id.resource, mdata->id.instance,
			     mdata->pid);
		slot = cuddlk_manager_find_device_slot_matching(
			cuddlk_global_manager_ptr,
			mdata->id.group, mdata->id.device, mdata->id.resource,
			mdata->id.instance, CUDDLK_RESOURCE_MEMREGION, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		mslot = cuddlk_device_find_memregion_slot(
			dev, mdata->id.resource);
		if (mslot < 0) {
			ret = mslot;
			break;
		}
		cuddlk_debug("  found mslot: %d\n", mslot);

		if (claim) {
			ret = _memregion_claim(
				&dev->mem[mslot],
				mdata->options & CUDDL_MEM_CLAIMF_HOSTILE);
			if (ret)
				break;
		}

		mdata->info.priv.token.device_index = slot;
		mdata->info.priv.token.resource_index = mslot;
		mdata->info.priv.pa_len = dev->mem[mslot].pa_len;
		mdata->info.priv.start_offset = dev->mem[mslot].start_offset;
		mdata->info.len = dev->mem[mslot].len;
		mdata->info.flags = 0;
		if (dev->mem[mslot].flags & CUDDLK_MEMF_SHARED)
			mdata->info.flags |= CUDDL_MEMF_SHARED;
		if (rt) {
			mdata->info.priv.pa_mmap_offset = 0;
			snprintf(mdata->info.priv.device_name,
				 CUDDLCI_MAX_STR_LEN,
				 "/dev/rtdm/%s,mapper%d",
				 dev->priv.unique_name,
				 mslot);
		} else {
			mdata->info.priv.pa_mmap_offset = \
				mslot * CUDDLK_PAGE_SIZE;
			snprintf(mdata->info.priv.device_name,
				 CUDDLCI_MAX_STR_LEN,
				 "/dev/uio%d",
				 dev->priv.uio.uio_dev->minor);
		}
		if (copy_to_user((void*)arg, mdata, sizeof(*mdata))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			_memregion_decr_ref_count(&dev->mem[mslot]);
			break;
		}
		if (claim) {
			tmp_ref = kzalloc(
				sizeof(struct cuddlk_resource_ref_list),
				GFP_KERNEL);
			if (!tmp_ref) {
				cuddlk_print("kzalloc failed for ref\n");
				ret = -ENOMEM;
				_memregion_decr_ref_count(&dev->mem[mslot]);
				break;
			}
			tmp_ref->token.device_index =
				mdata->info.priv.token.device_index;
			tmp_ref->token.resource_index =
				mdata->info.priv.token.resource_index;
			tmp_ref->pid = mdata->pid;
			list_add(&tmp_ref->list, &cuddlk_mem_refs.list);
		}
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->mem[mslot].kernel.ref_count);
		break;

	case CUDDLCI_EVENTSRC_CLAIM_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDLCI_EVENTSRC_CLAIM_UIO_IOCTL:
		claim = 1;
		/* FALLTHROUGH */
	case CUDDLCI_GET_EVENTSRC_INFO_IOCTL:
		cuddlk_debug("CUDDLCI_EVENTSRC_CLAIM_*/INFO_IOCTL called\n");
		if (copy_from_user(edata, (void*)arg, sizeof(*edata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(edata->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		cuddlk_debug("  %s %s %s %d (pid: %d)\n",
			     edata->id.group, edata->id.device,
			     edata->id.resource, edata->id.instance,
			     edata->pid);
		slot = cuddlk_manager_find_device_slot_matching(
			cuddlk_global_manager_ptr,
			edata->id.group, edata->id.device, edata->id.resource,
			edata->id.instance, CUDDLK_RESOURCE_EVENTSRC, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		eslot = cuddlk_device_find_eventsrc_slot(
			dev, edata->id.resource);
		if (eslot < 0) {
			ret = eslot;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);

		if (claim) {
			ret = _eventsrc_claim(
				&dev->events[eslot],
				edata->options&CUDDL_EVENTSRC_CLAIMF_HOSTILE);
			if (ret)
				break;
		}

		edata->info.priv.token.device_index = slot;
		edata->info.priv.token.resource_index = eslot;
		edata->info.flags = 0;
		edata->info.flags |= CUDDL_EVENTSRCF_WAITABLE;
		if (dev->events[eslot].flags & CUDDLK_EVENTSRCF_SHARED)
			edata->info.flags |= CUDDL_EVENTSRCF_SHARED;
		if (dev->events[eslot].intr.enable)
			edata->info.flags |= CUDDL_EVENTSRCF_HAS_ENABLE;
		if (dev->events[eslot].intr.disable)
			edata->info.flags |= CUDDL_EVENTSRCF_HAS_DISABLE;
		if (dev->events[eslot].intr.is_enabled)
			edata->info.flags |= CUDDL_EVENTSRCF_HAS_IS_ENABLED;

		if (rt) {
			snprintf(edata->info.priv.device_name,
				 CUDDLCI_MAX_STR_LEN,
				 "/dev/rtdm/%s",
				 dev->priv.unique_name);
		} else {
			snprintf(edata->info.priv.device_name,
				 CUDDLCI_MAX_STR_LEN,
				 "/dev/uio%d",
				 dev->priv.uio.uio_dev->minor);
		}
		if (copy_to_user((void*)arg, edata, sizeof(*edata))) {
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			_eventsrc_decr_ref_count(&dev->events[eslot]);
			break;
		}
		if (claim) {
			tmp_ref = kzalloc(
				sizeof(struct cuddlk_resource_ref_list),
				GFP_KERNEL);
			if (!tmp_ref) {
				cuddlk_print("kzalloc failed for ref\n");
				ret = -ENOMEM;
				_eventsrc_decr_ref_count(&dev->events[eslot]);
				break;
			}
			tmp_ref->token.device_index =
				edata->info.priv.token.device_index;
			tmp_ref->token.resource_index =
				edata->info.priv.token.resource_index;
			tmp_ref->pid = edata->pid;
			list_add(&tmp_ref->list, &cuddlk_event_refs.list);
		}
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->events[eslot].kernel.ref_count);
		break;

	case CUDDLCI_MEMREGION_RELEASE_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDLCI_MEMREGION_RELEASE_UIO_IOCTL:
		cuddlk_debug("CUDDLCI_MEMREGION_RELEASE_*_IOCTL called\n");
		if (copy_from_user(mrdata, (void*)arg, sizeof(*mrdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(mrdata->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = mrdata->token.device_index;
		mslot = mrdata->token.resource_index;
		cuddlk_debug("  token: %d %d (pid: %d)\n", slot, mslot,
			     mrdata->pid);

		if ((slot >= CUDDLK_MAX_MANAGED_DEVICES) || (slot < 0)) {
			ret = -EBADSLT;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			ret = -ENODEV;
			break;
		}
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		if ((mslot >= CUDDLK_MAX_DEV_MEM_REGIONS) || (mslot < 0)) {
			ret = -EBADSLT;
			break;
		}
		cuddlk_debug("  found mslot: %d\n", mslot);

		_memregion_decr_ref_count(&dev->mem[mslot]);
		freed_ref = 0;
		list_for_each_entry_safe(
			pos, tmp, &cuddlk_mem_refs.list, list) {
			if ((slot        == pos->token.device_index) &&
			    (mslot       == pos->token.resource_index) &&
			    (mrdata->pid == pos->pid)) {
				cuddlk_debug("clean up ref for pid %d, "
					     "mem slot: %d %d\n",
					     pos->pid, slot, mslot);
				ref_to_free = pos;
				list_del(&pos->list);
				kfree(ref_to_free);
				freed_ref = 1;
				break;
			}
		}
		if (!freed_ref)
			cuddlk_print("could not clean up mem ref\n");
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->mem[mslot].kernel.ref_count);
		break;

	case CUDDLCI_EVENTSRC_RELEASE_UDD_IOCTL:
		rt = 1;
		/* FALLTHROUGH */
	case CUDDLCI_EVENTSRC_RELEASE_UIO_IOCTL:
		cuddlk_debug("CUDDLCI_EVENTSRC_RELEASE_*_IOCTL called\n");
		if (copy_from_user(erdata, (void*)arg, sizeof(*erdata))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(erdata->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = erdata->token.device_index;
		eslot = erdata->token.resource_index;
		cuddlk_debug("  token: %d %d (pid: %d)\n", slot, eslot,
			     erdata->pid);

		if ((slot >= CUDDLK_MAX_MANAGED_DEVICES) || (slot < 0)) {
			ret = -EBADSLT;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			ret = -ENODEV;
			break;
		}
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		if ((eslot >= CUDDLK_MAX_DEV_EVENTS) || (eslot < 0)) {
			ret = -EBADSLT;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);

		_eventsrc_decr_ref_count(&dev->events[eslot]);
		freed_ref = 0;
		list_for_each_entry_safe(
			pos, tmp, &cuddlk_event_refs.list, list) {
			if ((slot        == pos->token.device_index) &&
			    (eslot       == pos->token.resource_index) &&
			    (erdata->pid == pos->pid)) {
				cuddlk_debug("clean up ref for pid %d, "
					     "event slot: %d %d\n",
					     pos->pid, slot, eslot);
				ref_to_free = pos;
				list_del(&pos->list);
				kfree(ref_to_free);
				freed_ref = 1;
				break;
			}
		}
		if (!freed_ref)
			cuddlk_print("could not clean up event ref\n");
		cuddlk_debug("  success: ref_count: %d\n",
		       dev->events[eslot].kernel.ref_count);
		break;

	case CUDDLCI_GET_MAX_MANAGED_DEVICES_IOCTL:
		cuddlk_debug("CUDDLCI_GET_MAX_MANAGED_DEVICES_IOCTL called\n");
		if (copy_from_user(
			    void_data, (void*)arg, sizeof(*void_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(void_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		ret = CUDDLK_MAX_MANAGED_DEVICES;
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_MAX_DEV_MEM_REGIONS_IOCTL:
		cuddlk_debug(
			"CUDDLCI_GET_MAX_DEV_MEM_REGIONS_IOCTL called\n");
		if (copy_from_user(
			    void_data, (void*)arg, sizeof(*void_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(void_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		ret = CUDDLK_MAX_DEV_MEM_REGIONS;
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_MAX_DEV_EVENTS_IOCTL:
		cuddlk_debug("CUDDLCI_GET_MAX_DEV_EVENTS_IOCTL called\n");
		if (copy_from_user(
			    void_data, (void*)arg, sizeof(*void_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(void_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		ret = CUDDLK_MAX_DEV_EVENTS;
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_MEMREGION_ID_IOCTL:
		cuddlk_debug("CUDDLCI_GET_MEMREGION_ID_IOCTL called\n");
		if (copy_from_user(
			    get_id_data, (void*)arg, sizeof(*get_id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(get_id_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = get_id_data->device_slot;
		mslot = get_id_data->resource_slot;
		cuddlk_debug("  device_slot: %d, resource_slot: %d\n",
			     slot, mslot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -EBADSLT;
			break;
		}
		if ((mslot < 0) || (mslot >= CUDDLK_MAX_DEV_MEM_REGIONS)) {
			cuddlk_debug("mem region slot out of range\n");
			ret = -EBADSLT;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -ENODEV;
			break;
		}
		if(dev->mem[mslot].type == CUDDLK_MEMT_NONE) {
			cuddlk_debug("empty mem region slot\n");
			ret = -EINVAL;
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
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_EVENTSRC_ID_IOCTL:
		cuddlk_debug("CUDDLCI_GET_EVENTSRC_ID_IOCTL called\n");
		if (copy_from_user(
			    get_id_data, (void*)arg, sizeof(*get_id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(get_id_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = get_id_data->device_slot;
		eslot = get_id_data->resource_slot;
		cuddlk_debug("  device_slot: %d, resource_slot: %d\n",
			     slot, eslot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -EBADSLT;
			break;
		}
		if ((eslot < 0) || (eslot >= CUDDLK_MAX_DEV_EVENTS)) {
			cuddlk_debug("event slot out of range\n");
			ret = -EBADSLT;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -ENODEV;
			break;
		}
		if(dev->events[eslot].intr.irq == CUDDLK_IRQ_NONE) {
			cuddlk_debug("empty event slot\n");
			ret = -EINVAL;
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
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_DECREMENT_MEMREGION_REF_COUNT_IOCTL:
		decrement = 1;
		/* FALLTHROUGH */
	case CUDDLCI_GET_MEMREGION_REF_COUNT_IOCTL:
		cuddlk_debug(
			"CUDDLCI_GET_MEMREGION_REF_COUNT_IOCTL called\n");
		if (copy_from_user(
			    id_data, (void*)arg, sizeof(*id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(id_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		cuddlk_debug("  %s %s %s %d\n",
			     id_data->id.group, id_data->id.device,
			     id_data->id.resource, id_data->id.instance);
		slot = cuddlk_manager_find_device_slot_matching(
			cuddlk_global_manager_ptr,
			id_data->id.group, id_data->id.device,
			id_data->id.resource, id_data->id.instance,
			CUDDLK_RESOURCE_MEMREGION, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		mslot = cuddlk_device_find_memregion_slot(
			dev, id_data->id.resource);
		if (mslot < 0) {
			ret = mslot;
			break;
		}
		cuddlk_debug("  found mslot: %d\n", mslot);

		if (decrement) {
			ret = _memregion_decr_ref_count(&dev->mem[mslot]);
			if (ret)
				break;
		} else {
			ret = dev->mem[mslot].kernel.ref_count;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_DECREMENT_EVENTSRC_REF_COUNT_IOCTL:
		decrement = 1;
		/* FALLTHROUGH */
	case CUDDLCI_GET_EVENTSRC_REF_COUNT_IOCTL:
		cuddlk_debug("CUDDLCI_GET_EVENTSRC_REF_COUNT_IOCTL called\n");
		if (copy_from_user(
			    id_data, (void*)arg, sizeof(*id_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(id_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		cuddlk_debug("  %s %s %s %d\n",
			     id_data->id.group, id_data->id.device,
			     id_data->id.resource, id_data->id.instance);
		slot = cuddlk_manager_find_device_slot_matching(
			cuddlk_global_manager_ptr,
			id_data->id.group, id_data->id.device,
			id_data->id.resource, id_data->id.instance,
			CUDDLK_RESOURCE_EVENTSRC, 0);
		if (slot < 0) {
			ret = slot;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);

		eslot = cuddlk_device_find_eventsrc_slot(
			dev, id_data->id.resource);
		if (eslot < 0) {
			ret = eslot;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);

		if (decrement) {
			ret = _eventsrc_decr_ref_count(&dev->events[eslot]);
			if (ret)
				break;
		} else {
			ret = dev->events[eslot].kernel.ref_count;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_JANITOR_REGISTER_PID_IOCTL:
		cuddlk_print("error: CUDDLCI_JANITOR_REGISTER_PID_IOCTL "
			     "received on manager device instead of "
			     "janitor device\n");
		ret = -ENOSYS;
		break;

	case CUDDLCI_GET_KERNEL_COMMIT_ID_IOCTL:
		cuddlk_debug("CUDDLCI_GET_KERNEL_COMMIT_ID_IOCTL called\n");
		if (copy_from_user(
			    commit_data, (void*)arg, sizeof(*commit_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(commit_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		cuddlk_get_commit_id(
			commit_data->id_str, CUDDLCI_MAX_STR_LEN);
		if (copy_to_user(
		    (void*)arg, commit_data, sizeof(*commit_data)))
		{
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_DRIVER_INFO_IOCTL:
		cuddlk_debug(
			"CUDDLCI_GET_DRIVER_INFO_IOCTL called\n");
		if (copy_from_user(
			    driver_info_data, (void*)arg,
			    sizeof(*driver_info_data)))
		{
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(
			    driver_info_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = driver_info_data->device_slot;
		cuddlk_debug("  device_slot: %d\n", slot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -EBADSLT;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -ENODEV;
			break;
		}

		if (dev->driver_info)
			strncpy(driver_info_data->info_str, dev->driver_info,
				CUDDLCI_MAX_STR_LEN);
		else
			strncpy(driver_info_data->info_str, "UNKNOWN",
				CUDDLCI_MAX_STR_LEN);
		if (copy_to_user(
		    (void*)arg, driver_info_data, sizeof(*driver_info_data)))
		{
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_HW_INFO_IOCTL:
		cuddlk_debug(
			"CUDDLCI_GET_HW_INFO_IOCTL called\n");
		if (copy_from_user(
			    driver_info_data, (void*)arg,
			    sizeof(*driver_info_data)))
		{
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}

		if (!_version_code_is_compat(
			    driver_info_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}

		slot = driver_info_data->device_slot;
		cuddlk_debug("  device_slot: %d\n", slot);

		if ((slot < 0) || (slot >= CUDDLK_MAX_MANAGED_DEVICES)) {
			cuddlk_debug("device slot out of range\n");
			ret = -EBADSLT;
			break;
		}

		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			cuddlk_debug("empty device slot\n");
			ret = -ENODEV;
			break;
		}

		if (dev->hw_info)
			strncpy(driver_info_data->info_str, dev->hw_info,
				CUDDLCI_MAX_STR_LEN);
		else
			strncpy(driver_info_data->info_str, "UNKNOWN",
				CUDDLCI_MAX_STR_LEN);
		if (copy_to_user(
		    (void*)arg, driver_info_data, sizeof(*driver_info_data)))
		{
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_GET_KERNEL_VERSION_CODE_IOCTL:
		cuddlk_debug(
			"CUDDLCI_GET_KERNEL_VERSION_CODE_IOCTL called\n");
		if (copy_from_user(
			    void_data, (void*)arg, sizeof(*void_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(void_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		cuddlk_debug("  success\n");
		ret = CUDDLK_VERSION_CODE;
		break;

	case CUDDLCI_GET_KERNEL_VARIANT_IOCTL:
		cuddlk_debug("CUDDLCI_GET_KERNEL_VARIANT_IOCTL called\n");
		if (copy_from_user(
			    commit_data, (void*)arg, sizeof(*commit_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(commit_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		strncpy(commit_data->id_str, CUDDLK_VARIANT,
			CUDDLCI_MAX_STR_LEN);
		if (copy_to_user(
		    (void*)arg, commit_data, sizeof(*commit_data)))
		{
			cuddlk_print("copy_to_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		cuddlk_debug("  success\n");
		break;

	case CUDDLCI_EVENTSRC_IS_ENABLED_IOCTL:
		cuddlk_debug(
			"CUDDLCI_EVENTSRC_IS_ENABLED_IOCTL called\n");
		if (copy_from_user(
			    is_enabled_data, (void*)arg,
			    sizeof(*is_enabled_data))) {
			cuddlk_print("copy_from_user failed\n");
			ret = -EOVERFLOW;
			break;
		}
		if (!_version_code_is_compat(is_enabled_data->version_code)) {
			cuddlk_print("cuddl user/kernel version mismatch "
			             "in IOCTL\n");
			ret = -ENOEXEC;
			break;
		}
		slot = is_enabled_data->token.device_index;
		eslot = is_enabled_data->token.resource_index;
		cuddlk_debug("  token: %d %d\n", slot, eslot);
		if ((slot >= CUDDLK_MAX_MANAGED_DEVICES) || (slot < 0)) {
			ret = -EBADSLT;
			break;
		}
		dev = cuddlk_global_manager_ptr->devices[slot];
		if (!dev) {
			ret = -ENODEV;
			break;
		}
		cuddlk_debug("  found slot: %d (dev_ptr: %p)\n", slot, dev);
		if ((eslot >= CUDDLK_MAX_DEV_EVENTS) || (eslot < 0)) {
			ret = -EBADSLT;
			break;
		}
		cuddlk_debug("  found eslot: %d\n", eslot);
		if (!dev->events[eslot].intr.is_enabled) {
			ret = -EINVAL;
			break;
		}
		ret = dev->events[eslot].intr.is_enabled(
			&dev->events[eslot].intr);
		if (ret < 0)
			break;
		cuddlk_debug("  success\n");
		break;

	default:
		cuddlk_print("Unknown Cuddl manager IOCTL\n");
		ret = -ENOSYS;
	}

	cuddlk_manager_unlock();

	kfree(is_enabled_data);
	kfree(id_data);
	kfree(void_data);
	kfree(driver_info_data);
	kfree(commit_data);
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
	cuddlk_manager_device = NULL;

	return ret;
}

static int __init cuddlk_manager_init(void)
{
	int ret;
	enum cuddlk_manager_init_failure failure;
	
	INIT_LIST_HEAD(&cuddlk_mem_refs.list);
	INIT_LIST_HEAD(&cuddlk_event_refs.list);

	cuddlk_global_manager_ptr = kzalloc(
		sizeof(struct cuddlk_manager), GFP_KERNEL);
	if (!cuddlk_global_manager_ptr) {
		ret = -ENOMEM;
		failure = CUDDLK_MGR_FAIL_ALLOC_MANAGER;
		cuddlk_print("%s: kzalloc failed: %d\n", __func__, ret);
		goto handle_failure;
	}

	mutex_init(&cuddlk_global_manager_ptr->priv.global_mutex);

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
	struct cuddlk_resource_ref_list *pos;
	struct cuddlk_resource_ref_list *tmp;
	struct cuddlk_resource_ref_list *ref_to_free;

	list_for_each_entry_safe(
		pos, tmp, &cuddlk_mem_refs.list, list) {
		cuddlk_print("fallback clean up for mem slot: %d %d\n",
			     pos->token.device_index,
			     pos->token.resource_index);
		ref_to_free = pos;
		list_del(&pos->list);
		kfree(ref_to_free);
	}

	list_for_each_entry_safe(
		pos, tmp, &cuddlk_event_refs.list, list) {
		cuddlk_print("fallback clean up for event slot: %d %d\n",
			     pos->token.device_index,
			     pos->token.resource_index);
		ref_to_free = pos;
		list_del(&pos->list);
		kfree(ref_to_free);
	}

	cuddlk_manager_cleanup(CUDDLK_MGR_NO_FAILURE);
}

module_init(cuddlk_manager_init)
module_exit(cuddlk_manager_exit)
MODULE_LICENSE("GPL");
