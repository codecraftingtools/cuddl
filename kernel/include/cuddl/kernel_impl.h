/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel implementation decls.
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

#ifndef _CUDDL_KERNEL_IMPL_H
#define _CUDDL_KERNEL_IMPL_H

#include <cuddl/common_impl.h>

/*
 * Kernel implementation declarations for all targets.
 *
 * These declarations are only available to kernel-space code.
 */

/**
 * DOC: Compile-time options applicable to kernel-space code.
 *
 * These flags may be set by editing *cuddl/kernel_impl.h*, or by adding
 * flags to define the required symbols when invoking the C compiler.  Take
 * care to ensure that all kernel modules (including any Cuddl drivers) are
 * compiled with the same options or some disaster may occur.
 *
 * .. c:macro:: CUDDLK_DISABLE_UDD_ON_XENOMAI
 *
 *    Disables UDD support on a Xenomai system for testing purposes.  Linux
 *    UIO is used instead.
 */
#define CUDDLK_DISABLE_UDD_ON_XENOMAI /* Only UIO is implemented for now */

/* Allow use of Linux UIO configuration on Xenomai systems for testing */
#if defined(CONFIG_XENOMAI) && !defined(CUDDLK_DISABLE_UDD_ON_XENOMAI)
#define CUDDLK_USE_UDD
#endif

#if defined(__rtems__)
#define CUDDLK_RTEMS
#include <cuddl/kernel_impl_rtems.h>

#elif defined(__KERNEL__) /* Linux UIO and Xenomai UDD */
#define CUDDLK_LINUX
#include <cuddl/kernel_impl_linux.h>

#else
#error Kernel variant unsupported
#endif /* defined(__rtems__) */

#endif /* !_CUDDL_KERNEL_IMPL_H */
