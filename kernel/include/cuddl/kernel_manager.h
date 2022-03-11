/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel device manager decls.
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

#ifndef _CUDDL_KERNEL_MANAGER_H
#define _CUDDL_KERNEL_MANAGER_H

#include <cuddl/kernel_device.h>

/**
 * DOC: Kernel-space device manager declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:macro:: CUDDLK_MAX_MANAGED_DEVICES
 *
 *    Maximum number of Cuddl devices that can be managed.
 *
 * .. c:var:: extern struct cuddlk_manager *cuddlk_global_manager;
 *
 *    Global device manager instance pointer.
 */

#define CUDDLK_MAX_MANAGED_DEVICES 100

/**
 * enum cuddlk_resource - Types of device resources.
 *
 * @CUDDLK_RESOURCE_UNSPECIFIED: No resource type is specified.
 *
 * @CUDDLK_RESOURCE_MEMREGION: The associated resource is a memory region.
 *
 * @CUDDLK_RESOURCE_EVENTSRC: The associated resource is an event source.
 *
 * This type enumerates the kinds of device resources that may be queried by
 * name.
 */
enum cuddlk_resource {
	CUDDLK_RESOURCE_UNSPECIFIED = 0,
	CUDDLK_RESOURCE_MEMREGION   = 1,
	CUDDLK_RESOURCE_EVENTSRC    = 2,
};

/**
 * struct cuddlk_manager - Cuddl device manager data structure.
 *
 * @devices: Array of pointers to the devices currently being managed.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * The device manager keeps track of all managed Cuddl devices, their memory
 * regions, and their event sources.  User-space applications may query the
 * device manager to retrieve the information required to access a particular
 * memory region or event source.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_manager {
	struct cuddlk_device *devices[CUDDLK_MAX_MANAGED_DEVICES];
	struct cuddlk_manager_priv priv;
};

/**
 * cuddlk_manager_add_device() - Start managing a Cuddl device.
 *
 * @dev: Cuddl device to manage.
 *
 * Add the specified device to the global device manager's ``devices`` array.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_manager_add_device(struct cuddlk_device *dev);

/**
 * cuddlk_manager_remove_device() - Stop managing a Cuddl device.
 *
 * @dev: Cuddl device to stop managing.
 *
 * Remove the specified device from the global device manager's ``devices``
 * array.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_manager_remove_device(struct cuddlk_device *dev);

/**
 * cuddlk_manager_find_device_matching() - Search for a device by name.
 *
 * @group: Name of group to search for or ``NULL``.
 *
 * @name: Name of device to search for or ``NULL``.
 *
 * @resource: Name of resource to search for or ``NULL``.
 *
 * @instance: Instance number to search for or ``0``.
 *
 * @type: Type of resource associated with ``resource`` argument or ``0``.
 *
 * @start_index: Index in device array to start the search.
 *
 * Search the global device manager's ``devices`` array for an entry matching
 * the specified (non-``NULL``) parameters.
 *
 * Return: Index of a matching device in the ``devices`` array on success, or
 * a negative error code.
 */
int cuddlk_manager_find_device_matching(
	const char *group, const char *name, const char *resource,
	int instance, enum cuddlk_resource type, int start_index);

/**
 * cuddlk_manager_find_device() - Search for a device by reference.
 *
 * @dev: Cuddl device to search for.
 *
 * Search the global device manager's ``devices`` array for the specified
 * device.
 *
 * Return: Index of matching device in the ``devices`` array on success, or a
 * negative error code.
 */
int cuddlk_manager_find_device(struct cuddlk_device *dev);

/**
 * cuddlk_manager_find_empty_slot() - Search for an empty device slot.
 *
 * Search the global device manager's ``devices`` array for an empty device
 * slot.
 *
 * Return: Index of first empty slot in the ``devices`` array on success, or
 * a negative error code.
 */
int cuddlk_manager_find_empty_slot(void);

/* Global device manager instance. (kerneldoc comment at top of file) */
extern struct cuddlk_manager *cuddlk_global_manager;

#endif /* !_CUDDL_KERNEL_MANAGER_H */
