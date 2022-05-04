/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space mem region decls.
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

#ifndef _CUDDL_MEMREGION_H
#define _CUDDL_MEMREGION_H

#include <cuddl/common_memregion.h>
#include <cuddl/iomem.h>

/**
 * DOC: User-space memory region declarations.
 *
 * This part of the API is only available to user-space code.
 */

/**
 * struct cuddl_memregion - User-space memory-mapped I/O region accessor.
 *
 * @token: Opaque token used (internally) when releasing ownership of the
 *         associated memory region.
 *
 * @addr: Pointer to the starting address of the memory-mapped I/O region.
 *        This value is not necessarily ``CUDDLK_PAGE_SIZE``-aligned
 *
 * @len: The exact size of the memory region to be mapped, in bytes.  This is
 *       not necessarily a multiple of ``CUDDLK_PAGE_SIZE``.
 *
 * @flags: Flags that describe the properties of the memory region.  This
 *         field may be a set of ``cuddl_memregion_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Provides user-space applications access to a memory-mapped device I/O
 * region.
 */
struct cuddl_memregion {
	cuddl_token_t token;
	cuddl_iomem_t *addr;
	cuddl_size_t len;
	int flags;
	struct cuddl_memregion_priv priv;
};

/**
 * cuddl_memregion_claim() - Claim a memory region from user space.
 *
 * @meminfo: Pointer to a data structure that will receive the information
 *           required to map the specified memory region from user space.  If
 *           the claim operation is successful, the required information will
 *           be copied into the data structure specified by this parameter.
 *
 * @group: Name of the group in which the specified memory region's parent
 *         device resides.  In some cases, the ``group`` is used to indicate
 *         the PCI card on which the parent device is located.
 *
 * @device: Name of the specified memory region's parent device
 *          (i.e. peripheral).
 *
 * @memregion: Name of the specific memory region to be claimed.
 *
 * @instance: Used to differentiate between multiple memory regions that are
 *            registered with identical ``group``, ``device``, and
 *            ``resource`` (i.e. ``memregion``) fields.
 *
 * @options: Input parameter consisting of a set of flags (ORed together)
 *           that are applicable to the memory region claim operation.  No
 *           claim flags are currently defined for memory regions, so this
 *           parameter should be set to ``0``.
 *
 * Request ownership of a specific memory region for user-space access.  The
 * particular memory region is identified by the ``group``, ``device``,
 * ``memregion``, and ``instance`` arguments.  If any of these arguments is
 * ``NULL`` or contains an empty string (or ``instance`` is ``0``), the
 * parameter will be treated as a *don't care* value when searching for a
 * matching memory region in the resource list.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_claim(
	struct cuddl_memregion_info *meminfo,
	const char *group,
	const char *device,
	const char *memregion,
	int instance,
	int options);

/**
 * cuddl_memregion_release() - Release a memory region from user space.
 *
 * @meminfo: Input parameter identifying the memory region to be released.
 *           The data structure pointed to by this parameter should contain
 *           the information returned by a successful call to
 *           ``cuddl_memregion_claim()``.
 *
 * Release ownership of the memory region identified by ``meminfo``.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_release(struct cuddl_memregion_info *meminfo);

/**
 * cuddl_memregion_map() - Map a memory region for user-space access.
 *
 * @memregion: Pointer to a data structure that will receive the information
 *             required to access the specified memory region from user
 *             space.  If the map operation is successful, and if this
 *             parameter is non-``NULL``, the required information will be
 *             copied into the data structure specified by this parameter.
 *
 * @meminfo: Input parameter identifying the memory region to be mapped.  The
 *           data structure pointed to by this parameter should contain the
 *           information returned by a successful call to
 *           ``cuddl_memregion_claim()``.
 *
 * @options: Input parameter consisting of a set of flags (ORed together)
 *           that are applicable to the memory region map operation.  No map
 *           flags are currently defined for memory regions, so this
 *           parameter should be set to ``0``.
 *
 * Map the memory region identified by ``meminfo`` for user-space access.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_map(
	struct cuddl_memregion *memregion,
	const struct cuddl_memregion_info *meminfo,
	int options);

/**
 * cuddl_memregion_unmap() - Unmap a memory region from user space.
 *
 * @memregion: Input parameter identifying the memory region accessor to be
 *             unmapped.  The data structure pointed to by this parameter
 *             should contain the information returned by a successful call
 *             to ``cuddl_memregion_map()``.
 *
 * Unmap the memory region associated with the ``memregion`` memory region
 * accessor.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_unmap(struct cuddl_memregion *memregion);

/**
 * cuddl_memregion_claim_and_map() - Claim and map a memory region.
 *
 * @memregion: See ``cuddl_memregion_map()``.
 * @group: See ``cuddl_memregion_claim()``.
 * @device: See ``cuddl_memregion_claim()``.
 * @resource: Name of the specific memory region to be claimed.
 * @instance: See ``cuddl_memregion_claim()``.
 * @claim_options: See ``cuddl_memregion_claim()``.
 * @map_options: See ``cuddl_memregion_map()``.
 *
 * Request ownership of a specific memory region and then map it for
 * user-space access.  See ``cuddl_memregion_claim()`` and
 * ``cuddl_memregion_map()`` for more details.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_claim_and_map(
	struct cuddl_memregion *memregion,
	const char *group,
	const char *device,
	const char *resource,
	int instance,
	int claim_options,
	int map_options);

/**
 * cuddl_memregion_unmap_and_release() - Unmap and release a memory region.
 *
 * @memregion: Input identifying the memory region to be unmapped and
 *             released.  The data structure pointed to by this parameter
 *             should contain the information returned by a successful call
 *             to ``cuddl_memregion_map()``.
 *
 * Unmap the memory region associated with the ``memregion`` memory region
 * accessor and then release ownership of the memory region.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_memregion_unmap_and_release(struct cuddl_memregion *memregion);

/**
 * cuddl_memregion_release_by_token() - Release memory region using a token.
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
 */
int cuddl_memregion_release_by_token(cuddl_token_t token);

#endif /* !_CUDDL_MEMREGION_H */
