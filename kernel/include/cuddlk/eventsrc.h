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

#ifndef _CUDDLK_EVENTSRC_H
#define _CUDDLK_EVENTSRC_H

#include <cuddlk/interrupt.h>

/**
 * DOC: Kernel-space event source declarations.
 *
 * Event sources are used to wake up user-space tasks when a specific event
 * (such as a hardware interrupt from a specific peripheral device) occurs.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * Note that the comments in this file regarding Linux UIO and Xenomai UDD
 * implementations are based on *linux-cip-4.19.94-cip18* and *xenomai-3.1*,
 * but other versions are probably very similar.
 */

/**
 * enum cuddlk_eventsrc_flags - Event source flags for kernel space.
 *
 * @CUDDLK_EVENTSRCF_SHARED: Indicates that the associated event source may
 *                           be claimed by more than one user-space
 *                           application simultaneously.
 *
 * Flags that describe the properties of an event source.  These may be used
 * in the ``flags`` member of the ``cuddlk_eventsrc`` struct.
 */
enum cuddlk_eventsrc_flags {
	CUDDLK_EVENTSRCF_SHARED = (1 << 0),
};

/**
 * struct cuddlk_eventsrc_kernel - Kernel-managed event source data members.
 *
 * @ref_count: Number of user-space applications that have claimed this event
 *             source.  This should be either ``0`` or ``1`` unless the event
 *             source has the ``CUDDLK_EVENTSRCF_SHARED`` flag set.
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
 * @flags: Flags that describe the properties of the event source.  This
 *         field may be a set of ``cuddlk_eventsrc_flags`` ORed together.
 *
 * @intr: Data structure for managing the interrupt handler and
 *        enable/disable functions.
 *
 * @kernel: Kernel-managed memory region data that is available for use by
 *          Cuddl drivers.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Represents an event source (usually triggered by an interrupt) that may be
 * monitored by user-space applications.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_eventsrc {
	char *name;
	int flags;
	struct cuddlk_interrupt intr;
	struct cuddlk_eventsrc_kernel kernel;
	struct cuddlk_eventsrc_priv priv;
};

/**
 * cuddlk_eventsrc_notify() - Programmatically trigger a user-space event.
 *
 * Trigger a user-space interrupt event for the specified device.  This
 * routine is typically not called directly unless the event source's
 * ``intr.irq`` field is set to ``CUDDLK_IRQ_CUSTOM``.  If the event source
 * is directly associated with a hardware interrupt (i.e. ``int.irq`` >
 * ``0``), then user-space events are triggered automatically without calling
 * this function.
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
void cuddlk_eventsrc_notify(struct cuddlk_eventsrc *eventsrc);

#endif /* !_CUDDLK_EVENTSRC_H */
