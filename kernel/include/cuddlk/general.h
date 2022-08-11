/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer general-purpose kernel decls.
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

#ifndef _CUDDLK_GENERAL_H
#define _CUDDLK_GENERAL_H

#include <cuddl/common_general.h>
#include <cuddlk/impl.h>

/**
 * DOC: General-purpose kernel-space declarations.
 *
 * These basic declarations are available to Cuddl kernel drivers.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:macro:: CUDDLK_MAX_STR_LEN
 *
 *    Maximum allowed length for Cuddl kernel string variables.
 *
 *    This is equivalent to ``CUDDL_MAX_STR_LEN``, but ``CUDDLK_MAX_STR_LEN``
 *    is used in kernel code to keep the notation consistent.
 *
 * .. c:macro:: CUDDLK_VARIANT_STR
 *
 *    String identifying the kernel implementation.
 *
 *    (e.g. ``"Xenomai UDD"``, ``"Linux UIO"``, ``"RTEMS"``)
 */

#define CUDDLK_MAX_STR_LEN CUDDL_IMPL_MAX_STR_LEN
#define CUDDLK_VARIANT_STR CUDDLK_IMPL_VARIANT_STR

/**
 * typedef cuddlk_size_t - Kernel type used to represent a count of bytes.
 *
 * This will be equivalent to ``size_t`` on POSIX systems.  This is also
 * equivalent to ``cuddl_size_t``, but ``cuddlk_size_t`` is used in kernel
 * code to keep the notation consistent.
 */
typedef cuddl_impl_size_t cuddlk_size_t;

#endif /* !_CUDDLK_GENERAL_H */
