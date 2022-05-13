/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel compilation options.
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

#ifndef _CUDDL_KERNEL_COMPILATION_OPTS_H
#define _CUDDL_KERNEL_COMPILATION_OPTS_H

/**
 * DOC: Compile-time options applicable to kernel-space code.
 *
 * These flags may be set by editing *cuddl/kernel_compilation_opts.h*, or by
 * adding flags to define the required symbols when invoking the C compiler.
 * Take care to ensure that all kernel modules (including any Cuddl drivers)
 * are compiled with the same options or some disaster may possibly occur.
 *
 * .. c:macro:: CUDDLK_DISABLE_UDD_ON_XENOMAI
 *
 *    Disables UDD support on a Xenomai system for testing purposes.  Linux
 *    UIO is used instead.
 *
 * .. c:macro:: CUDDLK_ENABLE_DEBUG_PRINT
 *
 *    Enables standard debug print statements.
 *
 * .. c:macro:: CUDDLK_ENABLE_INTRUSIVE_DEBUG_PRINT
 *
 *    Enables intrusive debug print statements.
 *
 *    Enabling this option is likely to affect performance.
 */
//#define CUDDLK_DISABLE_UDD_ON_XENOMAI /* For testing purposes */
//#define CUDDLK_ENABLE_DEBUG_PRINT
//#define CUDDLK_ENABLE_INTRUSIVE_DEBUG_PRINT

#endif /* !_CUDDL_KERNEL_COMPILATION_OPTS_H */
