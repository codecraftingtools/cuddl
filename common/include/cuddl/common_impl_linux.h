/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common Linux declarations.
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

#ifndef _CUDDL_COMMON_IMPL_LINUX_H
#define _CUDDL_COMMON_IMPL_LINUX_H

#ifdef __KERNEL__
  #include <linux/types.h> /* size_t */
#else
  #include <sys/types.h> /* size_t */
#endif

/**
 * DOC: Common user/kernel-space declarations for the Linux implementation.
 *
 * These declarations apply to both Linux UIO and Xenomai UDD.
 *
 * These declarations are technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 */

#ifdef CUDDL_BUILD_WARN_TARGET
#warning Compiling for Linux user/kernel space
#endif

/* System type used to represent a count of bytes. */
typedef size_t cuddl_impl_size_t;

/**
 * struct cuddl_meminfo_priv - Private memory region information.
 *
 * @pa_mmap_offset: Page-aligned mmap offset.
 *
 *     Value to be used as the ``offset`` argument when mapping this memory
 *     region via the ``mmap()`` system call on the device named
 *     ``device_name``.  This value is expressed in bytes, and will be a
 *     multiple of ``PAGE_SIZE``.
 *
 *     Under Linux UIO, this value will be ``N * getpagesize()``, where ``N``
 *     is the memory region number.
 *
 *     This value will always be zero under Xenomai UDD (because a separate
 *     device node is created for each memory region), but should still be
 *     used for portability reasons.
 *
 * @device_name:
 *
 *     Name of the device node that will be used to map the memory region via
 *     the ``mmap()`` system call.
 *
 *     Under Xenomai UDD, the value of this field will be something like::
 *      
 *       /dev/rtdm/mydevname,mapper0
 *      
 *     where ``0`` indicates the memory region number and ``mydevname``
 *     indicates the name of the associated Cuddl device.
 *      
 *     Under Linux UIO, the value of this field will be something like::
 *      
 *       /dev/uio0
 *      
 *     Note that UIO device names are based solely on the device registration
 *     order, so in this case the ``0`` has nothing to do with the memory
 *     region number (or Cuddl device name) at all.  Instead, the ``mmap()``
 *     offset argument is used to select which memory region of a device is
 *     to be mapped.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddl_meminfo_priv {
	unsigned long pa_mmap_offset;
	char device_name[CUDDL_IMPL_MAX_STR_LEN];
};

/**
 * struct cuddl_eventinfo_priv - Private event source information.
 *
 * @device_name:
 *
 *     Name of the device that will be used to receive interrupt events.
 *      
 *     Under Xenomai UDD, the value of this field will be something like::
 *      
 *       /dev/rtdm/mydevname
 *      
 *     where ``mydevname`` indicates the name of the associated Cuddl device.
 *      
 *     Under Linux UIO, the value of this field will be something like::
 *      
 *       /dev/uio0
 *      
 *     Note that UIO device names are based solely on the device registration
 *     order, so in this case the ``0`` has nothing to do with the Cuddl
 *     device name at all.
 *      
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddl_eventinfo_priv {
	char device_name[CUDDL_IMPL_MAX_STR_LEN];
};

#endif /* !_CUDDL_COMMON_IMPL_LINUX_H */
