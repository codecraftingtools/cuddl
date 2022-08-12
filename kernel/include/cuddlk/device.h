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

#ifndef _CUDDLK_DEVICE_H
#define _CUDDLK_DEVICE_H

#include <cuddlk/memregion.h>
#include <cuddlk/eventsrc.h>

/**
 * DOC: Kernel-space device declarations.
 *
 * The primary function of a Cuddl kernel driver is to set up a
 * ``cuddlk_device`` data structure for each hardware peripheral, expose each
 * peripheral to user-space applications by calling
 * ``cuddlk_device_manage()``, and then clean them up up by calling
 * ``cuddlk_device_release()``.
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
 * typedef cuddlk_parent_device_t - Parent device type.
 *
 * OS-specific parent device type.
 *
 * On Linux, this type is a ``struct device``, which is usually the ``dev``
 * member of a ``struct pci_dev`` or ``struct platform_device``.
 *   
 * This type is not used on RTEMS, so it is defined as ``void``.
 */
typedef cuddlki_parent_device_t cuddlk_parent_device_t;

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
 * @driver_info: String used to identify the device driver and its revision.
 *
 * @hw_info: String used to identify the device hardware and its revision.
 *
 * @instance: Integer identifier to ensure uniqueness.
 *
 *            This field must be positive and unique when passed to
 *            ``cuddlk_device_register()``.  A value of ``0`` may be passed
 *            to ``cuddlk_device_manage()``, however.  In this case,
 *            ``cuddlk_device_manage()`` will determine the next available
 *            unique instance number for the specified ``group`` and ``name``
 *            combination and then update this field with the new value
 *            before calling ``cuddlk_device_register()``.
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
 * @parent_device_ptr: Parent device pointer.
 *
 *                     OS-specific field that contains a pointer to the
 *                     "parent" device.
 *                    
 *                     On Linux, this field should be a pointer to a ``struct
 *                     device``, which is usually the ``dev`` member of a
 *                     ``struct pci_dev`` or ``struct platform_device``.
 *                    
 *                     This field is not used on RTEMS.
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
 * Under Xenomai UDD, the ``GROUP``, ``NAME``, and ``INSTANCE`` fields are
 * used to construct device names::
 *
 *   Interrupt event device name: /dev/rtdm/<UNIQUE_NAME>
 *   Memory region 0 device name: /dev/rtdm/<UNIQUE_NAME>,mapper0
 *
 * where ``<UNIQUE_NAME>`` is equivalent to ``<GROUP>.<NAME>.<INSTANCE>``.
 *
 * Under Linux UIO, the value of ``<UNIQUE_NAME>`` can be read from a file::
 *
 *   e.g. /sys/class/uio/uio0/name
 *
 * The UIO device name itself, however, is simply generated from the
 * registration order::
 *
 *   /dev/uio0
 *
 * So, in the above case, the ``0`` has nothing to do with the device name.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_device {
	const char *group;
	const char *name;
	const char *driver_info;
	const char *hw_info;
	int instance;
	struct cuddlk_memregion mem[CUDDLK_MAX_DEV_MEM_REGIONS];
	struct cuddlk_eventsrc events[CUDDLK_MAX_DEV_EVENTS];
	cuddlk_parent_device_t *parent_device_ptr;
	struct cuddlk_device_kernel kernel;
	struct cuddlki_device_priv priv;
};

/**
 * cuddlk_device_find_memregion_slot() - Search for a memory region by name.
 *
 * @dev: Cuddl device to query.
 *
 * @name: Name of the memory region to search for.
 *
 * Return: Index of matching memory region in the ``mem`` array on success,
 * or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified memory region was not found.
 */
int cuddlk_device_find_memregion_slot(
	struct cuddlk_device *dev, const char *name);

/**
 * cuddlk_device_find_eventsrc_slot() - Search for an event source by name.
 *
 * @dev: Cuddl device to query.
 *
 * @name: Name of the event source to search for.
 *
 * Return: Index of matching event source in the ``events`` array on success,
 * or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified event source was not found.
 */
int cuddlk_device_find_eventsrc_slot(
	struct cuddlk_device *dev, const char *name);

/**
 * cuddlk_device_register() - Register a Cuddl device.
 *
 * @dev: Cuddl device to register.
 *
 * This routine is automatically called from ``cuddlk_device_manage()``, so
 * Cuddl drivers do not typically need to call this routine directly.
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
 *
 *   Error codes:
 *     - ``-EINVAL``: Invalid ``group`` or ``name`` argument.
 *     - ``-ENOMEM``: Memory allocation error.
 *     - Linux: Error code returned from ``uio_device_register()``.
 *     - Xenomai UDD: Error code returned from ``udd_register_device()``.
 */
int cuddlk_device_register(struct cuddlk_device *dev);

/**
 * cuddlk_device_unregister() - Unregister a Cuddl device.
 *
 * @dev: Cuddl device to unregister.
 *
 * This routine is automatically called from ``cuddlk_device_release()``, so
 * Cuddl drivers do not typically need to call this routine directly.
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
 *
 *   Error codes:
 *     - Linux UIO: None.
 *     - Xenomai UDD: Error code returned from ``udd_unregister_device()``.
 */
int cuddlk_device_unregister(struct cuddlk_device *dev);

/**
 * cuddlk_device_manage() - Register and manage Cuddl device.
 *
 * @dev: Cuddl device to manage.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-EINVAL``: Invalid ``group`` or ``name`` argument
 *     - ``-ENOMEM``: Memory allocation error.
 *     - Error code returned from
 *       `` cuddlk_manager_next_available_instance_id()``.
 *     - Error code returned from ``cuddlk_device_register()``.
 *     - Error code returned from ``cuddlk_manager_add_device()``.
 */
int cuddlk_device_manage(struct cuddlk_device *dev);

/**
 * cuddlk_device_release() - Release and unregister a managed Cuddl device.
 *
 * @dev: Managed Cuddl device to release.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned from ``cuddlk_manager_remove_device()``.
 *     - Error code returned from ``cuddlk_device_unregister()``.
 */
int cuddlk_device_release(struct cuddlk_device *dev);

#endif /* !_CUDDLK_DEVICE_H */
