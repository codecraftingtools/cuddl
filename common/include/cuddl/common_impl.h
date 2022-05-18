/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common implementation decls.
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

#ifndef _CUDDL_COMMON_IMPL_H
#define _CUDDL_COMMON_IMPL_H

#include <cuddl/common_compilation_opts.h>

/**
 * DOC: Common user/kernel-space implementation declarations for all targets.
 *
 * These declarations are available to both user-space and kernel-space code.
 *
 * .. c:macro:: CUDDL_IMPL_MAX_STR_LEN
 *
 *    Maximum allowed length for Cuddl string variables.  (Equivalent to
 *    ``CUDDL_MAX_STR_LEN`` / ``CUDDLK_MAX_STR_LEN``.)
 */

/* Maximum allowed length for Cuddl strings. */
#define CUDDL_IMPL_MAX_STR_LEN 128

/**
 * struct cuddl_impl_token - User-space handle to a memregion or eventsrc.
 *
 * @device_index: Index of device in device manager array.
 * @resource_index: Index of memregion or eventsrc in device's array.
 *
 * Opaque token used internally when claiming and releasing memory regions
 * and event sources.
 */
struct cuddl_impl_token {
	int device_index;
	int resource_index;
};

#if defined(__rtems__)
#define CUDDL_RTEMS
#include <cuddl/common_impl_rtems.h>

#else /* Linux (including Xenomai) */
#define CUDDL_LINUX
#include <cuddl/common_impl_linux.h>
#endif /* defined(__rtems__) */

#endif /* !_CUDDL_COMMON_IMPL_H */
