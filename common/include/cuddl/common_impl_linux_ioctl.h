/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common Linux ioctl decls.
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

#ifndef _CUDDL_COMMON_IMPL_LINUX_IOCTL_H
#define _CUDDL_COMMON_IMPL_LINUX_IOCTL_H

#include <cuddl/common_memregion.h>
#include <cuddl/common_eventsrc.h>

/**
 * DOC: Common user/kernel-space declarations for the Linux IOTCL calls.
 *
 * These declarations apply to both Linux UIO and Xenomai UDD, and are used
 * to transfer information between user-space and kernel-space code.
 *
 * .. c:macro:: CUDDL_MEMREGION_CLAIM_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_claim()`` for Linux UIO.
 *
 * .. c:macro:: CUDDL_MEMREGION_CLAIM_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_claim()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDL_EVENTSRC_CLAIM_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_claim()`` for Linux UIO.
 *
 * .. c:macro:: CUDDL_EVENTSRC_CLAIM_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_claim()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDL_MEMREGION_RELEASE_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_release()`` for Linux UIO.
 *
 * .. c:macro:: CUDDL_MEMREGION_RELEASE_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_release()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDL_EVENTSRC_RELEASE_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_release()`` for Linux UIO.
 *
 * .. c:macro:: CUDDL_EVENTSRC_RELEASE_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_release()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDL_GET_MAX_MANAGED_DEVICES_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_managed_devices()``.
 *
 * .. c:macro:: CUDDL_GET_MAX_DEV_MEM_REGIONS_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_dev_mem_regions()``.
 *
 * .. c:macro:: CUDDL_GET_MAX_DEV_EVENTS_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_dev_events()``.
 *
 * .. c:macro:: CUDDL_GET_MEMREGION_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_id_for_slot()``.
 *
 * .. c:macro:: CUDDL_GET_EVENTSRC_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_id_for_slot()``.
 *
 * .. c:macro:: CUDDL_GET_MEMREGION_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_info_for_id()``.
 *
 * .. c:macro:: CUDDL_GET_EVENTSRC_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_info_for_id()``.
 *
 * .. c:macro:: CUDDL_GET_MEMREGION_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_ref_count_for_id()``.
 *
 * .. c:macro:: CUDDL_GET_EVENTSRC_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_ref_count_for_id()``.
 *
 * .. c:macro:: CUDDL_DECREMENT_MEMREGION_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_decrement_memregion_info_for_id()``.
 *
 * .. c:macro:: CUDDL_DECREMENT_EVENTSRC_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_decrement_eventsrc_info_for_id()``.
 *
 * .. c:macro:: CUDDL_JANITOR_REGISTER_PID_IOCTL
 *
 *    IOCTL associated with implicit ``cuddl_startup()`` call.
 *
 * .. c:macro:: CUDDL_GET_KERNEL_COMMIT_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_kernel_commit_id()``.
 *
 * .. c:macro:: CUDDL_GET__DRIVER_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_get_driver_info()`` and
 *    ``cuddl_eventsrc_get_driver_info()``.
 */

/**
 * struct cuddl_memregion_claim_ioctl_data - Memory region claim IOCTL data.
 *
 * @id: Resource identifier passed in from user space.
 * @info: Memory region information returned from kernel space.
 * @pid: Process id passed in from user space.
 */
struct cuddl_memregion_claim_ioctl_data {
	struct cuddl_resource_id id;
	struct cuddl_memregion_info info;
	pid_t pid;
};

/**
 * struct cuddl_eventsrc_claim_ioctl_data - Event source claim IOCTL data.
 *
 * @id: Resource identifier passed in from user space.
 * @info: Event source information returned from kernel space.
 * @pid: Process id passed in from user space.
 */
struct cuddl_eventsrc_claim_ioctl_data {
	struct cuddl_resource_id id;
	struct cuddl_eventsrc_info info;
	pid_t pid;
};

/**
 * struct cuddl_memregion_release_ioctl_data - Memregion release IOCTL data.
 *
 * @token: Token for resource to be released (passed in from user space).
 * @pid: Process id passed in from user space.
 */
struct cuddl_memregion_release_ioctl_data {
	struct cuddl_impl_token token;
	pid_t pid;
};

/**
 * struct cuddl_eventsrc_release_ioctl_data - Eventsrc release IOCTL data.
 *
 * @token: Token for resource to be released (passed in from user space).
 * @pid: Process id passed in from user space.
 */
struct cuddl_eventsrc_release_ioctl_data {
	struct cuddl_impl_token token;
	pid_t pid;
};

