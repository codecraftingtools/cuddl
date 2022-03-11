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

#ifndef _CUDDL_KERNEL_GENERAL_H
#define _CUDDL_KERNEL_GENERAL_H

#include <cuddl/common_general.h>
#include <cuddl/kernel_impl.h>

/**
 * DOC: General-purpose kernel-space declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:macro:: CUDDLK_MAX_STR_LEN
 *
 *    Maximum allowed length for Cuddl kernel strings.
 *
 *    In theory, this could have a different value than
 *    ``CUDDL_MAX_STR_LEN``, but we would rather keep our sanity.  For
 *    consistency in notation, however, ``CUDDLK_MAX_STR_LEN`` is used in
 *    kernel code.
 *
 * .. c:macro:: CUDDLK_VARIANT_STR
 *
 *    String identifying the kernel implementation.
 */

#define CUDDLK_MAX_STR_LEN CUDDL_MAX_STR_LEN
#define CUDDLK_VARIANT_STR CUDDLK_IMPL_VARIANT_STR

#endif /* !_CUDDL_KERNEL_GENERAL_H */
