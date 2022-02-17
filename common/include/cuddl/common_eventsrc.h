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
 * This part of the API is technically available to both user-space and
 * kernel-space code, however the only intended kernel-space usage of these
 * declarations is to convey information to user-space applications.
 */

/** 
 * enum cuddl_eventsrc_flags - Event source flags for user space.
 *
 * @CUDDL_EVENTSRCF_WAITABLE:
 *     Indicates that the event source can wake up a waiting user-space task.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     ``read()`` operation on the file descriptor associated with the event
 *     source.
 *
 * @CUDDL_EVENTSRCF_MASKABLE:
 *     Indicates that a user-space task may mask interrupt events through the
 *     event source.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     ``write()`` operation on file descriptor associated with the event
 *     source.  If supported, writing a ``uint32_t`` value of ``0`` to the
 *     file descriptor masks (disables) interrupt events.
 *
 * @CUDDL_EVENTSRCF_UNMASKABLE:
 *     Indicates that a user-space task may unmask interrupt events through
 *     the event source.
 *
 *     On Linux and Xenomai, this feature is implemented via a 4-byte
 *     ``write()`` operation on file descriptor associated with the event
 *     source.  If supported, writing a ``uint32_t`` value of ``1`` to the
 *     file descriptor unmasks (enables) interrupt events.
 *
 * Flags that describe the properties of an event source to user-space code.
 */
enum cuddl_eventsrc_flags {
	CUDDL_EVENTSRCF_WAITABLE   = (1 << 0),
	CUDDL_EVENTSRCF_MASKABLE   = (1 << 1),
	CUDDL_EVENTSRCF_UNMASKABLE = (1 << 2),
};

/**
 * struct cuddl_eventinfo - Event source information for user space.
 *
 * @flags: Flags that describe the properties of the event source.  This
 *         field may be a set of ``cuddl_eventsrc_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Event source information that is exported to user-space code.
 *
 * On Linux and Xenomai systems, this information is retrieved from the
 * kernel via an ``ioctl`` call.
 */
struct cuddl_eventinfo {
	int flags;
	struct cuddl_eventinfo_priv priv;
};

#endif /* !_CUDDL_COMMON_EVENTSRC_H */
