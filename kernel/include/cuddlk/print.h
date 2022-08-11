/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel printing decls.
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

#ifndef _CUDDLK_PRINT_H
#define _CUDDLK_PRINT_H

/**
 * DOC: Kernel-space printing declarations.
 *
 * Cuddl kernel drivers may use this API to print information information in
 * a platform-independent manner.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:function:: int cuddlk_print(...)
 *
 *    Platform-specific kernel ``printf()`` implementation.
 *
 *    This maps to ``printk()`` on standard Linux, ``rtdm_printk()`` on
 *    Xenomai, and ``printf()`` on RTEMS.
 *
 * .. c:function:: int cuddlk_debug(...)
 *
 *    Print from kernel if debugging output is enabled.
 *
 *    This ``printf``-style function generates output if and only if
 *    ``CUDDLK_ENABLE_DEBUG_PRINT`` is enabled at compile time.
 *
 * .. c:function:: int cuddlk_idebug(...)
 *
 *    Print from kernel if intrusive debugging output is enabled.
 *
 *    This ``printf``-style function is for printing debugging information in
 *    contexts that are likely to affect performance.  It only generates
 *    output if and only if ``CUDDLK_ENABLE_INTRUSIVE_DEBUG_PRINT`` is
 *    enabled at compile time.
 */

#define cuddlk_print(...) cuddlk_impl_print(__VA_ARGS__)

#ifdef CUDDLK_ENABLE_DEBUG_PRINT
  #define cuddlk_debug(...) cuddlk_impl_print(__VA_ARGS__)
#else
  #define cuddlk_debug(...)
#endif

#ifdef CUDDLK_ENABLE_INTRUSIVE_DEBUG_PRINT
  #define cuddlk_idebug(...) cuddlk_impl_print(__VA_ARGS__)
#else
  #define cuddlk_idebug(...)
#endif

#endif /* !_CUDDLK_PRINT_H */
