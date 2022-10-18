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
 * .. c:macro:: CUDDLCI_MEMREGION_CLAIM_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_claim()`` for Linux UIO.
 *
 * .. c:macro:: CUDDLCI_MEMREGION_CLAIM_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_claim()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDLCI_EVENTSRC_CLAIM_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_claim()`` for Linux UIO.
 *
 * .. c:macro:: CUDDLCI_EVENTSRC_CLAIM_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_claim()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDLCI_MEMREGION_RELEASE_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_release()`` for Linux UIO.
 *
 * .. c:macro:: CUDDLCI_MEMREGION_RELEASE_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_release()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDLCI_EVENTSRC_RELEASE_UIO_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_release()`` for Linux UIO.
 *
 * .. c:macro:: CUDDLCI_EVENTSRC_RELEASE_UDD_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_release()`` for Xenomai UDD.
 *
 * .. c:macro:: CUDDLCI_GET_MAX_MANAGED_DEVICES_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_managed_devices()``.
 *
 * .. c:macro:: CUDDLCI_GET_MAX_DEV_MEM_REGIONS_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_dev_mem_regions()``.
 *
 * .. c:macro:: CUDDLCI_GET_MAX_DEV_EVENTS_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_max_dev_events()``.
 *
 * .. c:macro:: CUDDLCI_GET_MEMREGION_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_id_for_slot()``.
 *
 * .. c:macro:: CUDDLCI_GET_EVENTSRC_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_id_for_slot()``.
 *
 * .. c:macro:: CUDDLCI_GET_MEMREGION_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_info_for_id()``.
 *
 * .. c:macro:: CUDDLCI_GET_EVENTSRC_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_info_for_id()``.
 *
 * .. c:macro:: CUDDLCI_GET_MEMREGION_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_memregion_ref_count_for_id()``.
 *
 * .. c:macro:: CUDDLCI_GET_EVENTSRC_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_eventsrc_ref_count_for_id()``.
 *
 * .. c:macro:: CUDDLCI_DECREMENT_MEMREGION_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_decrement_memregion_info_for_id()``.
 *
 * .. c:macro:: CUDDLCI_DECREMENT_EVENTSRC_REF_COUNT_IOCTL
 *
 *    IOCTL associated with ``cuddl_decrement_eventsrc_info_for_id()``.
 *
 * .. c:macro:: CUDDLCI_JANITOR_REGISTER_PID_IOCTL
 *
 *    IOCTL associated with implicit ``cuddl_startup()`` call.
 *
 * .. c:macro:: CUDDLCI_GET_KERNEL_COMMIT_ID_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_kernel_commit_id()``.
 *
 * .. c:macro:: CUDDLCI_GET_DRIVER_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_get_driver_info()``,
 *    ``cuddl_eventsrc_get_driver_info()``, and
 *    ``cuddl_get_driver_info_for_slot()``.
 *
 * .. c:macro:: CUDDLCI_GET_HW_INFO_IOCTL
 *
 *    IOCTL associated with ``cuddl_memregion_get_hw_info()``,
 *    ``cuddl_eventsrc_get_hw_info()`` and
 *    ``cuddl_get_hw_info_for_slot()``.
 *
 * .. c:macro:: CUDDLCI_GET_KERNEL_VERSION_CODE_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_kernel_version_code()``.
 *
 * .. c:macro:: CUDDLCI_GET_KERNEL_VARIANT_IOCTL
 *
 *    IOCTL associated with ``cuddl_get_kernel_variant()``.
 *
 * .. c:macro:: CUDDLCI_EVENTSRC_IS_ENABLED_IOCTL
 *
 *    IOCTL associated with ``cuddl_eventsrc_is_enabled()``.
 */

/**
 * struct cuddlci_void_ioctl_data - IOCTL struct for calls without data.
 *
 * @version_code: Cuddl version code passed in from user space.
 */
struct cuddlci_void_ioctl_data {
	int version_code;
};

