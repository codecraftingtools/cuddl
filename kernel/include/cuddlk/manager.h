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

#ifndef _CUDDLK_MANAGER_H
#define _CUDDLK_MANAGER_H

#include <cuddlk/device.h>

/**
 * DOC: Kernel-space device manager declarations.
 *
 * In most cases, Cuddl kernel drivers interact with the device manager by
 * simply calling the ``cuddlk_device_manage()`` and
 * ``cuddlk_device_release()`` routines described elsewhere.  This means that
 * most of the manager routines documented here (i.e. ``cuddlk_manager_*``)
 * are not typically called directly, but some of them may be useful in
 * certain situations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:macro:: CUDDLK_MAX_MANAGED_DEVICES
 *
 *    Maximum number of Cuddl devices that can be managed.
 */

#define CUDDLK_MAX_MANAGED_DEVICES 256

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
 * memory region or event source.  Typically, there is only a single, global
 * ``cuddlk_manager`` instance that is managed by the Cuddl implementation.
 * A pointer to this manager instance may be retrieved by calling
 * ``cuddlk_manager_lock()``.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_manager {
	struct cuddlk_device *devices[CUDDLK_MAX_MANAGED_DEVICES];
	struct cuddlki_manager_priv priv;
};

/**
 * cuddlk_manager_lock() - Retrieve an exclusive pointer to the global device
 *                         manager.
 *
 * This function may block if the device manager is currently in use.  The
 * ``cuddlk_manager_unlock()`` function should be called as soon as exclusive
 * access to the global device manager is no longer required to avoid
 * blocking other threads.  Note that ``cuddlk_device_manage()`` and
 * ``cuddlk_device_release()`` lock/unlock the global device manager
 * internally, so these two functions should not be called with the manager
 * locked.
 *
 * Return: Pointer to the global device manager instance, or ``NULL``.
 */
struct cuddlk_manager *cuddlk_manager_lock(void);

/**
 * cuddlk_manager_unlock() - Release exclusive access to the global device
 *                           manager.
 *
 * This function releases a lock on the global device manager that was
 * acquired by calling ``cuddlk_manager_lock()``.
 *
 * Return: Pointer to the global device manager instance, or ``NULL``.
 */
void cuddlk_manager_unlock(void);

/**
 * cuddlk_manager_add_device() - Start managing a Cuddl device.
 *
 * @manager: Cuddl manager instance.
 * @dev: Cuddl device to manage.
 *
 * Add the specified device to the device manager's ``devices`` array.  This
 * routine is automatically called from ``cuddlk_device_manage()``, so Cuddl
 * drivers do not typically need to call this routine directly.
 *
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENOMEM``: No empty device slots available.
 */
int cuddlk_manager_add_device(
	struct cuddlk_manager * manager, struct cuddlk_device *dev);

/**
 * cuddlk_manager_remove_device() - Stop managing a Cuddl device.
 *
 * @manager: Cuddl manager instance.
 * @dev: Cuddl device to stop managing.
 *
 * Remove the specified device from the device manager's ``devices`` array.
 * This routine is automatically called from ``cuddlk_device_release()``, so
 * Cuddl drivers do not typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddlk_manager_find_device_slot()``.
 */
int cuddlk_manager_remove_device(
	struct cuddlk_manager * manager, struct cuddlk_device *dev);

/**
 * cuddlk_manager_find_device_slot_matching() - Search for a device by name.
 *
 * @manager: Cuddl manager instance.
 *
 * @group: Name of group to search for or ``NULL`` / empty string.
 *
 * @name: Name of device to search for or ``NULL`` / empty string.
 *
 * @resource: Name of resource to search for or ``NULL`` / empty string.
 *
 * @instance: Instance number to search for or ``0``.
 *
 * @type: Type of resource associated with ``resource`` argument or ``0``.
 *
 * @start_index: Index in device array to start the search.
 *
 * Search the device manager's ``devices`` array for an entry matching the
 * specified (non-``NULL``) parameters.
 *
 * Return: Index of a matching device in the ``devices`` array on success, or
 * a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: No match was found.
 */
int cuddlk_manager_find_device_slot_matching(
	struct cuddlk_manager *manager,
	const char *group, const char *name, const char *resource,
	int instance, enum cuddlk_resource type, int start_index);

/**
 * cuddlk_manager_find_device_slot() - Search for a device by reference.
 *
 * @manager: Cuddl manager instance.
 * @dev: Cuddl device to search for.
 *
 * Search the device manager's ``devices`` array for the specified device.
 *
 * Return: Index of matching device in the ``devices`` array on success, or a
 * negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: No match was found.
 */
int cuddlk_manager_find_device_slot(
	struct cuddlk_manager *manager, struct cuddlk_device *dev);

/**
 * cuddlk_manager_find_empty_slot() - Search for an empty device slot.
 *
 * @manager: Cuddl manager instance.
 *
 * Search the device manager's ``devices`` array for an empty device slot.
 * This routine is not typically called directly by Cuddl kernel drivers.
 *
 * Return: Index of first empty slot in the ``devices`` array on success, or
 * a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddlk_manager_find_device_slot()``.
 */
int cuddlk_manager_find_empty_slot(struct cuddlk_manager *manager);

/**
 * cuddlk_manager_next_available_instance_id() - Find next unique instance id.
 *
 * Search the device manager's ``devices`` array for devices with a ``group``
 * and ``name`` combination matching the specified device.  The next
 * available instance identifier that is not used by one of the found devices
 * will be returned.  This routine is not typically called directly by Cuddl
 * kernel drivers.
 *
 * @manager: Cuddl manager instance.
 * @dev: Cuddl device supplying the ``group`` and ``name`` fields to search
 *       for.
 *
 * Return: Next available unique instance identifier on success, or a
 * negative error code.
 *
 *   Error codes:
 *     - ``-ENOMEM``: No empty managed device slots are available.
 */
int  cuddlk_manager_next_available_instance_id(
	struct cuddlk_manager *manager, struct cuddlk_device *dev);

#endif /* !_CUDDLK_MANAGER_H */
