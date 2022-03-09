/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel device manager decls.
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

#ifndef _CUDDL_KERNEL_MANAGER_H
#define _CUDDL_KERNEL_MANAGER_H

#include <cuddl/kernel_device.h>

/**
 * DOC: Kernel-space device manager declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 */

/**
 * struct cuddlk_manager - Cuddl device manager data structure.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Keeps track of all managed Cuddl devices, their memory regions, and their
 * event sources.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_manager {
	struct cuddlk_manager_priv priv;
};

/**
 * cuddlk_manager_add() - Manage a Cuddl device.
 *
 * @dev: Cuddl device to manage.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_manager_add(struct cuddlk_device *dev);

/**
 * cuddlk_manager_remove() - Release a managed Cuddl device.
 *
 * @dev: Managed Cuddl device to release.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_manager_remove(struct cuddlk_device *dev);

#endif /* !_CUDDL_KERNEL_MANAGER_H */