/**
 * struct cuddlci_memregion_claim_ioctl_data - Memory region claim IOCTL data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @id: Resource identifier passed in from user space.
 * @info: Memory region information returned from kernel space.
 * @pid: Process id passed in from user space.
 * @options: Memregion claim options passed in from user space.
 */
struct cuddlci_memregion_claim_ioctl_data {
	int version_code;
	struct cuddl_resource_id id;
	struct cuddl_memregion_info info;
	pid_t pid;
	int options;
};

/**
 * struct cuddlci_eventsrc_claim_ioctl_data - Event source claim IOCTL data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @id: Resource identifier passed in from user space.
 * @info: Event source information returned from kernel space.
 * @pid: Process id passed in from user space.
 * @options: Eventsrc claim options passed in from user space.
 */
struct cuddlci_eventsrc_claim_ioctl_data {
	int version_code;
	struct cuddl_resource_id id;
	struct cuddl_eventsrc_info info;
	pid_t pid;
	int options;
};

/**
 * struct cuddlci_memregion_release_ioctl_data - Memregion release IOCTL data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @token: Token for resource to be released (passed in from user space).
 * @pid: Process id passed in from user space.
 */
struct cuddlci_memregion_release_ioctl_data {
	int version_code;
	struct cuddlci_token token;
	pid_t pid;
};

/**
 * struct cuddlci_eventsrc_release_ioctl_data - Eventsrc release IOCTL data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @token: Token for resource to be released (passed in from user space).
 * @pid: Process id passed in from user space.
 */
struct cuddlci_eventsrc_release_ioctl_data {
	int version_code;
	struct cuddlci_token token;
	pid_t pid;
};

/**
 * struct cuddlci_get_resource_id_ioctl_data - Get event id IOCTL data.
 *
 * Used for both ``cuddl_get_memregion_id_for_slot()`` and
 * ``cuddl_get_eventsrc_id_for_slot()`` implementations.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @device_slot: Manager device slot number to query (input).
 * @resource_slot: Device memregion or eventsrc slot number to query (input).
 * @id: Resource identifier returned from kernel space (output).
 */
struct cuddlci_get_resource_id_ioctl_data {
	int version_code;
	int device_slot;
	int resource_slot;
	struct cuddl_resource_id id;
};

/**
 * struct cuddlci_ref_count_ioctl_data - Get/decr ref count IOCTL data.
 *
 * Used for ``cuddl_get_memregion_ref_count_for_id()``,
 * ``cuddl_get_eventsrc_ref_count_for_id()``,
 * ``cuddl_decrement_memregion_ref_count_for_id()``, and
 * ``cuddl_decrement_eventsrc_ref_count_for_id()`` implementations.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @id: Resource identifier passed in from user space.
 */
struct cuddlci_ref_count_ioctl_data {
	int version_code;
	struct cuddl_resource_id id;
};

/**
 * struct cuddlci_janitor_pid_ioctl_data - Janitor registration data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @pid: Process id passed in from user space.
 */
struct cuddlci_janitor_pid_ioctl_data {
	int version_code;
	pid_t pid;
};

/**
 * struct cuddlci_get_kernel_commit_id_ioctl_data - Kernel commit IOCTL data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @id_str: Kernel commit id string returned from kernel space.
 */
struct cuddlci_get_kernel_commit_id_ioctl_data {
	int version_code;
	char id_str[CUDDLCI_MAX_STR_LEN];
};

/**
 * struct cuddlci_get_driver_info_ioctl_data - Get driver info string data.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @device_slot: Manager device slot number to query (input).
 * @info_str: Driver info string returned from kernel space.
 */
struct cuddlci_get_driver_info_ioctl_data {
	int version_code;
	int device_slot;
	char info_str[CUDDLCI_MAX_STR_LEN];
};

/**
 * struct cuddlci_eventsrc_is_enabled_ioctl_data - IOCTL data for is_enabled.
 *
 * @version_code: Cuddl version code passed in from user space.
 * @token: Token for event source to be queried (passed in from user space).
 */
