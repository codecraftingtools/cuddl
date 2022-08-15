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

/**
 * DOC: User-space implementation declarations for all targets.
 *
 * These declarations are only available to user-space code.
 */

/**
 * cuddli_memregion_release_by_token() - Release memory region using a token.
 *
 * @token: Input parameter identifying the memory region to be released.
 *         Memory region tokens originate with a successful call to to
 *         ``cuddl_memregion_claim()``, and can be obtained from the
 *         ``token`` member of a ``cuddl_memregion_info`` or
 *         ``cuddl_memregion`` structure.
 *
 * Release ownership of the memory region identified by ``token`` from user
 * space.  Typically this function is not called directly, but memory regions
 * are instead released by calling ``cuddl_memregion_release()`` or
 * ``cuddl_memregion_unmap_and_release()``.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENODEV``: The token is invalid.
 *     - ``-EBADSLT``: The token is invalid (Linux).
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       Cuddl manager device.
 */
int cuddli_memregion_release_by_token(struct cuddlci_token token);

/**
 * cuddli_eventsrc_release_by_token() - Release event source using a token.
 *
 * @token: Input parameter identifying the event source to be released.
 *         Event source tokens originate with a successful call to to
 *         ``cuddl_eventsrc_claim()``, and can be obtained from the ``token``
 *         member of a ``cuddl_eventsrc_info`` or ``cuddl_eventsrc``
 *         structure.
 *
 * Release ownership of the event source identified by ``token`` from user
 * space.  Typically this function is not called directly, but event sources
 * are instead released by calling ``cuddl_eventsrc_release()`` or
 * ``cuddl_eventsrc_close_and_release()``.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENODEV``: The token is invalid.
 *     - ``-EBADSLT``: The token is invalid (Linux).
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       Cuddl manager device.
 */
int cuddli_eventsrc_release_by_token(struct cuddlci_token token);

#if defined(CUDDL_RTEMS)
#include <cuddl/impl_rtems.h>
#endif /* defined(CUDDL_RTEMS) */

#if defined(CUDDL_LINUX) /* Including Xenomai */
#include <cuddl/impl_linux.h>
#endif /* defined(CUDDL_LINUX) */

#endif /* !_CUDDL_IMPL_H */
