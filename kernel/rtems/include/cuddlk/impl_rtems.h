/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer RTEMS kernel declarations.
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

#ifndef _CUDDLK_IMPL_RTEMS_H
#define _CUDDLK_IMPL_RTEMS_H
/**
 * DOC: Kernel-space declarations for the RTEMS implementation.
 *
 * These declarations are only available to kernel-space code.
 */

#ifdef CUDDLC_BUILD_WARN_TARGET
  #warning Compiling for RTEMS kernel
#endif /* CUDDLC_BUILD_WARN_TARGET */

#define CUDDLKI_MEMT_NONE    0
#define CUDDLKI_MEMT_PHYS    1
#define CUDDLKI_MEMT_LOGICAL 2
#define CUDDLKI_MEMT_VIRTUAL 3

#define CUDDLKI_VARIANT_STR "RTEMS"

#define CUDDLKI_RET_INTR_NOT_HANDLED 0
#define CUDDLKI_RET_INTR_HANDLED    (1 << 0)

#define CUDDLKI_IRQ_NONE   0
#define CUDDLKI_IRQ_CUSTOM (-1)

typedef void cuddlki_iomem_t;

typedef void cuddlk_parent_device_t;

/**
 * struct cuddlki_memregion_priv - Private kernel memory region data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlki_memregion_priv {
};

/**
 * struct cuddlki_interrupt_priv - Private kernel interrupt handler data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlki_interrupt_priv {
};

/**
 * struct cuddlki_eventsrc_priv - Private kernel event source data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlki_eventsrc_priv {
};

/**
 * struct cuddlki_device_priv - Private kernel device data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlki_device_priv {
};

/**
 * struct cuddlki_manager_priv - Private kernel device data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlki_manager_priv {
};

#endif /* !_CUDDLK_IMPL_RTEMS_H */
