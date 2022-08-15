/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel version decls.
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

#ifndef _CUDDLK_VERSION_H
#define _CUDDLK_VERSION_H

#include <cuddlk/general.h>

/**
 * DOC: Kernel-space version information declarations.
 *
 * Cuddl kernel drivers may use this API to retrieve information about the
 * version of the source code used to build the Cuddl kernel modules.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * .. c:macro:: CUDDLK_VARIANT
 *
 *    String identifying the kernel implementation.
 *
 *    (e.g. ``"Xenomai UDD"``, ``"Linux UIO"``, ``"RTEMS"``)
 *
 * .. c:macro:: CUDDLK_VERSION_MAJOR
 *
 *    Major portion of the version number.
 *
 * .. c:macro:: CUDDLK_VERSION_MINOR
 *
 *    Minor portion of the version number.
 *
 * .. c:macro:: CUDDLK_REVISION_LEVEL
 *
 *    Revision portion of the version number.
 *
 * .. c:macro:: CUDDLK_VERSION_CODE
 *
 *    32-bit integer version code value that can be used in comparisons.
 *    Similar to ``XENO_VERSION_CODE`` and ``LINUX_VERSION_CODE``.
 */

#define CUDDLK_VARIANT CUDDLKI_VARIANT

#define CUDDLK_VERSION_MAJOR CUDDLCI_VERSION_MAJOR
#define CUDDLK_VERSION_MINOR CUDDLCI_VERSION_MINOR
#define CUDDLK_REVISION_LEVEL CUDDLCI_REVISION_LEVEL

/**
 * CUDDLK_VERSION() - Construct a integer version code.
 *
 * Returns a 32-bit integer version code corresponding to the given major,
 * minor, and revision values that can be used in comparisons.  Similar to
 * ``XENO_VERSION()`` and ``KERNEL_VERSION()``.
 *
 * @major: Major portion of the version number.
 * @minor: Minor portion of the version number.
 * @revision: Revision portion of the version number.
 *
 * Return: Integer version code.
 */
#define CUDDLK_VERSION(major, minor, revision) \
	(((major)<<16) | ((minor)<<8) | (revision))

#define CUDDLK_VERSION_CODE CUDDLK_VERSION(CUDDLK_VERSION_MAJOR, \
                                           CUDDLK_VERSION_MINOR, \
                                           CUDDLK_REVISION_LEVEL)

/**
 * cuddlk_get_commit_id() - Return a string describing the kernel src commit.
 *
 * If the Cuddl kernel modules were built from a clean source tree, then the
 * returned commit id will be a string containing the commit hash (SHA-1
 * checksum), otherwise it will be a string containing the commit hash with
 * an ``M`` prepended to the front of the string (to indicate that the
 * sources have been "modified").
 *
 * @id_str: Pointer to the buffer to be used for returning the commit id
 *          string.
 *
 * @id_len: Input parameter specifying the size of the buffer used for
 *          returning the commit id string.
 *
 * Return: ``0`` (Always succeeds, but result may be truncated.)
 */
int cuddlk_get_commit_id(char *id_str, cuddlk_size_t id_len);

#endif /* !_CUDDLK_VERSION_H */
