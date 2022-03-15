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
 * These declarations apply to both Linux UIO and Xenomai UDD.
 *
 * These declarations are technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 *
 * .. c:macro:: CUDDL_MEMREGION_CLAIM_IOCTL
 *
 *    IOCTL call associated with ``cuddl_memregion_claim()``.
 */

/**
 * struct cuddl_memregion_claim_ioctl_data - Memregion claim IOCTL data.
 *
 * @id: Resource identifier passed in from user space.
 * @info: Memory region information returned from kernel space.
 */
struct cuddl_memregion_claim_ioctl_data {
	struct cuddl_resource_id id;
	struct cuddl_meminfo info;
};

#define CUDDL_IOCTL_TYPE 'A'
#define CUDDL_MEMREGION_CLAIM_IOCTL \
  _IOWR(CUDDL_IOCTL_TYPE, 2, struct cuddl_memregion_claim_ioctl_data)

#endif /* !_CUDDL_COMMON_IMPL_LINUX_IOCTL_H */
