/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer I/O memory user-space decls.
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

#ifndef _CUDDL_IOMEM_H
#define _CUDDL_IOMEM_H

#include <stdint.h>
#include <cuddl/impl.h>

/**
 * DOC: User-space I/O memory access declarations.
 *
 * User-space applications may use this API to read from (or write to) a
 * peripheral's hardware registers in a platform-independent manner.
 *
 * This part of the API is only applicable to user-space code.
 */

/**
 * typedef cuddl_iomem_t - I/O memory accessor for user space.
 *
 * Data type used to access user-space memory-mapped I/O regions.
 *
 * Pointers of type ``cuddl_iomem_t *`` are typically obtained from the
 * ``addr`` field of a ``cuddl_memregion`` struct returned by
 * ``cuddl_memregion_map()`` or ``cuddl_memregion_claim_and_map()``.  These
 * pointers can then be used with ``cuddl_ioread32()``,
 * ``cuddl_iowrite16()``, etc.
 */
typedef void cuddl_iomem_t;

/**
 * cuddl_ioread8() - Read an 8-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint8_t cuddl_ioread8(cuddl_iomem_t *addr)
{
	return *(volatile uint8_t *) (addr);
}

/**
 * cuddl_ioread16() - Read a 16-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint16_t cuddl_ioread16(cuddl_iomem_t *addr)
{
	return *(volatile uint16_t *) (addr);
}

/**
 * cuddl_ioread32() - Read a 32-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint32_t cuddl_ioread32(cuddl_iomem_t *addr)
{
	return *(volatile uint32_t *) (addr);
}

/**
 * cuddl_iowrite8() - Write an 8-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddl_iowrite8(uint8_t value, cuddl_iomem_t *addr)
{
	(*(volatile uint8_t *) (addr)) = value;
}

/**
 * cuddl_iowrite16() - Write a 16-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddl_iowrite16(uint16_t value, cuddl_iomem_t *addr)
{
	(*(volatile uint16_t *) (addr)) = value;
}

/**
 * cuddl_iowrite32() - Write a 32-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddl_iowrite32(uint32_t value, cuddl_iomem_t *addr)
{
	(*(volatile uint32_t *) (addr)) = value;
}

#endif /* !_CUDDL_IOMEM_H */
