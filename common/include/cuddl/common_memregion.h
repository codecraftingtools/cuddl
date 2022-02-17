/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common mem region decls.
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

#ifndef _CUDDL_COMMON_MEMREGION_H
#define _CUDDL_COMMON_MEMREGION_H

#include <cuddl/common_general.h>

/**
 * DOC: Common user/kernel-space memory region declarations.
 *
 * This part of the API is technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 */

/** 
 * enum cuddl_memregion_flags - Memory region flags for user space.
 *
 * @CUDDL_MEMF_SHARED: Indicates that the associated memory region may be
 *                     claimed by more than one user-space application
 *                     simultaneously.
 *
 * Flags that describe the properties of a memory region to user-space code.
 */
enum cuddl_memregion_flags {
	CUDDL_MEMF_SHARED = (1 << 0),
};

/**
 * struct cuddl_meminfo - Memory region information for user space.
 *
 * @pa_len: Page-aligned length of the memory region. This field represents
 *          the size of the memory region to be mapped, in bytes.  This value
 *          will be a multiple of the system ``PAGE_SIZE``.
 *
 *          On Linux and Xenomai systems, this value will be used as the
 *          ``length`` argument when mapping this memory region via the POSIX
 *          ``mmap()`` system call on the device node specified in the
 *          ``priv`` data structure.
 *
 *          Under RTEMS, the page-aligned length field will be set, but is
 *          not typically used.
 *
 * @start_offset: Starting offset of the memory region to be mapped, in
 *                bytes.  This value does NOT need to be
 *                ``PAGE_SIZE``-aligned, so it is used when defining
 *                non-``PAGE_SIZE``-aligned memory regions.
 *
 *                On Linux and Xenomai systems, the start offset is relative
 *                to the page-aligned address returned by the ``mmap()``
 *                system call.
 *
 *                Under RTEMS, this value is added to the page-aligned
 *                address (``pa_addr``) specified in the ``priv`` data
 *                structure to determine the precise starting address for the
 *                memory region.
 *
 * @len: The exact size of the memory region to be mapped, in bytes.  This
 *       value does NOT need to be a multiple of the system ``PAGE_SIZE``.
 *
 * @flags: Flags that describe the properties of the memory region.  This
 *         field may be a set of ``cuddl_memregion_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Memory region information that is exported to user-space code.
 *
 * On Linux and Xenomai systems, this information is retrieved from the
 * kernel via an ``ioctl`` call.
 */
struct cuddl_meminfo {
	cuddl_size_t pa_len;
	cuddl_size_t start_offset;
	cuddl_size_t len;
	int flags;
	struct cuddl_meminfo_priv priv;
};

#endif /* !_CUDDL_COMMON_MEMREGION_H */
