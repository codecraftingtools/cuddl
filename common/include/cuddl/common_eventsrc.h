/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common event source decls.
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

#ifndef _CUDDL_COMMON_EVENTSRC_H
#define _CUDDL_COMMON_EVENTSRC_H

#include <cuddl/common_general.h>

/**
 * DOC: Common user/kernel-space event source declarations.
 *
 * Event sources are used to wake up user-space tasks when a specific event
 * (such as a hardware interrupt from a specific peripheral device) occurs.
 *
 * This part of the API is technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 */

/** 
 * enum cuddl_eventsrc_flags - Event source flags for user space.
 *
 * @CUDDL_EVENTSRCF_SHARED: Indicates that the associated event source may be
 *                          claimed by more than one user-space application
 *                          simultaneously.
 *
 * @CUDDL_EVENTSRCF_WAITABLE:
 *     Indicates that the event source can wake up a waiting user-space task.
 *
 *     If this flag is set, ``cuddl_eventsrc_wait()`` and
 *     ``cuddl_eventsrc_timedwait()`` are supported.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     blocking ``read()`` operation on the file descriptor associated with
 *     the event source. The ``select()`` operation is used when a timeout is
 *     specified.
 *
 * @CUDDL_EVENTSRCF_HAS_DISABLE:
 *     Indicates that a user-space task may disable interrupt events through
 *     the event source.
 *
 *     If this flag is set, ``cuddl_eventsrc_disable()`` is supported.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     ``write()`` operation on file descriptor associated with the event
 *     source.  If supported, writing a ``uint32_t`` value of ``0`` to the
 *     file descriptor masks (disables) interrupt events.
 *
 * @CUDDL_EVENTSRCF_HAS_ENABLE:
 *     Indicates that a user-space task may enable interrupt events through
 *     the event source.
 *
 *     If this flag is set, ``cuddl_eventsrc_enable()`` is supported.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     ``write()`` operation on file descriptor associated with the event
 *     source.  If supported, writing a ``uint32_t`` value of ``1`` to the
 *     file descriptor unmasks (enables) interrupt events.
 *
 * Flags that describe the properties of an event source to user-space code.
 */
enum cuddl_eventsrc_flags {
	CUDDL_EVENTSRCF_SHARED      = (1 << 0),
	CUDDL_EVENTSRCF_WAITABLE    = (1 << 1),
	CUDDL_EVENTSRCF_HAS_DISABLE = (1 << 2),
	CUDDL_EVENTSRCF_HAS_ENABLE  = (1 << 3),
};

/**
 * struct cuddl_eventsrc_info - Event source information for user space.
 *
 * @flags: Flags that describe the properties of the event source.  This
 *         field may be a set of ``cuddl_eventsrc_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Event source information that is exported to user-space code.  Typically,
 * the internal members of this structure do not need to be directly accessed
 * from user-space applications.
 *
 * On Linux and Xenomai systems, this information is retrieved from the
 * kernel via an ``ioctl`` call.
 */
struct cuddl_eventsrc_info {
	int flags;
	struct cuddlci_eventsrc_info_priv priv;
};

#endif /* !_CUDDL_COMMON_EVENTSRC_H */
