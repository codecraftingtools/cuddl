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
 * Memory regions are commonly used to expose device registers to user-space
 * applications for reading and writing.
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
 * struct cuddl_memregion_info - Memory region information for user space.
 *
 * @len: The exact size of the memory region to be mapped, in bytes.  This is
 *       not necessarily a multiple of ``CUDDLK_PAGE_SIZE``.
 *
 * @flags: Flags that describe the properties of the memory region.  This
 *         field may be a set of ``cuddl_memregion_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Memory region information that is exported to user-space code.  Typically,
 * the internal members of this structure do not need to be directly accessed
 * from user-space applications.
 *
 * On Linux and Xenomai systems, this information is retrieved from the
 * kernel via an ``ioctl`` call.
 */
struct cuddl_memregion_info {
	cuddl_size_t len;
	int flags;
	struct cuddlci_memregion_info_priv priv;
};

#endif /* !_CUDDL_COMMON_MEMREGION_H */
