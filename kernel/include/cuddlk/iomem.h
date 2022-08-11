/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer I/O memory kernel decls.
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

#ifndef _CUDDLK_IOMEM_H
#define _CUDDLK_IOMEM_H

#include <cuddlk/general.h>

/**
 * DOC: Kernel-space I/O memory access declarations.
 *
 * Cuddl kernel drivers may use this API to read from (or write to) a
 * peripheral's hardware registers in a platform-independent manner.
 *
 * This part of the API is only applicable to kernel-space code.
 */

/**
 * typedef cuddlk_iomem_t - I/O memory accessor for kernel space.
 *
 * Type used to access kernel-space memory-mapped I/O regions.
 *
 * Under Linux and Xenomai, this type is equivalent to ``void __iomem``.
 * Under RTEMS, this type is a raw pointer.
 *
 * Pointers of type ``cuddlk_iomem_t *`` can be used with
 * ``cuddlk_ioread32()``, ``cuddlk_iowrite16()``, etc.
 */
typedef cuddlki_iomem_t cuddlk_iomem_t;

/* Linux definitions are included by kernel_impl_linux.h */
#ifndef CUDDLK_LINUX

/**
 * cuddlk_ioread8() - Read an 8-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint8_t cuddlk_ioread8(cuddlk_iomem_t *addr)
{
	return *(volatile uint8_t *) (addr);
}

/**
 * cuddlk_ioread16() - Read a 16-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint16_t cuddlk_ioread16(cuddlk_iomem_t *addr)
{
	return *(volatile uint16_t *) (addr);
}

/**
 * cuddlk_ioread32() - Read a 32-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
inline uint32_t cuddlk_ioread32(cuddlk_iomem_t *addr)
{
	return *(volatile uint32_t *) (addr);
}

/**
 * cuddlk_iowrite8() - Write an 8-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddlk_iowrite8(uint8_t value, cuddlk_iomem_t *addr)
{
	(*(volatile uint8_t *) (addr)) = value;
}

/**
 * cuddlk_iowrite16() - Write a 16-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddlk_iowrite16(uint16_t value, cuddlk_iomem_t *addr)
{
	(*(volatile uint16_t *) (addr)) = value;
}

/**
 * cuddlk_iowrite32() - Write a 32-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
inline void cuddlk_iowrite32(uint32_t value, cuddlk_iomem_t *addr)
{
	(*(volatile uint32_t *) (addr)) = value;
}

#endif /* CUDDLK_LINUX */

#endif /* !_CUDDLK_IOMEM_H */
