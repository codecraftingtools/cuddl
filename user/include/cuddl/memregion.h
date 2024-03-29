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
 * Memory regions are commonly used to expose device registers to user-space
 * applications for reading and writing.
 *
 * This part of the API is only available to user-space code.
 */

/**
 * struct cuddl_memregion - User-space memory-mapped I/O region accessor.
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
	cuddl_iomem_t *addr;
	cuddl_size_t len;
	int flags;
	struct cuddli_memregion_priv priv;
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
 *          (i.e. hardware peripheral).
 *
 * @memregion: Name of the specific memory region to be claimed.
 *
 * @instance: Used to differentiate between multiple memory regions that are
 *            registered with identical ``group``, ``device``, and
 *            ``resource`` (i.e. ``memregion``) fields.
 *
 * @options: Input parameter specifying the set of flags to apply to this
 *           memory region claim operation.  This field may be a set of
 *           ``cuddl_memregion_claim_flags`` ORed together.
 *
 * Request ownership of a specific memory region for user-space access.  The
 * particular memory region is identified by the ``group``, ``device``,
 * ``memregion``, and ``instance`` arguments.  If any of these arguments is
 * ``NULL`` or contains an empty string (or is ``0``, in the case of
 * ``instance``), the parameter will be treated as a *don't care* value when
 * searching for a matching memory region in the resource list.
 *
 * This routine is automatically called from
 * ``cuddl_memregion_claim_and_map()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-EBUSY``: The specified memory region is already in use.
 *     - ``-ENXIO``: The specified memory region was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - ``-ENOEXEC``: User/kernel major version number mismatch (Linux).
 *     - Value of ``-errno`` resulting from from ``open()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``ioctl()`` call on Cuddl
 *       manager device (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on Cuddl
 *       manager device (Linux).
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
 * This routine is automatically called from
 * ``cuddl_memregion_unmap_and_release()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddli_memregion_release_by_token()``
 *       (Linux).
 */
int cuddl_memregion_release(struct cuddl_memregion_info *meminfo);

/**
 * cuddl_memregion_map() - Map a memory region for user-space access.
 *
 * @memregion: Pointer to a data structure that will receive the information
 *             required to access the specified memory region from user
 *             space.  If the map operation is successful, the required
 *             information will be copied into the data structure specified
 *             by this parameter.
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
 * This routine is automatically called from
 * ``cuddl_memregion_claim_and_map()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Value of ``-errno`` resulting from from ``open()`` call on UIO or
 *       UDD memory region device (Linux).
 *     - Value of ``-errno`` resulting from from ``mmap()`` call on UIO or
 *       UDD memory region file descriptor (Linux).
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
 * This routine is automatically called from
 * ``cuddl_memregion_unmap_and_release()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Value of ``-errno`` resulting from from ``munmap()`` call on UIO or
 *       UDD memory region file descriptor (Linux).
 *     - Value of ``-errno`` resulting from from ``close()`` call on UIO or
 *       UDD memory region device (Linux).
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
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_memregion_claim()`` (Linux).
 *     - Error code returned by ``cuddl_memregion_map()`` (Linux).
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
 *             to ``cuddl_memregion_claim_and_map()``.
 *
 * Unmap the memory region associated with the ``memregion`` memory region
 * accessor and then release ownership of the memory region.  See
 * ``cuddl_memregion_unmap()`` and ``cuddl_memregion_release()`` for more
 * details.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_memregion_unmap()``
 *       (Linux).
 *     - Error code returned by ``cuddli_memregion_release_by_token()``
 *       (Linux).
 */
int cuddl_memregion_unmap_and_release(struct cuddl_memregion *memregion);

/**
 * cuddl_memregion_ioread8() - Read an 8-bit value from a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @offset: I/O memory address offset for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint8_t cuddl_memregion_ioread8(
	struct cuddl_memregion *memregion, cuddl_size_t offset)
{
	return cuddl_ioread8(
		(uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_ioread16() - Read a 16-bit value from a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @offset: I/O memory address offset for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint16_t cuddl_memregion_ioread16(
	struct cuddl_memregion *memregion, cuddl_size_t offset)
{
	return cuddl_ioread16(
		(uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_ioread32() - Read a 32-bit value from a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @offset: I/O memory address offset for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint32_t cuddl_memregion_ioread32(
	struct cuddl_memregion *memregion, cuddl_size_t offset)
{
	return cuddl_ioread32(
		(uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_iowrite8() - Write an 8-bit value to a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @value: Value to be written.
 *
 * @offset: I/O memory address for writing.
 */
inline void cuddl_memregion_iowrite8(
	struct cuddl_memregion *memregion, uint8_t value, cuddl_size_t offset)
{
	cuddl_iowrite8(value, (uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_iowrite16() - Write a 16-bit value to a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @value: Value to be written.
 *
 * @offset: I/O memory address for writing.
 */
inline void cuddl_memregion_iowrite16(
	struct cuddl_memregion *memregion, uint16_t value,
	cuddl_size_t offset)
{
	cuddl_iowrite16(value, (uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_iowrite32() - Write a 32-bit value to a memregion.
 *
 * @memregion: Input identifying the memory region to be read from.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @value: Value to be written.
 *
 * @offset: I/O memory address for writing.
 */
inline void cuddl_memregion_iowrite32(
	struct cuddl_memregion *memregion, uint32_t value,
	cuddl_size_t offset)
{
	cuddl_iowrite32(value, (uint8_t*)(memregion->addr)+offset);
}

/**
 * cuddl_memregion_get_resource_id() - Get the associated resource ID.
 *
 * @memregion: Input identifying the memory region to be queried.  The data
 *             structure pointed to by this parameter should contain the
 *             information returned by a successful call to
 *             ``cuddl_memregion_map()`` or
 *             ``cuddl_memregion_claim_and_map()``.
 *
 * @id: Pointer to a data structure that will receive the resource identifier
 *      information associated with the specified memory region.  On success,
 *      the required information will be copied into the data structure
 *      specified by this parameter.
 *
 * Retrieve the resource identifier associated with this memory region.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Error code returned by ``cuddl_get_memregion_id_for_slot()`` (Linux).
 */
int cuddl_memregion_get_resource_id(
	struct cuddl_memregion *memregion, struct cuddl_resource_id *id);

#endif /* !_CUDDL_MEMREGION_H */