/**
 * struct cuddl_get_resource_id_ioctl_data - Get event id IOCTL data.
 *
 * Used for both ``cuddl_get_memregion_id_for_slot()`` and
 * ``cuddl_get_eventsrc_id_for_slot()`` implementations.
 *
 * @device_slot: Manager device slot number to query (input).
 * @resource_slot: Device memregion or eventsrc slot number to query (input).
 * @id: Resource identifier returned from kernel space (output).
 */
struct cuddl_get_resource_id_ioctl_data {
	int device_slot;
	int resource_slot;
	struct cuddl_resource_id id;
};

/**
 * struct cuddl_get_kernel_commit_id_ioctl_data - Get kern commit IOCTL data.
 *
 * @id_str: Kernel commit id string returned from kernel space.
 */
struct cuddl_get_kernel_commit_id_ioctl_data {
	char id_str[CUDDL_MAX_STR_LEN];
};

/**
 * struct cuddl_get_driver_info_ioctl_data - Get driver info string.
 *
 * @device_slot: Manager device slot number to query (input).
 * @info_str: Driver info string returned from kernel space.
 */
struct cuddl_get_driver_info_ioctl_data {
	int device_slot;
	char info_str[CUDDL_MAX_STR_LEN];
};

#define CUDDL_IOCTL_TYPE 'A'

#define CUDDL_MEMREGION_CLAIM_UIO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 2, struct cuddl_memregion_claim_ioctl_data)
#define CUDDL_MEMREGION_CLAIM_UDD_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 3, struct cuddl_memregion_claim_ioctl_data)
#define CUDDL_EVENTSRC_CLAIM_UIO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 4, struct cuddl_eventsrc_claim_ioctl_data)
#define CUDDL_EVENTSRC_CLAIM_UDD_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 5, struct cuddl_eventsrc_claim_ioctl_data)

#define CUDDL_MEMREGION_RELEASE_UIO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 6, struct cuddl_memregion_release_ioctl_data)
#define CUDDL_MEMREGION_RELEASE_UDD_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 7, struct cuddl_memregion_release_ioctl_data)
#define CUDDL_EVENTSRC_RELEASE_UIO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 8, struct cuddl_eventsrc_release_ioctl_data)
#define CUDDL_EVENTSRC_RELEASE_UDD_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 9, struct cuddl_eventsrc_release_ioctl_data)

#define CUDDL_GET_MAX_MANAGED_DEVICES_IOCTL _IO(CUDDL_IOCTL_TYPE, 10)
#define CUDDL_GET_MAX_DEV_MEM_REGIONS_IOCTL _IO(CUDDL_IOCTL_TYPE, 11)
#define CUDDL_GET_MAX_DEV_EVENTS_IOCTL _IO(CUDDL_IOCTL_TYPE, 12)

#define CUDDL_GET_MEMREGION_ID_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 13, struct cuddl_get_resource_id_ioctl_data)
#define CUDDL_GET_EVENTSRC_ID_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 14, struct cuddl_get_resource_id_ioctl_data)

#define CUDDL_GET_MEMREGION_INFO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 15, struct cuddl_memregion_claim_ioctl_data)
#define CUDDL_GET_EVENTSRC_INFO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 16, struct cuddl_eventsrc_claim_ioctl_data)

#define CUDDL_GET_MEMREGION_REF_COUNT_IOCTL \
  _IOW(CUDDL_IOCTL_TYPE, 17, struct cuddl_resource_id)
#define CUDDL_GET_EVENTSRC_REF_COUNT_IOCTL \
  _IOW(CUDDL_IOCTL_TYPE, 18, struct cuddl_resource_id)

#define CUDDL_DECREMENT_MEMREGION_REF_COUNT_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 19, struct cuddl_memregion_claim_ioctl_data)
#define CUDDL_DECREMENT_EVENTSRC_REF_COUNT_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 20, struct cuddl_eventsrc_claim_ioctl_data)

#define CUDDL_JANITOR_REGISTER_PID_IOCTL _IOWR(CUDDL_IOCTL_TYPE, 21, pid_t)

#define CUDDL_GET_KERNEL_COMMIT_ID_IOCTL \
  _IOR(CUDDL_IOCTL_TYPE, 22, struct cuddl_get_kernel_commit_id_ioctl_data)

#define CUDDL_GET_DRIVER_INFO_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 23, struct cuddl_get_driver_info_ioctl_data)

#endif /* !_CUDDL_COMMON_IMPL_LINUX_IOCTL_H */
