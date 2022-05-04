/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common compilation options.
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

#ifndef _CUDDL_COMMON_COMPILATION_OPTS_H
#define _CUDDL_COMMON_COMPILATION_OPTS_H

/**
 * DOC: Compile-time options applicable to both kernel and user-space code.
 *
 * These flags may be set by editing *cuddl/common_compilation_opts.h*, or by
 * adding flags to define the required symbols when invoking the C compiler.
 * Take care to ensure that all kernel and user-space code is compiled with
 * the same options or some disaster may possibly occur.
 *
 * .. c:macro:: CUDDL_BUILD_WARN_TARGET
 *
 *    If this symbol is defined, pre-processor ``#warning`` output will be
 *    enabled to indicate the type of target system detected at build time.
 *    This may be useful for debugging purposes.
 */
//#define CUDDL_BUILD_WARN_TARGET

#endif /* !_CUDDL_COMMON_COMPILATION_OPTS_H */
