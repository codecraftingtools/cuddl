/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space version decls.
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

#ifndef _CUDDL_VERSION_H
#define _CUDDL_VERSION_H

#include <cuddl/common_general.h>
#include <cuddl/impl.h>

/**
 * DOC: User-space version information declarations.
 *
 * User-space applications may use this API to retrieve information about the
 * version of the source code used to build the Cuddl kernel modules and
 * user-space interface code.
 *
 * This part of the API is only applicable to user-space code.
 *
 * .. c:macro:: CUDDL_VERSION_MAJOR
 *
 *    Major portion of the version number.
 *
 * .. c:macro:: CUDDL_VERSION_MINOR
 *
 *    Minor portion of the version number.
 *
 * .. c:macro:: CUDDL_REVISION_LEVEL
 *
 *    Revision portion of the version number.
 *
 * .. c:macro:: CUDDL_VERSION_CODE
 *
 *    32-bit integer version code value that can be used in comparisons.
 *    Similar to ``XENO_VERSION_CODE`` and ``LINUX_VERSION_CODE``.
 */

#define CUDDL_VERSION_MAJOR CUDDLCI_VERSION_MAJOR
#define CUDDL_VERSION_MINOR CUDDLCI_VERSION_MINOR
#define CUDDL_REVISION_LEVEL CUDDLCI_REVISION_LEVEL

/**
 * CUDDL_VERSION() - Construct an integer version code.
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
#define CUDDL_VERSION(major, minor, revision) \
	CUDDLCI_VERSION(major, minor, revision)

#define CUDDL_VERSION_CODE CUDDL_VERSION(CUDDL_VERSION_MAJOR, \
                                         CUDDL_VERSION_MINOR, \
                                         CUDDL_REVISION_LEVEL)

/**
 * CUDDL_MAJOR_VERSION_FROM_CODE() - Extract the major version from a code.
 *
 * Extracts the major version portion of a 32-bit integer version code.
 *
 * @code: 32-bit integer version code
 *
 * Return: Major portion of the version number.
 */
#define CUDDL_MAJOR_VERSION_FROM_CODE(code) \
	CUDDLCI_MAJOR_VERSION_FROM_CODE(code)

/**
 * CUDDL_MINOR_VERSION_FROM_CODE() - Extract the minor version from a code.
 *
 * Extracts the minor version portion of a 32-bit integer version code.
 *
 * @code: 32-bit integer version code
 *
 * Return: Minor portion of the version number.
 */
#define CUDDL_MINOR_VERSION_FROM_CODE(code) \
	CUDDLCI_MINOR_VERSION_FROM_CODE(code)

/**
 * CUDDL_REVISION_LEVEL_FROM_CODE() - Extract the revision level from a code.
 *
 * Extracts the revision level portion of a 32-bit integer version code.
 *
 * @code: 32-bit integer version code
 *
 * Return: Revision level portion of the version number.
 */
#define CUDDL_REVISION_LEVEL_FROM_CODE(code) \
	CUDDLCI_REVISION_LEVEL_FROM_CODE(code)

/**
 * cuddl_get_kernel_version_code() - Return the kernel version code.
 *
 * Retrieve the ``CUDDLK_VERSION_CODE`` from the kernel.
 *
 * Return: The kernel version code on success, or a negative error code.
 *
 *   Error codes:
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
int cuddl_get_kernel_version_code();

/**
 * cuddl_get_kernel_commit_id() - Return the kernel commit id string.
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
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
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
int cuddl_get_kernel_commit_id(char *id_str, cuddl_size_t id_len);

/**
 * cuddl_get_userspace_commit_id() - Return the user-space commit id string.
 *
 * If the Cuddl user-space interface code was built from a clean source tree,
 * then the returned commit id will be a string containing the commit hash
 * (SHA-1 checksum), otherwise it will be a string containing the commit hash
 * with an ``M`` prepended to the front of the string (to indicate that the
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
int cuddl_get_userspace_commit_id(char *id_str, cuddl_size_t id_len);

/**
 * cuddl_get_kernel_variant() - Retrieve the kernel variant description.
 *
 * Retrieve the ``CUDDLK_VARIANT`` string from the kernel.  This string
 * identifies the kernel implementation (e.g. ``"Xenomai UDD"``, ``"Linux
 * UIO"``, ``"RTEMS"``).
 *
 * @str: Pointer to the buffer to be used for returning the variant string.
 *
 * @len: Input parameter specifying the size of the buffer used for returning
 *       the variant string.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
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
int cuddl_get_kernel_variant(char *str, cuddl_size_t len);

#endif /* !_CUDDL_VERSION_H */
