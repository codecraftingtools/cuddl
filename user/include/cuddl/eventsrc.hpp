/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer user-space C++ declarations.
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

#ifndef _CUDDL_EVENTSRC_HPP
#define _CUDDL_EVENTSRC_HPP

// C++ event source declarations.

#include <cuddl/general.hpp>
#include <chrono>

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:enum:`cuddl_eventsrc_flags`.
///
/// The ``|`` operator is overloaded to return an :cpp:type:`EventSrcFlags`
/// instance.  The stream output operator is also overloaded.
///
/// \endverbatim
enum class EventSrcFlag {
	SHARED      = CUDDL_EVENTSRCF_SHARED,
	WAITABLE    = CUDDL_EVENTSRCF_WAITABLE,
	HAS_DISABLE = CUDDL_EVENTSRCF_HAS_DISABLE,
	HAS_ENABLE  = CUDDL_EVENTSRCF_HAS_ENABLE,
};

inline std::ostream &operator <<(std::ostream &os, const EventSrcFlag &f)
{
	if      (f == EventSrcFlag::SHARED)      os << "SHARED";
	else if (f == EventSrcFlag::WAITABLE)    os << "WAITABLE";
	else if (f == EventSrcFlag::HAS_DISABLE) os << "HAS_DISABLE";
	else if (f == EventSrcFlag::HAS_ENABLE)  os << "HAS_ENABLE";
	else                                     os << "INVALID_FLAG";
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more :cpp:enum:`EventSrcFlag`
/// elements.
///
/// See the :cpp:type:`Flags` class template documentation for member
/// functions.
///
/// The ``|`` operator is overloaded for :cpp:enum:`EventSrcFlag` and
/// :cpp:type:`EventSrcFlags` arguments.  The ``|=`` operator is overloaded
/// as an alias for ``Flags::set``.  The stream output operator is also
/// overloaded.
///
/// \endverbatim
using EventSrcFlags = Flags<EventSrcFlag>;

inline std::ostream &operator <<(std::ostream &os, const EventSrcFlags &f)
{
	std::string sep = "";

	if (f.is_set(EventSrcFlag::SHARED)) {
		os << sep << EventSrcFlag::SHARED;
		sep = flag_sep;
	}
	if (f.is_set(EventSrcFlag::WAITABLE)) {
		os << sep << EventSrcFlag::WAITABLE;
		sep = flag_sep;
	}
	if (f.is_set(EventSrcFlag::HAS_DISABLE)) {
		os << sep << EventSrcFlag::HAS_DISABLE;
		sep = flag_sep;
	}
	if (f.is_set(EventSrcFlag::HAS_ENABLE)) {
		os << sep << EventSrcFlag::HAS_ENABLE;
		sep = flag_sep;
	}
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:enum:`cuddl_eventsrc_claim_flags`.
///
/// The ``|`` operator is overloaded to return an
/// :cpp:type:`EventSrcClaimFlags` instance.  The stream output operator is
/// also overloaded.
///
/// \endverbatim
enum class EventSrcClaimFlag {
	HOSTILE = CUDDL_EVENTSRC_CLAIMF_HOSTILE,
};

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcClaimFlag &f)
{
	if (f == EventSrcClaimFlag::HOSTILE) os << "HOSTILE";
	else                                 os << "INVALID_FLAG";
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more :cpp:enum:`EventSrcClaimFlag`
/// elements.
///
/// See the :cpp:type:`Flags` class template documentation for member
/// functions.
///
/// The ``|`` operator is overloaded for :cpp:enum:`EventSrcClaimFlag` and
/// :cpp:type:`EventSrcClaimFlags` arguments.  The ``|=`` operator is
/// overloaded as an alias for ``Flags::set``.  The stream output operator is
/// also overloaded.
///
/// \endverbatim
using EventSrcClaimFlags = Flags<EventSrcClaimFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcClaimFlags &f)
{
	std::string sep = "";

	if (f.is_set(EventSrcClaimFlag::HOSTILE)) {
		os << sep << EventSrcClaimFlag::HOSTILE;
		sep = flag_sep;
	}
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// Placeholder for event source open flags that may possibly be implemented
/// in the future.
///
/// The ``|`` operator is overloaded to return an
/// :cpp:type:`EventSrcOpenFlags` instance.  The stream output operator is
/// also overloaded.
///
/// \endverbatim
enum class EventSrcOpenFlag {
};

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcOpenFlag &f)
{
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more :cpp:enum:`EventSrcOpenFlag`
/// elements.
///
/// See the :cpp:type:`Flags` class template documentation for member
/// functions.
///
/// The ``|`` operator is overloaded for :cpp:enum:`EventSrcOpenFlag` and
/// :cpp:type:`EventSrcOpenFlags` arguments.  The ``|=`` operator is
/// overloaded as an alias for ``Flags::set``.  The stream output operator is
/// also overloaded.
///
/// \endverbatim
using EventSrcOpenFlags = Flags<EventSrcOpenFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcOpenFlags &f)
{
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_eventsrc_info`.
///
/// The stream output operator is overloaded for instances of this class.
///
/// \endverbatim
class EventSrcInfo
{
public:
	/// @name Constructors
	/// @{
	EventSrcInfo() {
		memset(&info, 0, sizeof(info));
	}
	EventSrcInfo(const cuddl_eventsrc_info &info) : info(info) {}
        ///  @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_eventsrc_info() const {return info;}

	/// @name Getter Functions
	/// @{
	EventSrcFlags flags() const {return info.flags;}
        ///  @}

private:
	cuddl_eventsrc_info info;
};

inline std::ostream &operator <<(std::ostream &os, const EventSrcInfo &info)
{
	os << "flags: " << info.flags();
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_timespec`.
///
/// \endverbatim
class TimeSpec
{
public:
	/// @name Constructors
	/// @{
	TimeSpec() {
		memset(&ts, 0, sizeof(ts));
	}
	TimeSpec(const cuddl_timespec &ts) : ts(ts) {}
	TimeSpec(cuddl_time_t s, long n) {
		ts.tv_sec = s;
		ts.tv_nsec = n;
	}
        ///  @}

	operator cuddl_timespec() const {return ts;}

	/// @name Getter Functions
	/// @{
	cuddl_time_t sec() const {return ts.tv_sec;}
	long nsec() const {return ts.tv_nsec;}
        ///  @}

	/// @name Setter Functions
	/// @{
	void sec(cuddl_time_t s) {ts.tv_sec = s;}
	void nsec(long n) {ts.tv_nsec = n;}
        ///  @}

private:
	cuddl_timespec ts;
};

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_eventsrc`.
///
/// The stream output operator is overloaded for instances of this class.
///
/// \endverbatim
class EventSrc
{
private:
	/// @name Constructors
	/// @{
	EventSrc(const EventSrc&) = delete;
	EventSrc& operator=(const EventSrc&) = delete;
public:
	EventSrc() {
		memset(&eventsrc, 0, sizeof(eventsrc));
	}
	EventSrc(const cuddl_eventsrc &eventsrc, bool opened=false)
		: eventsrc(eventsrc), opened_(opened)
	{
	}
	/// @throws std::system_error Operation failed.
	EventSrc(const cuddl_resource_id &id,
		  const EventSrcClaimFlags &claim_flags=0,
		  const EventSrcOpenFlags &open_flags=0) {
		memset(&eventsrc, 0, sizeof(eventsrc));
		claim_and_open(id, claim_flags, open_flags);
	}
	/// @throws std::system_error Operation failed.
	EventSrc(const char *group, const char *device="",
		  const char *resource="", int instance=0,
		  const EventSrcClaimFlags &claim_flags=0,
		  const EventSrcOpenFlags &open_flags=0) {
		memset(&eventsrc, 0, sizeof(eventsrc));
		claim_and_open(
			ResourceID(group, device, resource, instance),
			claim_flags, open_flags);
	}
        ///  @}

	/// @name Destructor
	/// @{
	~EventSrc() {close_and_release();}
        ///  @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_eventsrc() const {return eventsrc;}

	/// @name Getter Functions
	/// @{
	EventSrcFlags flags() const {return eventsrc.flags;}
        ///  @}

	/// Test if the event source has been successfully opened.
	bool is_open() const {return opened_;}

	/// @name Explicit Resource Management
	///
	/// Note that resource management can be performed explicitly, or via
	/// the constructor / destructor.
	/// @{

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_claim_and_open`.
	///
	/// \endverbatim
	/// @throws std::system_error Operation failed.
	void claim_and_open(const cuddl_resource_id &id,
		           const EventSrcClaimFlags &claim_flags=0,
		           const EventSrcOpenFlags &open_flags=0) {
		int ret = cuddl_eventsrc_claim_and_open(
			&eventsrc, id.group, id.device, id.resource,
			id.instance, claim_flags.as_int(), 0);
		if (ret < 0) { throw_err(ret, __func__); }
		opened_ = true;
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_close_and_release`.
	///
        /// \endverbatim
	void close_and_release() {
		if (opened_) {
			cuddl_eventsrc_close_and_release(&eventsrc);
			opened_ = false;
		}
	}
        ///  @}

	/// @name Event Wake-Up
	/// @{

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_wait`.
	///
	/// \endverbatim
	int wait() {return cuddl_eventsrc_wait(&eventsrc);}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_timed_wait`.
	///
	/// \endverbatim
	int timed_wait(const cuddl_timespec &timeout) {
		return cuddl_eventsrc_timed_wait(&eventsrc, &timeout);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_wait`.
	///
	/// \endverbatim
	int timed_wait(const std::chrono::nanoseconds &timeout) {
		auto s = std::chrono::duration_cast<std::chrono::seconds>(
			timeout);
		auto ns = timeout - s;
		cuddl_timespec ts;
		ts.tv_sec = s.count();
		ts.tv_nsec = ns.count();
		return cuddl_eventsrc_timed_wait(&eventsrc, &ts);
	}
        ///  @}

	/// @name Event Enable/Disable
	/// @{

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_enable`.
	///
	/// \endverbatim
	int enable() {return cuddl_eventsrc_enable(&eventsrc);}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_disable`.
	///
	/// \endverbatim
	int disable() {return cuddl_eventsrc_disable(&eventsrc);}
        ///  @}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_eventsrc_get_resource_id`.
	///
	/// \endverbatim
	/// @throws std::system_error Operation failed.
	ResourceID get_resource_id() {
		cuddl_resource_id id;

		int ret = cuddl_eventsrc_get_resource_id(&eventsrc, &id);
		if (ret < 0) { throw_err(ret, __func__); }
		return ResourceID(id);
	}

private:
	cuddl_eventsrc eventsrc;
	bool opened_{false};
};

inline std::ostream &operator <<(std::ostream &os, const EventSrc &eventsrc)
{
	os << "flags: " << eventsrc.flags();
	return os;
}

} // namespace cuddl

#endif /* !_CUDDL_EVENTSRC_HPP */
