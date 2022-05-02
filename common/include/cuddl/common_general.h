/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common general-purpose decls.
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

#ifndef _CUDDL_COMMON_GENERAL_H
#define _CUDDL_COMMON_GENERAL_H

#include <cuddl/common_impl.h>

/**
 * DOC: Common user/kernel-space general-purpose declarations.
 *
 * This part of the API is technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 *
 * .. c:macro:: CUDDL_MAX_STR_LEN
 *
 *    Maximum allowed length for Cuddl strings.
 */

#define CUDDL_MAX_STR_LEN CUDDL_IMPL_MAX_STR_LEN

/**
 * typedef cuddl_size_t - Type used to represent a count of bytes.
 *
 * This will be equivalent to ``size_t`` on POSIX systems.
 */
typedef cuddl_impl_size_t cuddl_size_t;

/**
 * struct cuddl_resource_id - User-space resource identifier.
 *
 * @group: Name of the group in which the resource's parent device resides.
 *         In some cases, this field is used to indicate the PCI card on
 *         which the parent device is located.
 *
 * @device: Name of the parent device (i.e. peripheral) for a particular
 *          resource.
 *
 * @resource: Name of a particular resource (i.e. specific memory region or
 *            event source) that is associated with a specific (parent)
 *            device.
 *
 * @instance: Used to differentiate between multiple resources that are
 *            registered with identical ``group``, ``device``, and
 *            ``resource`` fields.
 *
 * Specifies a memory region or event source resource by name.  If any field
 * is ``NULL`` or contains an empty string (or ``instance`` is ``0``), the
 * field will be treated as a *don't care* value when searching for a
 * matching resource in the resource list.
 */
struct cuddl_resource_id {
	char group[CUDDL_MAX_STR_LEN];
	char device[CUDDL_MAX_STR_LEN];
	char resource[CUDDL_MAX_STR_LEN];
	int instance;
};

#endif /* !_CUDDL_COMMON_GENERAL_H */
