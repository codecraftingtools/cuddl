/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space event src decls.
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

#ifndef _CUDDL_EVENTSRC_H
#define _CUDDL_EVENTSRC_H

#include <cuddl/common_eventsrc.h>
#include <cuddl/impl.h>

/**
 * DOC: User-space event source declarations.
 *
 * This part of the API is only available to user-space code.
 */

/**
 * typedef cuddl_time_t - Type used to represent a time in seconds.
 *
 * This will be equivalent to ``time_t`` on POSIX systems.
 */
typedef cuddl_impl_time_t cuddl_time_t;

/**
 * struct cuddl_timespec - Represents time in seconds and nanoseconds.
 *
 * @tv_sec: Seconds
 * @tv_nsec: Nanoseconds
 *
 * This is equivalent to ``struct timespec`` on POSIX systems.
 */
struct cuddl_timespec {
	cuddl_time_t tv_sec;
	long tv_nsec;
};

/**
 * struct cuddl_eventsrc - User-space event source interface.
 *
 * @flags: Flags that describe the properties of the event source.  This
 *         field may be a set of ``cuddl_eventsrc_flags`` ORed together.
 *
 * @priv: Private data reserved for internal use by the Cuddl implementation.
 *
 * Provides user-space applications with an interface for receiving
 * kernel-based (e.g. interrupt) events.
 */
struct cuddl_eventsrc {
	int flags;
	struct cuddl_eventsrc_priv priv;
};

/**
 * cuddl_event_claim() - Claim an event source from user space.
 *
 * @eventinfo: Pointer to a data structure that will receive the information
 *             required to open the specified event source from user space.
 *             If the claim operation is successful, and if this parameter is
 *             non-``NULL``, the required information will be copied into the
 *             data structure specified by this parameter.
 *
 * @id: Input parameter identifying the event source to be claimed.
 *
 * @options: Input parameter consisting of a set of flags (ORed together)
 *           that are applicable to the event source claim operation.  No
 *           claim flags are currently defined for event sources, so this
 *           parameter should be set to ``0``.
 *
 * Request ownership of the event source identified by ``id`` for the purpose
 * of receiving events in user space.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_claim(
	struct cuddl_eventinfo *eventinfo,
	const struct cuddl_resource_id *id,
	int options);

/**
 * cuddl_eventsrc_release() - Release an event source from user space.
 *
 * @eventinfo: Input parameter identifying the event source to be released.
 *             The data structure pointed to by this parameter should contain
 *             the information returned by a successful call to
 *             ``cuddl_eventsrc_claim()``.
 *
 * Release ownership of the event source identified by ``eventinfo``.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_release(struct cuddl_eventinfo *eventinfo);

/**
 * cuddl_eventsrc_open() - Open an event source from user space.
 *
 * @eventsrc: Pointer to a data structure that will receive the information
 *            required to receive events in user space.  If the open
 *            operation is successful, and if this parameter is non-``NULL``,
 *            the required information will be copied into the data structure
 *            specified by this parameter.
 *
 * @eventinfo: Input parameter identifying the event source to be opened.
 *             The data structure pointed to by this parameter should contain
 *             the information returned by a successful call to
 *             ``cuddl_eventsrc_claim()``.
 *
 * @options: Input parameter consisting of a set of flags (ORed together)
 *           that are applicable to the event source open operation.  No open
 *           flags are currently defined for event sources, so this parameter
 *           should be set to ``0``.
 *
 * Open the event source identified by ``eventinfo`` to enable reception of
 * events in user space.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_open(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_eventinfo *eventinfo,
	int options);

/**
 * cuddl_eventsrc_close() - Close an event source from user space.
 *
 * @eventsrc: Input parameter identifying the event source to be closed.  The
 *            data structure pointed to by this parameter should contain the
 *            information returned by a successful call to
 *            ``cuddl_eventsrc_open()``.
 *
 * Close the event source ``eventsrc`` to disable reception of events from
 * this source in user space.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_close(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_wait() - Wait for an event in user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            waited on.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()``.
 *
 * Performs a blocking wait for events from ``eventsrc``.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_wait(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_trywait() - Check for the occurance of a user-space event.
 *
 * @eventsrc: Input parameter identifying the event source to be polled for
 *            an event.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()``.
 *
 * Performs a non-blocking check for events from ``eventsrc``.
 *
 * Return:
 *   ``0`` on success (i.e. an event has occurred since the last check), or a
 *   negative error code.
 */
int cuddl_eventsrc_trywait(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_timedwait() - Timed wait for an event in user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            waited on.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()``.
 *
 * @timeout: Relative time value specifying the maximum time to wait for an
 *           event.
 *
 * Performs a blocking wait for events from ``eventsrc`` with a
 * timeout.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddl_eventsrc_timedwait(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_timespec *timeout);

#endif /* !_CUDDL_EVENTSRC_H */