struct cuddlci_eventsrc_is_enabled_ioctl_data {
	int version_code;
	struct cuddlci_token token;
};

#define CUDDLCI_IOCTL_TYPE 'A'

#define CUDDLCI_MEMREGION_CLAIM_UIO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 2, struct cuddlci_memregion_claim_ioctl_data)
#define CUDDLCI_MEMREGION_CLAIM_UDD_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 3, struct cuddlci_memregion_claim_ioctl_data)
#define CUDDLCI_EVENTSRC_CLAIM_UIO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 4, struct cuddlci_eventsrc_claim_ioctl_data)
#define CUDDLCI_EVENTSRC_CLAIM_UDD_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 5, struct cuddlci_eventsrc_claim_ioctl_data)

#define CUDDLCI_MEMREGION_RELEASE_UIO_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 6, struct cuddlci_memregion_release_ioctl_data)
#define CUDDLCI_MEMREGION_RELEASE_UDD_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 7, struct cuddlci_memregion_release_ioctl_data)
#define CUDDLCI_EVENTSRC_RELEASE_UIO_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 8, struct cuddlci_eventsrc_release_ioctl_data)
#define CUDDLCI_EVENTSRC_RELEASE_UDD_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 9, struct cuddlci_eventsrc_release_ioctl_data)

#define CUDDLCI_GET_MAX_MANAGED_DEVICES_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 10, struct cuddlci_void_ioctl_data)
#define CUDDLCI_GET_MAX_DEV_MEM_REGIONS_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 11, struct cuddlci_void_ioctl_data)
#define CUDDLCI_GET_MAX_DEV_EVENTS_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 12, struct cuddlci_void_ioctl_data)

#define CUDDLCI_GET_MEMREGION_ID_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 13, struct cuddlci_get_resource_id_ioctl_data)
#define CUDDLCI_GET_EVENTSRC_ID_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 14, struct cuddlci_get_resource_id_ioctl_data)

#define CUDDLCI_GET_MEMREGION_INFO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 15, struct cuddlci_memregion_claim_ioctl_data)
#define CUDDLCI_GET_EVENTSRC_INFO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 16, struct cuddlci_eventsrc_claim_ioctl_data)

#define CUDDLCI_GET_MEMREGION_REF_COUNT_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 17, struct cuddlci_ref_count_ioctl_data)
#define CUDDLCI_GET_EVENTSRC_REF_COUNT_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 18, struct cuddlci_ref_count_ioctl_data)

#define CUDDLCI_DECREMENT_MEMREGION_REF_COUNT_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 19, struct cuddlci_ref_count_ioctl_data)
#define CUDDLCI_DECREMENT_EVENTSRC_REF_COUNT_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 20, struct cuddlci_ref_count_ioctl_data)

#define CUDDLCI_JANITOR_REGISTER_PID_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 21, struct cuddlci_janitor_pid_ioctl_data)

#define CUDDLCI_GET_KERNEL_COMMIT_ID_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 22, \
        struct cuddlci_get_kernel_commit_id_ioctl_data)

#define CUDDLCI_GET_DRIVER_INFO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 23, struct cuddlci_get_driver_info_ioctl_data)

#define CUDDLCI_GET_HW_INFO_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 24, struct cuddlci_get_driver_info_ioctl_data)

#define CUDDLCI_GET_KERNEL_VERSION_CODE_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 25, struct cuddlci_void_ioctl_data)

#define CUDDLCI_GET_KERNEL_VARIANT_IOCTL \
  _IOWR(CUDDLCI_IOCTL_TYPE, 26, \
        struct cuddlci_get_kernel_commit_id_ioctl_data)

#define CUDDLCI_EVENTSRC_IS_ENABLED_IOCTL \
  _IOW(CUDDLCI_IOCTL_TYPE, 27, struct cuddlci_eventsrc_is_enabled_ioctl_data)

#endif /* !_CUDDL_COMMON_IMPL_LINUX_IOCTL_H */
