/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space impl declarations.
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

#ifndef _CUDDL_IMPL_H
#define _CUDDL_IMPL_H

#include <cuddl/common_impl.h>

/*
 * User-space implementation declarations for all targets.
 *
 * These declarations are only available to user-space code.
 */

#if defined(CUDDL_RTEMS)
#include <cuddl/impl_rtems.h>
#endif /* defined(CUDDL_RTEMS) */

#if defined(CUDDL_LINUX) /* Including Xenomai */
#include <cuddl/impl_linux.h>
#endif /* defined(CUDDL_LINUX) */

#endif /* !_CUDDL_IMPL_H */
