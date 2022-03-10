/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel device declarations.
 *
 * Copyright (C) 2022 Jeff Webb <jeff.webb@codecraftsmen.org>
 *
 * This software is dual-licensed and is made available under the terms of
 * the MIT License or under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.  You may select (at your
 * option) either of the licenses listed above.  See the LICENSE.MIT and
 * LICENSE.GPL-2.0 files in the top-level directory of this distribution for
 * copyright information and license terms.
 */

#ifndef _CUDDL_KERNEL_DEVICE_H
#define _CUDDL_KERNEL_DEVICE_H

#include <cuddl/kernel_memregion.h>
#include <cuddl/kernel_eventsrc.h>

/**
 * DOC: Kernel-space device declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * Note that the comments in this file regarding Linux UIO and Xenomai UDD
 * implementations are based on *linux-cip-4.19.94-cip18* and *xenomai-3.1*,
 * but other versions are probably very similar.
 *
 * .. c:macro:: CUDDLK_MAX_DEV_MEM_REGIONS
 *
 *    Maximum number of memory regions allowed for a single Cuddl device.
 *     
 *    Similar to::
 *     
 *      MAX_UIO_MAPS (#define) in linux/uio_driver.h for Linux   UIO
 *      UDD_NR_MAPS  (#define) in rtdm/udd.h         for Xenomai UDD
 *     
 *    Note that the equivalent Xenomai UDD and Linux UIO constants have
 *    similar values, but this is not strictly necessary for our purposes.
 *    If they differ, the minimum value will apply.
 *
 * .. c:macro:: CUDDLK_MAX_DEV_EVENTS
 *
 *    Maximum number of event sources allowed for a single Cuddl device.
 *
 *    Currently only one event source per Cuddl device is supported.
 *    Removing this limitation should be possible, but it would require
 *    substantial changes in the implementation because this feature is not
 *    supported natively under Linux UIO and Xenomai UDD.
 */

#define CUDDLK_MAX_DEV_MEM_REGIONS 5
#define CUDDLK_MAX_DEV_EVENTS 1

/**
 * struct cuddlk_device_kernel - Kernel-managed device data members.
 *
 * Kernel-managed ``cuddlk_device`` data members that are available for use
 * by Cuddl drivers.
 */
struct cuddlk_device_kernel {
};

/**
 * struct cuddlk_device - Cuddl device information.
 *
 * @group: Group used to identify the device.
 *
 * @name: Name used to identify the device.
 *
 *        Under Xenomai UDD, this field is used to construct device names::
 *         
 *          Interrupt event device name: /dev/rtdm/<NAME>
 *          Memory region 0 device name: /dev/rtdm/<NAME>,mapper0
 *         
 *        Under Linux UIO, the value of this field can be read from a file::
 *         
 *          e.g. /sys/class/uio/uio0/name
 *         
 *        but the UIO device name itself, however, is simply generated from
 *        the registration order::
 *         
 *          /dev/uio0
 *         
 *        So, in this case, the ``0`` has nothing to do with the device name.
 *
 * @instance: Integer identifier to ensure uniqueness.
 *
 * @mem: Array of memory regions to expose.
 *
 *       The associated UIO/UDD field declarations look like this::
 *
 *         UIO: struct uio_mem       mem        [MAX_UIO_MAPS];
 *         UDD: struct udd_memregion mem_regions[UDD_NR_MAPS];
 *
 * @events: Array of event sources to expose.
 *
 * @parent_dev_ptr: Parent device pointer.
 *
 *                  OS-specific field that contains a pointer to the "parent"
 *                  device.
 *                    
 *                  On Linux, this field should be a pointer to a ``struct
 *                  device``, which is usually the ``dev`` member of a
 *                  ``struct pci_dev`` or ``struct platform_device``.
 *                    
 *                  This field is not used on RTEMS.
 *
 * @kernel: Kernel-managed memory region data that is available for use by
 *          Cuddl drivers.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Describes a hardware device that has memory regions and/or event sources
 * that are exposed to user-space applications.
 *
 * Similar to::
 *
 *   struct uio_info   in linux/uio_driver.h for Linux   UIO
 *   struct udd_device in rtdm/udd.h         for Xenomai UDD
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_device {
	char *group;
	char *name;
	int instance;
	struct cuddlk_memregion mem[CUDDLK_MAX_DEV_MEM_REGIONS];
	struct cuddlk_eventsrc events[CUDDLK_MAX_DEV_EVENTS];
	cuddlk_parent_device_t *parent_dev_ptr;
	struct cuddlk_device_kernel kernel;
	struct cuddlk_device_priv priv;
};

/**
 * cuddlk_device_find_memregion() - Search for a memory region by name.
 *
 * @dev: Cuddl device to query.
 *
 * @name: Name of the memory region to search for.
 *
 * Return: Index of matching memory region in the ``mem`` array on success,
 * or a negative error code.
 */
int cuddlk_device_find_memregion(struct cuddlk_device *dev, const char *name);

/**
 * cuddlk_device_find_eventsrc() - Search for an event source by name.
 *
 * @dev: Cuddl device to query.
 *
 * @name: Name of the event source to search for.
 *
 * Return: Index of matching event source in the ``events`` array on success,
 * or a negative error code.
 */
int cuddlk_device_find_eventsrc(struct cuddlk_device *dev, const char *name);

/**
 * cuddlk_register_device() - Register a Cuddl device.
 *
 * @dev: Cuddl device to register.
 *
 * Linux UIO equivalent in *linux/uio_driver.h*::
 *
 *   int uio_register_device(struct device *parent, struct uio_info *info);
 *
 * Xenomai UDD equivalent in *rtdm/udd.h*::
 *
 *   int udd_register_device(struct udd_device *udd);
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_register_device(struct cuddlk_device *dev);

/**
 * cuddlk_unregister_device() - Unregister a Cuddl device.
 *
 * @dev: Cuddl device to unregister.
 *
 * Linux UIO equivalent in *linux/uio_driver.h*::
 *
 *   void uio_unregister_device(struct uio_info *info);
 *
 * Xenomai UDD equivalent in *rtdm/udd.h*::
 *
 *   int udd_unregister_device(struct udd_device *udd);
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_unregister_device(struct cuddlk_device *dev);

/**
 * cuddlk_manage_device() - Register and manage Cuddl device.
 *
 * @dev: Cuddl device to manage.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_manage_device(struct cuddlk_device *dev);

/**
 * cuddlk_release_device() - Release and unregister a managed Cuddl device.
 *
 * @dev: Managed Cuddl device to release.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_release_device(struct cuddlk_device *dev);

#endif /* !_CUDDL_KERNEL_DEVICE_H */
