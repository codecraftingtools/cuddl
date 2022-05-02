/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common RTEMS declarations.
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

#ifndef _CUDDL_COMMON_IMPL_RTEMS_H
#define _CUDDL_COMMON_IMPL_RTEMS_H

#include <sys/types.h> /* size_t */
#include <semaphore.h> /* sem_t */

/**
 * DOC: Common user/kernel-space declarations for the RTEMS implementation.
 *
 * These declarations are technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 */

#ifdef CUDDL_BUILD_WARN_TARGET
#warning Compiling for RTEMS user/kernel space
#endif

/* System type used to represent a count of bytes. */
typedef size_t cuddl_impl_size_t;

/**
 * struct cuddl_memregion_info_priv - Private memory region information.
 *
 * @pa_addr: Page-aligned starting address for the memory region to be
 *           mapped.  This value will be a multiple of the system
 *           ``CUDDLK_PAGE_SIZE``.
 *
 * This data structure contains private, and likely platform-specific, data
 * members reserved for internal use by the Cuddl implementation.
 */
struct cuddl_memregion_info_priv {
	unsigned long pa_addr;
};

/**
 * struct cuddl_eventsrc_info_priv - Private event source information.
 *
 * @sem: Semaphore used for waiting on events.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddl_eventsrc_info_priv {
	sem_t sem;
};

#endif /* !_CUDDL_COMMON_IMPL_RTEMS_H */
