/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space Linux decls.
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

#ifndef _CUDDL_IMPL_LINUX_H
#define _CUDDL_IMPL_LINUX_H

#include <sys/types.h> /* time_t, size_t */

/**
 * DOC: User-space declarations for the Linux implementation.
 *
 * These declarations apply to both Linux UIO and Xenomai UDD.  They are only
 * available to user-space code.
 */

#ifdef CUDDL_BUILD_WARN_TARGET
#warning Compiling for Linux user space
#endif

/* System type used to represent a time in seconds. */
typedef time_t cuddl_impl_time_t;

/**
 * struct cuddl_memregion_priv - Private memory region data.
 *
 * @pa_addr: Page-aligned starting address for the memory region mapping, as
 *           returned by ``mmap()``.  This value will be a multiple of
 *           ``CUDDLK_PAGE_SIZE``.  It will be used as the ``addr`` argument
 *           when calling ``munmap()`` to unmap the memory region.
 *
 * @pa_len: Page-aligned length of the memory region. This field represents
 *          the size of the memory region to be mapped, in bytes, and will be
 *          a multiple of ``CUDDLK_PAGE_SIZE``.  This value will be used as
 *          the ``length`` argument when calling ``munmap()`` to unmap the
 *          memory region.
 *
 * @fd: File descriptor used in the ``mmap()`` call to map the memory region.
 *      This file descriptor will be closed via ``close()`` after calling
 *      ``munmap()`` when unmapping the memory region.
 *
 * @token: Opaque token used (internally) when releasing ownership of the
 *         associated memory region.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddl_memregion_priv {
	unsigned long pa_addr;
	size_t pa_len;
	int fd;
	struct cuddl_impl_token token;
};

/**
 * struct cuddl_eventsrc_priv - Private event source data.
 *
 * @fd: File descriptor used in the ``open()`` call when opening the event
 *      source.  This file descriptor will be to wait on events and to
 *      enable/disable interrupt events.  The file descriptor will be closed
 *      via ``close()`` when closing the event source.
 *
 * @token: Opaque token used (internally) when releasing ownership of the
 *         associated event source.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddl_eventsrc_priv {
	int fd;
	struct cuddl_impl_token token;
};

#endif /* !_CUDDL_IMPL_LINUX_H */
