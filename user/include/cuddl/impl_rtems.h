/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space RTEMS decls.
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

#ifndef _CUDDL_IMPL_RTEMS_H
#define _CUDDL_IMPL_RTEMS_H

#include <sys/types.h> /* time_t */
#include <semaphore.h> /* sem_t */

/**
 * DOC: User-space declarations for the RTEMS implementation.
 *
 * These declarations are only available to user-space code.
 */

#ifdef CUDDLC_BUILD_WARN_TARGET
#warning Compiling for RTEMS user space
#endif

/* System type used to represent a time in seconds. */
typedef time_t cuddli_time_t;

/**
 * struct cuddli_memregion_priv - Private memory region data.
 *
 * This data structure is for holding private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 *
 * No extra data members are required for the RTEMS implementation.
 */
struct cuddli_memregion_priv {
};

/**
 * struct cuddli_eventsrc_priv - Private event source data.
 *
 * @sem: Semaphore used for waiting on events.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddli_eventsrc_priv {
	sem_t sem;
};

#endif /* !_CUDDL_IMPL_RTEMS_H */
