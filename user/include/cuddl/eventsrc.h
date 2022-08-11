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
 * Event sources are used to wake up user-space tasks when a specific event
 * (such as a hardware interrupt from a specific peripheral device) occurs.
 *
 * This part of the API is only available to user-space code.
 */

/**
 * typedef cuddl_time_t - Data type used to represent a time in seconds.
 *
 * This will be equivalent to ``time_t`` on POSIX systems.
 */
typedef cuddli_time_t cuddl_time_t;

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
 * enum cuddl_eventsrc_claim_flags - Flags used when claiming event sources.
 *
 * @CUDDL_EVENTSRC_CLAIMF_HOSTILE: Allows the claim operation to succeed,
 *                                 even if the event source has already been
 *                                 claimed by another application and the
 *                                 ``CUDDL_EVENTSRCF_SHARED`` flag has not
 *                                 been set.  Specifying this flag is usually
 *                                 not the right thing to do.
 *
 * Flags that are applicable to the event source claim operation.
 */
enum cuddl_eventsrc_claim_flags {
	CUDDL_EVENTSRC_CLAIMF_HOSTILE = (1 << 0),
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
 *             If the claim operation is successful, the required information
 *             will be copied into the data structure specified by this
 *             parameter.
 *
 * @group: Name of the group in which the specified event source's parent
 *         device resides.  In some cases, the ``group`` is used to indicate
 *         the PCI card on which the parent device is located.
 *
 * @device: Name of the specified event source's parent device.
 *          (i.e. hardware peripheral).
 *
 * @eventsrc: Name of the specific event source to be claimed.
 *
 * @instance: Used to differentiate between event sources that are registered
 *            with identical ``group``, ``device``, and ``resource``
 *            (i.e. ``eventsrc``) fields.
 *
 * @options: Input parameter specifying the set of flags to apply to this
 *           event source claim operation.  This field may be a set of
 *           ``cuddl_eventsrc_claim_flags`` ORed together.
 *
 * Request ownership of a specific event source for the purpose of receiving
 * events in user space.  The particular event source is identified by the
 * ``group``, ``device``, ``eventsrc``, and ``instance`` arguments.  If any
 * of these arguments is ``NULL`` or contains an empty string (or is ``0``,
 * in the case of ``instance``), the parameter will be treated as a *don't
 * care* value when searching for a matching event source in the resource
 * list.
 *
 * This routine is automatically called from
 * ``cuddl_eventsrc_claim_and_open()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENXIO``: The specified event source was not found.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       Cuddl manager device.
 */
int cuddl_eventsrc_claim(
	struct cuddl_eventsrc_info *eventinfo,
	const char *group,
	const char *device,
	const char *eventsrc,
	int instance,
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
 * This routine is automatically called from
 * ``cuddl_eventsrc_close_and_release()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Error code returned by ``cuddl_eventsrc_release_by_token()``.
 */
int cuddl_eventsrc_release(struct cuddl_eventsrc_info *eventinfo);

/**
 * cuddl_eventsrc_open() - Open an event source from user space.
 *
 * @eventsrc: Pointer to a data structure that will receive the information
 *            required to receive events in user space.  If the open
 *            operation is successful, the required information will be
 *            copied into the data structure specified by this parameter.
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
 * This routine is automatically called from
 * ``cuddl_eventsrc_claim_and_open()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       UIO or UDD event source device.
 */
int cuddl_eventsrc_open(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_eventsrc_info *eventinfo,
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
 * This routine is automatically called from
 * ``cuddl_eventsrc_close_and_release()``, so user-space applications do not
 * typically need to call this routine directly.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       UIO or UDD event source file descriptor.
 */
int cuddl_eventsrc_close(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_claim_and_open() - Claim and open an event source.
 *
 * @eventsrc: See ``cuddl_eventsrc_open()``.
 * @group: See ``cuddl_eventsrc_claim()``.
 * @device: See ``cuddl_eventsrc_claim()``.
 * @resource: Name of the specific event source to be claimed.
 * @instance: See ``cuddl_eventsrc_claim()``.
 * @claim_options: See ``cuddl_eventsrc_claim()``.
 * @open_options1: See ``cuddl_eventsrc_open()``.
 *
 * Request ownership of a specific event source and then open it for the
 * purpose of receiving events in user space.  See ``cuddl_eventsrc_claim()``
 * and ``cuddl_eventsrc_open()`` for more details.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Error code returned by ``cuddl_eventsrc_claim()``.
 *     - Linux: Error code returned by ``cuddl_eventsrc_open()``.
 */
int cuddl_eventsrc_claim_and_open(
	struct cuddl_eventsrc *eventsrc,
	const char *group,
	const char *device,
	const char *resource,
	int instance,
	int claim_options,
	int open_options1);

/**
 * cuddl_eventsrc_close_and_release() - Close and release an event source.
 *
 * @eventsrc: Input identifying the event source to be closed and released.
 *            The data structure pointed to by this parameter should contain
 *            the information returned by a successful call to
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * Close the event source ``eventsrc`` (disabling the reception of events
 * from this source in user space), and then release ownership of the event
 * source.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Error code returned by ``cuddl_eventsrc_close()``.
 *     - Linux: Error code returned by ``cuddl_eventsrc_release_by_token()``.
 */
int cuddl_eventsrc_close_and_release(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_wait() - Wait for an event in user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            waited on.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * Performs a blocking wait for events from ``eventsrc``.
 *
 * Return:
 *   Cumulative event source interrupt count on success (i.e. an event has
 *   occurred since the last check), or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``read()`` call on
 *       event source file descriptor.
 */
int cuddl_eventsrc_wait(struct cuddl_eventsrc *eventsrc);

/* NOT PUBLIC AT THIS TIME -- This may not turn out to be useful.
 *
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
 *   Cumulative event source interrupt count on success (i.e. an event has
 *   occurred since the last check), or a negative error code.
 *
 *   Error codes:
 *     - Linux: Error code returned by `cuddl_eventsrc_timedwait()``.
 */
int cuddl_eventsrc_trywait(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_timedwait() - Timed wait for an event in user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            waited on.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * @timeout: Relative time value specifying the maximum time to wait for an
 *           event.
 *
 * Performs a blocking wait for events from ``eventsrc`` with a
 * timeout.
 *
 * Return:
 *   Cumulative event source interrupt count on success (i.e. an event has
 *   occurred since the last check), or a negative error code.
 *
 *   Error codes:
 *     - ``-ETIMEDOUT``: A timeout occurred.
 *     - Linux: Value of ``-errno`` resulting from from ``select()`` call on
 *       event source file descriptor.
 *     - Linux: Value of ``-errno`` resulting from from ``read()`` call on
 *       event source file descriptor.
 */
int cuddl_eventsrc_timedwait(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_timespec *timeout);

/**
 * cuddl_eventsrc_enable() - Enable an event source from user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            enabled.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * Enables the event source if this operation is supported.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``write()`` call on
 *       event source file descriptor.
 */
int cuddl_eventsrc_enable(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_disable() - Disables an event source from user space.
 *
 * @eventsrc: Input parameter identifying the source of the event to be
 *            disabled.  The data structure pointed to by this parameter
 *            should contain the information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * Disables the event source if this operation is supported.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - Linux: Value of ``-errno`` resulting from from ``write()`` call on
 *       event source file descriptor.
 */
int cuddl_eventsrc_disable(struct cuddl_eventsrc *eventsrc);

/**
 * cuddl_eventsrc_get_driver_info() - Get the associated driver info string.
 *
 * @eventsrc: Input identifying the event source to be queried.  The data
 *            structure pointed to by this parameter should contain the
 *            information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * Retrieve the driver information string associated with the specified event
 * source's parent device.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENODEV``: The specified device slot is empty.
 *     - ``-EBADSLT``: The specified device slot is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       Cuddl manager device.
 */
int cuddl_eventsrc_get_driver_info(
	struct cuddl_eventsrc *eventsrc, char *info_str,
	cuddl_size_t info_len);

/**
 * cuddl_eventsrc_get_hw_info() - Get the associated hardware info string.
 *
 * @eventsrc: Input identifying the event source to be queried.  The data
 *            structure pointed to by this parameter should contain the
 *            information returned by a successful call to
 *            ``cuddl_eventsrc_open()`` or
 *            ``cuddl_eventsrc_claim_and_open()``.
 *
 * @info_str: Pointer to the buffer to be used for returning the info
 *            string.
 *
 * @info_len: Input parameter specifying the size of the buffer used for
 *            returning the info string.
 *
 * Retrieve the hardware information string associated with the specified
 * event source's parent device.
 *
 * Return: ``0`` on success, or a negative error code.
 *
 *   Error codes:
 *     - ``-ENODEV``: The specified device slot is empty.
 *     - ``-EBADSLT``: The specified device slot is out of range.
 *     - ``-ENOMEM``: Error allocating memory in IOCTL call (Linux).
 *     - ``-EOVERFLOW``: Error copying data to/from kernel space (Linux).
 *     - Linux: Value of ``-errno`` resulting from from ``open()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``ioctl()`` call on
 *       Cuddl manager device.
 *     - Linux: Value of ``-errno`` resulting from from ``close()`` call on
 *       Cuddl manager device.
 */
int cuddl_eventsrc_get_hw_info(
	struct cuddl_eventsrc *eventsrc, char *info_str,
	cuddl_size_t info_len);

#endif /* !_CUDDL_EVENTSRC_H */
