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
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
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
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
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
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_max_dev_events(void);

/**
 * cuddl_get_memregion_id_for_slot() - Get ID describing a mem region by slot.
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
 *     - ``-ENODEV``: The specified device slot is empty.
 *     - ``-EINVAL``: The specified memory slot is empty.
 *     - ``-EBADSLT``: The specified device or mem slot is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_memregion_id_for_slot(
	struct cuddl_resource_id *id, int device_slot, int mem_slot);

/**
 * cuddl_get_eventsrc_id_for_slot() - Get ID describing an event src by slot.
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
 *     - ``-ENODEV``: The specified device slot is empty.
 *     - ``-EINVAL``: The specified event slot is empty.
 *     - ``-EBADSLT``: The specified device or event slot is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_eventsrc_id_for_slot(
	struct cuddl_resource_id *id, int device_slot, int event_slot);

/**
 * cuddl_get_memregion_info_for_id() - Retrieve memory region information.
 *
 * @meminfo: Pointer to a data structure that will receive the memory region
 *           information.  If the call is successful, the required
 *           information will be copied into the data structure specified by
 *           this parameter.
 *
 * @memregion_id: Resource identifier input identifying the memory region to
 *                be queried.
 *
 * Retrieve the properties of the specified memory region.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified memory region was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_memregion_info_for_id(
	struct cuddl_memregion_info *meminfo,
	const struct cuddl_resource_id *memregion_id);

/**
 * cuddl_get_eventsrc_info_for_id() - Retrieve event source information.
 *
 * @eventinfo: Pointer to a data structure that will receive the event source
 *             information.  If the call is successful, the required
 *             information will be copied into the data structure specified
 *             by this parameter.
 *
 * @eventsrc_id: Resource identifier input identifying the event source to be
 *               queried.
 *
 * Retrieve the properties of the specified event source.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified event source was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_eventsrc_info_for_id(
	struct cuddl_eventsrc_info *eventinfo,
	const struct cuddl_resource_id *eventsrc_id);

/**
 * cuddl_get_memregion_ref_count_for_id() - Return the memregion ref count.
 *
 * @memregion_id: Resource identifier input identifying the memory region to
 *                be queried.
 *
 * Return the reference count associated with the specified memory region.
 *
 * Return: Positive (or zero) reference count or negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified memory region was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_memregion_ref_count_for_id(
	const struct cuddl_resource_id *memregion_id);

/**
 * cuddl_get_eventsrc_ref_count_for_id() - Return the eventsrc ref count.
 *
 * @eventsrc_id: Resource identifier input identifying the event source to be
 *               queried.
 *
 * Return the reference count associated with the specified event source.
 *
 * Return: Positive (or zero) reference count or negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified event source was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_eventsrc_ref_count_for_id(
	const struct cuddl_resource_id *eventsrc_id);

/**
 * cuddl_decrement_memregion_ref_count_for_id() - Decrement mem ref count.
 *
 * @memregion_id: Resource identifier input identifying the memory region to
 *                be queried.
 *
 * Decrement the reference count associated with the specified memory region.
 * This functionality should only used to free resources when the standard
 * mechanism has failed for some reason.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified memory region was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_decrement_memregion_ref_count_for_id(
	const struct cuddl_resource_id *memregion_id);

/**
 * cuddl_decrement_eventsrc_ref_count_for_id() - Decrement event ref count.
 *
 * @eventsrc_id: Resource identifier input identifying the event source to be
 *               queried.
 *
 * Decrement the reference count associated with the specified event source.
 * This functionality should only used to free resources when the standard
 * mechanism has failed for some reason.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified event source was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_decrement_eventsrc_ref_count_for_id(
	const struct cuddl_resource_id *eventsrc_id);

/**
 * cuddl_get_driver_info_for_memregion_id() - Get the driver info string
 *                                            associated with the mem region.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * @memregion_id: Resource identifier input identifying the memory region to
 *                be queried.
 *
 * Retrieve the driver information string associated with the specified
 * memory region.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_get_memregion_info_for_id()``
 *       (Linux).
 *     - ``-ENODEV``: The device slot associated with the specified resource
 *       id is empty.
 *     - ``-EBADSLT``: The device slot associated with the specified resource
 *       id is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_driver_info_for_memregion_id(
	char *info_str, cuddl_size_t info_len,
	const struct cuddl_resource_id *memregion_id);

/**
 * cuddl_get_driver_info_for_eventsrc_id() - Get the driver info string
 *                                           associated with the event source.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * @eventsrc_id: Resource identifier input identifying the event source to be
 *               queried.
 *
 * Retrieve the driver information string associated with the specified
 * event source.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_get_eventsrc_info_for_id()`` (Linux).
 *     - ``-ENODEV``: The device slot associated with the specified resource
 *       id is empty.
 *     - ``-EBADSLT``: The device slot associated with the specified resource
 *       id is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_driver_info_for_eventsrc_id(
	char *info_str, cuddl_size_t info_len,
	const struct cuddl_resource_id *eventsrc_id);

/**
 * cuddl_get_hw_info_for_memregion_id() - Get the hardware info string
 *                                        associated with the mem region.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * @memregion_id: Resource identifier input identifying the memory region to
 *                be queried.
 *
 * Retrieve the hardware information string associated with the specified
 * memory region.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_get_memregion_info_for_id()``
 *       (Linux).
 *     - ``-ENODEV``: The device slot associated with the specified resource
 *       id is empty.
 *     - ``-EBADSLT``: The device slot associated with the specified resource
 *       id is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_hw_info_for_memregion_id(
	char *info_str, cuddl_size_t info_len,
	const struct cuddl_resource_id *memregion_id);

/**
 * cuddl_get_hw_info_for_eventsrc_id() - Get the hardware info string
 *                                       associated with the event source.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * @eventsrc_id: Resource identifier input identifying the event source to be
 *               queried.
 *
 * Retrieve the hardware information string associated with the specified
 * event source.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_get_eventsrc_info_for_id()`` (Linux).
 *     - ``-ENODEV``: The device slot associated with the specified resource
 *       id is empty.
 *     - ``-EBADSLT``: The device slot associated with the specified resource
 *       id is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
 */
int cuddl_get_hw_info_for_eventsrc_id(
	char *info_str, cuddl_size_t info_len,
	const struct cuddl_resource_id *eventsrc_id);

#endif /* !_CUDDL_DEVICE_H */
