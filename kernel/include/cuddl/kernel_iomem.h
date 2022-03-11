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

#ifndef _CUDDL_KERNEL_IOMEM_H
#define _CUDDL_KERNEL_IOMEM_H

#include <cuddl/kernel_general.h>

/**
 * DOC: Kernel-space I/O memory access declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 */

/**
 * typedef cuddlk_iomem_t - I/O memory accessor for kernel space.
 *
 * Type used to access kernel-space memory-mapped I/O regions.
 *
 * Pointers of type ``cuddlk_iomem_t *`` can be passed to
 * ``cuddlk_ioread32()``, ``cuddlk_iowrite16()``, etc.
 */
typedef cuddlk_impl_iomem_t cuddlk_iomem_t;

/* Linux definitions are included by kernel_impl_linux.h */
#ifndef CUDDLK_LINUX

/**
 * cuddlk_ioread_8() - Read an 8-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
uint8_t   cuddlk_ioread8(cuddlk_iomem_t *addr);

/**
 * cuddlk_ioread_16() - Read a 16-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
uint16_t cuddlk_ioread16(cuddlk_iomem_t *addr);

/**
 * cuddlk_ioread_32() - Read a 32-bit value from device I/O memory.
 *
 * @addr: I/O memory address for reading.
 *
 * Return: Value that results from reading the specified memory address.
 */
uint32_t cuddlk_ioread32(cuddlk_iomem_t *addr);

/**
 * cuddlk_iowrite_8() - Write an 8-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
void cuddlk_iowrite8(uint8_t value, cuddlk_iomem_t *addr);

/**
 * cuddlk_iowrite_16() - Write a 16-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
void cuddlk_iowrite16(uint16_t value, cuddlk_iomem_t *addr);

/**
 * cuddlk_iowrite_32() - Write a 32-bit value to device I/O memory.
 *
 * @value: Value to be written.
 * @addr: I/O memory address for writing.
 */
void cuddlk_iowrite32(uint32_t value, cuddlk_iomem_t *addr);

#endif /* CUDDLK_LINUX */

#endif /* !_CUDDL_KERNEL_IOMEM_H */
