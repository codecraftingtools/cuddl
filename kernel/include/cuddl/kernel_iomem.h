/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer I/O memory kernel decls.
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

#ifndef _CUDDL_KERNEL_IOMEM_H
#define _CUDDL_KERNEL_IOMEM_H

#include <cuddl/kernel_general.h>

/**
 * DOC: Kernel-space I/O memory access declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 */

/**
 * typedef cuddlk_iomem_t - I/O memory accessor for kernel space.
 *
 * Type used to access kernel-space memory-mapped I/O regions.
 *
 * Under Linux, pointers of type ``cuddlk_iomem_t *`` can be passed to
 * ``ioread32()``, ``iowrite16()``, etc.
 */
typedef cuddlk_impl_iomem_t cuddlk_iomem_t;

#endif /* !_CUDDL_KERNEL_IOMEM_H */
