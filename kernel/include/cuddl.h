/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel declarations.
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

#ifndef _CUDDL_H
#define _CUDDL_H

#include <linux/kernel.h>
#include <cuddlk.h>

/*
 * Kernel-space declarations with typedefs for user-space compatibility.
 *
 * This header may be used when writing code that is used in both kernel and
 * user space.  As a general rule, using ``cuddlk.h`` is preferred when
 * writing kernel-only code.
 */

#define printf cuddlk_print
#define cuddl_iomem_t cuddlk_iomem_t
#define cuddl_ioread8 cuddlk_ioread8
#define cuddl_ioread16 cuddlk_ioread16
#define cuddl_ioread32 cuddlk_ioread32
#define cuddl_iowrite8 cuddlk_iowrite8
#define cuddl_iowrite16 cuddlk_iowrite16
#define cuddl_iowrite32 cuddlk_iowrite32

#endif /* !_CUDDL_H */
