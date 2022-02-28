/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel event source decls.
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

#ifndef _CUDDL_KERNEL_EVENTSRC_H
#define _CUDDL_KERNEL_EVENTSRC_H

#include <cuddl/kernel_interrupt.h>

/**
 * DOC: Kernel-space event source declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * Note that the comments in this file regarding Linux UIO and Xenomai UDD
 * implementations are based on *linux-cip-4.19.94-cip18* and *xenomai-3.1*,
 * but other versions are probably very similar.
 */

/**
 * struct cuddlk_eventsrc_kernel - Kernel-managed event source data members.
 *
 * @ref_count: Number of user-space applications that have claimed this event
 *             source.  This should be either ``0`` or ``1``.
 *
 * Kernel-managed ``cuddlk_eventsrc`` data members that are available for use
 * by Cuddl drivers.
 */
struct cuddlk_eventsrc_kernel {
	int ref_count;
};

/**
 * struct cuddlk_eventsrc - Event source information (kernel-space).
 *
 * @name: Name used to identify the event source.
 *
 * @intr: Data structure for managing the interrupt handler and
 *        masking/unmasking functions.
 *
 * @kernel: Kernel-managed memory region data that is available for use by
 *          Cuddl drivers.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Represents an event source (usually triggered by an interrupt) that may be
 * mapped by user-space applications.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_eventsrc {
	char *name;
	struct cuddlk_interrupt intr;
	struct cuddlk_eventsrc_kernel kernel;
	struct cuddlk_eventsrc_priv priv;
};

/**
 * cuddlk_event_notify() - Programmatically trigger a user-space event.
 *
 * Trigger a user-space interrupt event for the specified device
 *
 * @eventsrc: Event source to notify.
 *
 * Linux UIO equivalent in *linux/uio_driver.h*::
 *
 *   void uio_event_notify(struct uio_info *info);
 *
 * Xenomai UDD equivalent in *rtdm/udd.h*::
 *
 *   void udd_notify_event(struct udd_device *udd)
 */
void cuddlk_event_notify(struct cuddlk_eventsrc *eventsrc);

#endif /* !_CUDDL_KERNEL_EVENTSRC_H */
