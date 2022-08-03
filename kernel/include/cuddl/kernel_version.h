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

#ifndef _CUDDL_KERNEL_VERSION_H
#define _CUDDL_KERNEL_VERSION_H

#include <cuddl/kernel_general.h>

/**
 * DOC: Kernel-space version information declarations.
 *
 * Cuddl kernel drivers may use this API to retrieve information about the
 * version of the source code used to build the Cuddl kernel modules.
 *
 * This part of the API is only applicable to kernel-space code.
 */

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
 * Return: ``0`` (Always succeeds, but may be truncated.)
 */
int cuddlk_get_commit_id(char *id_str, cuddlk_size_t id_len);

#endif /* !_CUDDL_KERNEL_VERSION_H */
