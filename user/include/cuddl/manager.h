/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space manager decls.
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

#ifndef _CUDDL_DEVICE_H
#define _CUDDL_DEVICE_H

#include <cuddl/common_general.h>
#include <cuddl/impl.h>

/**
 * DOC: User-space device manager declarations.
 *
 * The Cuddl device manager exposes device memory regions and event sources
 * to user-space applications.  The routines described here may be used to
 * query the device memory regions and event sources that have been made
 * available to user-space applications by Cuddl kernel drivers.
 *
 * This part of the API is only available to user-space code.
 */

/**
 * cuddl_get_max_managed_devices() - Get max number of managed devices.
 *
 * Retrieve the number of slots in the device manager's array of devices.
 * This is the maximum number of devices that can be managed
 * (i.e. ``CUDDLK_MAX_MANAGED_DEVICES``).
 *
 * Return: Positive count of device slots or negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Negative value returned from ``ioctl()`` call on Cuddl
 *       manager device.
 */
int cuddl_get_max_managed_devices(void);

/**
 * cuddl_get_max_dev_mem_regions() - Get max number of device mem regions.
 *
 * Retrieve the number of memory region slots in a device's ``mem`` array.
 * This is the maximum number of memory regions that can be associated with a
 * single device (i.e. ``CUDDLK_MAX_DEV_MEM_REGIONS``).
 *
 * Return: Positive count of memory region slots or negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Negative value returned from ``ioctl()`` call on Cuddl
 *       manager device.
 */
int cuddl_get_max_dev_mem_regions(void);

/**
 * cuddl_get_max_dev_events() - Get max number of device event sources.
 *
 * Retrieve the number of event source slots in a device's ``events`` array.
 * This is the maximum number of event sources that can be associated with a
 * single device (i.e. ``CUDDLK_MAX_DEV_EVENTS``).
 *
 * Return: Positive count of event source slots or negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Negative value returned from ``ioctl()`` call on Cuddl
 *       manager device.
 */
int cuddl_get_max_dev_events(void);

/**
 * cuddl_get_memregion_id() - Get ID describing a mem region by slot.
 *
 * Retrieve a resource identifier describing the memory region associated
 * with the specified device and memory region slots.
 *
 * @id: Pointer to a data structure that will receive the resource identifier
 *      information associated with the specified device / memory region
 *      slot.  If a memory region is found in the specified slot, the
 *      required information will be copied into the data structure specified
 *      by this parameter.
 *
 * @device_slot: Device slot number (non-negative integer).
 *
 * @mem_slot: Memory region slot number (non-negative integer).
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 */
int cuddl_get_memregion_id(
	struct cuddl_resource_id *id, int device_slot, int mem_slot);

/**
 * cuddl_get_eventsrc_id() - Get ID describing an event source by slot.
 *
 * Retrieve a resource identifier describing the event source associated with
 * the specified device and event source slots.
 *
 * @id: Pointer to a data structure that will receive the resource identifier
 *      information associated with the specified device / event source slot.
 *      If an event source is found in the specified slot, the required
 *      information will be copied into the data structure specified by this
 *      parameter.
 *
 * @device_slot: Device slot number (non-negative integer).
 *
 * @event_slot: Event source slot number (non-negative integer).
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 */
int cuddl_get_eventsrc_id(
	struct cuddl_resource_id *id, int device_slot, int event_slot);

#endif /* !_CUDDL_DEVICE_H */
