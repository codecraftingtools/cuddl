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

#ifndef _CUDDL_HPP
#define _CUDDL_HPP

/*
 * User-space declarations (C++).
 *
 * This part of the API is only available to user-space code.
 */

extern "C" {
#include <cuddl.h>
}

#include <cstring>
#include <string>
#include <system_error>
#include <sstream>
#include <ostream>
#include <chrono>

// Remove conflicting declarations from <sys/types.h>
// If you need these, #include <sys/sysmacros.h> after this header
#undef major
#undef minor

// Xenomai-specific code
#ifdef __XENO__
#  include <pthread.h>
#else
#  define pthread_setmode_np(a,b,c)
#  define PTHREAD_WARNSW 0
#endif

namespace cuddl {

//----------------------------------------------------------------------------
// C++ Utility Code for Internal Use
//----------------------------------------------------------------------------

inline void throw_err(int code, std::string const &msg)
{
	// Real-time bets are off (Xenomai only)
	pthread_setmode_np(PTHREAD_WARNSW, 0, NULL);

	auto ec = std::error_code(-code, std::system_category());
	std::ostringstream oss;
	oss << " returned " << code;
	throw std::system_error(ec, msg + oss.str());
}

const std::string flag_sep{" | "};

template<class F>
class Flags {
public:
	Flags(int flags=0) : flags_{flags} {}
	Flags(const F &f) {set(f);}

	int as_int() const {return flags_;}

	bool is_set(const F &f) const {
		return flags_ & static_cast<int>(f);
	}

	void set(const F &f) {
		flags_ |= static_cast<int>(f);
	}
	void clear(const F &f) {
		flags_ &= ~static_cast<int>(f);
	}

	void set(const Flags &f) {
		flags_ |= f.as_int();
	}
	void clear(const Flags &f) {
		flags_ &= ~f.as_int();
	}

	Flags &operator |=(const F &r) {
		flags_ |= static_cast<int>(r);
		return *this;
	}

	Flags &operator |=(const Flags &r) {
		flags_ |= r.flags_;
		return *this;
	}

	Flags operator | (const F &r) {
		Flags e;
		e |= *this;
		e |= r;
		return e;
	}

	Flags operator | (const Flags &r) {
		Flags e;
		e |= *this;
		e |= r;
		return e;
	}

private:
	int flags_{0};
};

template<class F>
inline Flags<F> operator |(const F &l, const F &r)
{
	Flags<F> e;
	e |= l;
	e |= r;
	return e;
}

template<class F>
inline Flags<F> operator |(const F &l, const Flags<F> &r)
{
	Flags<F> e;
	e |= l;
	e |= r;
	return e;
}

//----------------------------------------------------------------------------
// General
//----------------------------------------------------------------------------

const int MAX_STR_LEN = CUDDL_MAX_STR_LEN;

using size_t = cuddl_size_t;

class ResourceID
{
public:
	ResourceID() {
		memset(&id, 0, sizeof(id));
	}
	ResourceID(const cuddl_resource_id &id) : id{id} {
	}
	ResourceID(const char *group, const char *device,
		   const char *resource, int instance) {
		memset(&id, 0, sizeof(id));
		this->group(group);
		this->device(device);
		this->resource(resource);
		this->instance(instance);
	}

	operator cuddl_resource_id() const {return id;}

	std::string group()    const {return id.group;}
	std::string device()   const {return id.device;}
	std::string resource() const {return id.resource;}
	int         instance() const {return id.instance;}

	void group   (const char *s) {strncpy(id.group,    s, MAX_STR_LEN);}
	void device  (const char *s) {strncpy(id.device,   s, MAX_STR_LEN);}
	void resource(const char *s) {strncpy(id.resource, s, MAX_STR_LEN);}
	void instance(int i)         {id.instance = i;}

private:
	cuddl_resource_id id;
};

inline std::ostream &operator <<(std::ostream &os, const ResourceID &id)
{
	std::string group       {id.group() };
	std::string device      {id.device()};
	std::string resource    {id.resource()};
	int instance            {id.instance()};

	if (group    == "") group="*";
	if (device   == "") device="*";
	if (resource == "") resource="*";

	os << group << "/" << device << "/" << resource << "/";

	if (instance)
		os << id.instance();
	else
		os << "*";

	return os;
}

//----------------------------------------------------------------------------
// Version
//----------------------------------------------------------------------------

class Version
{
public:
	Version(int code=0) : code_(code) {}
	Version(int maj, int min, int rev) :
		code_(CUDDL_VERSION(maj,min,rev)) {}

	int code()     const {return code_;};

	int major()    const {return CUDDL_MAJOR_VERSION_FROM_CODE (code_);}
	int minor()    const {return CUDDL_MINOR_VERSION_FROM_CODE (code_);}
	int revision() const {return CUDDL_REVISION_LEVEL_FROM_CODE(code_);}
private:
	int code_;
};

inline std::ostream &operator <<(std::ostream &os, const Version &v)
{
	os << v.major() << "." << v.minor() << "." << v.revision();
	return os;
}

inline bool operator ==(const Version &l, const Version &r)
{ return l.code() == r.code(); }
inline bool operator !=(const Version &l, const Version &r)
{ return l.code() != r.code(); }
inline bool operator <(const Version &l, const Version &r)
{ return l.code() < r.code(); }
inline bool operator >(const Version &l, const Version &r)
{ return l.code() > r.code(); }
inline bool operator <=(const Version &l, const Version &r)
{ return l.code() <= r.code(); }
inline bool operator >=(const Version &l, const Version &r)
{ return l.code() >= r.code(); }

const Version version{
	CUDDL_VERSION_MAJOR, CUDDL_VERSION_MINOR, CUDDL_REVISION_LEVEL};

inline Version get_kernel_version()
{
	int ret = cuddl_get_kernel_version_code();
	if (ret < 0) { throw_err(ret, __func__); }
	return Version(ret);
}

inline std::string get_kernel_commit_id()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_kernel_commit_id(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

inline std::string get_userspace_commit_id()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_userspace_commit_id(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

inline std::string get_kernel_variant()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_kernel_variant(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

//----------------------------------------------------------------------------
// I/O Memory Access
//----------------------------------------------------------------------------

using iomem_t = cuddl_iomem_t;

const auto ioread8  = cuddl_ioread8;
const auto ioread16 = cuddl_ioread16;
const auto ioread32 = cuddl_ioread32;

const auto iowrite8  = cuddl_iowrite8;
const auto iowrite16 = cuddl_iowrite16;
const auto iowrite32 = cuddl_iowrite32;

//----------------------------------------------------------------------------
// Memory Region
//----------------------------------------------------------------------------

enum class MemRegionFlag {
	SHARED = CUDDL_MEMF_SHARED,
};

inline std::ostream &operator <<(std::ostream &os, const MemRegionFlag &f)
{
	if (f == MemRegionFlag::SHARED) os << "SHARED";
	else                            os << "INVALID_FLAG";
	return os;
}

using MemRegionFlags = Flags<MemRegionFlag>;

inline std::ostream &operator <<(std::ostream &os, const MemRegionFlags &f)
{
	std::string sep = "";

	if (f.is_set(MemRegionFlag::SHARED)) {
		os << sep << MemRegionFlag::SHARED;
		sep = flag_sep;
	}
	return os;
}

enum class MemRegionClaimFlag {
	HOSTILE = CUDDL_MEM_CLAIMF_HOSTILE,
};

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionClaimFlag &f)
{
	if (f == MemRegionClaimFlag::HOSTILE) os << "HOSTILE";
	else                                  os << "INVALID_FLAG";
	return os;
}

using MemRegionClaimFlags = Flags<MemRegionClaimFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionClaimFlags &f)
{
	std::string sep = "";

	if (f.is_set(MemRegionClaimFlag::HOSTILE)) {
		os << sep << MemRegionClaimFlag::HOSTILE;
		sep = flag_sep;
	}
	return os;
}

enum class MemRegionMapFlag {
};

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionMapFlag &f)
{
	return os;
}

using MemRegionMapFlags = Flags<MemRegionMapFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionMapFlags &f)
{
	return os;
}

class MemRegionInfo
{
public:
	MemRegionInfo() {
		memset(&info, 0, sizeof(info));
	}
	MemRegionInfo(const cuddl_memregion_info &info): info(info) {}

	operator cuddl_memregion_info() const {return info;}

	cuddl::size_t len() const {return info.len;}
	MemRegionFlags flags() const {return info.flags;}

private:
	cuddl_memregion_info info;
};

inline std::ostream &operator <<(std::ostream &os, const MemRegionInfo &info)
{
	os << "len: "   << info.len() << ", "
	   << "flags: " << info.flags();
	return os;
}

class MemRegion
{
private:
	MemRegion(const MemRegion&) = delete;
	MemRegion& operator=(const MemRegion&) = delete;
public:
	MemRegion() {
		memset(&mem, 0, sizeof(mem));
	}
	MemRegion(const cuddl_memregion &mem, bool mapped=false)
		: mem(mem), mapped_(mapped)
	{
	}
	MemRegion(const cuddl_resource_id &id,
		  const MemRegionClaimFlags &claim_flags=0,
		  const MemRegionMapFlags &map_flags=0) {
		memset(&mem, 0, sizeof(mem));
		claim_and_map(id, claim_flags, map_flags);
	}
	MemRegion(const char *group, const char *device="",
		  const char *resource="", int instance=0,
		  const MemRegionClaimFlags &claim_flags=0,
		  const MemRegionMapFlags &map_flags=0) {
		memset(&mem, 0, sizeof(mem));
		claim_and_map(
			ResourceID(group, device, resource, instance),
			claim_flags, map_flags);
	}

	~MemRegion() {unmap_and_release();}

	operator cuddl_memregion() const {return mem;}

	cuddl::iomem_t* addr() const {return mem.addr;}
	cuddl::size_t len() const {return mem.len;}
	MemRegionFlags flags() const {return mem.flags;}
	bool is_mapped() const {return mapped_;}

	void claim_and_map(const cuddl_resource_id &id,
		           const MemRegionClaimFlags &claim_flags=0,
		           const MemRegionMapFlags &map_flags=0) {
		int ret = cuddl_memregion_claim_and_map(
			&mem, id.group, id.device, id.resource,
			id.instance, claim_flags.as_int(), 0);
		if (ret < 0) { throw_err(ret, __func__); }
		mapped_ = true;
	}

	void unmap_and_release() {
		if (mapped_) {
			cuddl_memregion_unmap_and_release(&mem);
			mapped_ = false;
		}
	}

	std::string get_driver_info() {
		char s[MAX_STR_LEN];
		int ret = cuddl_memregion_get_driver_info(
			&mem, s, MAX_STR_LEN);
		if (ret < 0) { throw_err(ret, __func__); }
		return s;
	}

	std::string get_hw_info() {
		char s[MAX_STR_LEN];
		int ret = cuddl_memregion_get_hw_info(
			&mem, s, MAX_STR_LEN);
		if (ret < 0) { throw_err(ret, __func__); }
		return s;
	}

private:
	cuddl_memregion mem;
	bool mapped_{false};
};

inline std::ostream &operator <<(std::ostream &os, const MemRegion &mem)
{
	os << "addr: "   << mem.addr() << ", "
	   << "len: "   << mem.len() << ", "
	   << "flags: " << mem.flags();
	return os;
}

//----------------------------------------------------------------------------
// Event Source
//----------------------------------------------------------------------------

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

enum class EventSrcOpenFlag {
};

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcOpenFlag &f)
{
	return os;
}

using EventSrcOpenFlags = Flags<EventSrcOpenFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const EventSrcOpenFlags &f)
{
	return os;
}

class EventSrcInfo
{
public:
	EventSrcInfo() {
		memset(&info, 0, sizeof(info));
	}
	EventSrcInfo(const cuddl_eventsrc_info &info) : info(info) {}

	operator cuddl_eventsrc_info() const {return info;}

	EventSrcFlags flags() const {return info.flags;}

private:
	cuddl_eventsrc_info info;
};

inline std::ostream &operator <<(std::ostream &os, const EventSrcInfo &info)
{
	os << "flags: " << info.flags();
	return os;
}

class TimeSpec
{
public:
	TimeSpec() {
		memset(&ts, 0, sizeof(ts));
	}
	TimeSpec(const cuddl_timespec &ts) : ts(ts) {}
	TimeSpec(cuddl_time_t s, long n) {
		ts.tv_sec = s;
		ts.tv_nsec = n;
	}

	operator cuddl_timespec() const {return ts;}

	cuddl_time_t sec() const {return ts.tv_sec;}
	long nsec() const {return ts.tv_nsec;}

	void sec(cuddl_time_t s) {ts.tv_sec = s;}
	void nsec(long n) {ts.tv_nsec = n;}
private:
	cuddl_timespec ts;
};

class EventSrc
{
private:
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
	EventSrc(const cuddl_resource_id &id,
		  const EventSrcClaimFlags &claim_flags=0,
		  const EventSrcOpenFlags &open_flags=0) {
		memset(&eventsrc, 0, sizeof(eventsrc));
		claim_and_open(id, claim_flags, open_flags);
	}
	EventSrc(const char *group, const char *device="",
		  const char *resource="", int instance=0,
		  const EventSrcClaimFlags &claim_flags=0,
		  const EventSrcOpenFlags &open_flags=0) {
		memset(&eventsrc, 0, sizeof(eventsrc));
		claim_and_open(
			ResourceID(group, device, resource, instance),
			claim_flags, open_flags);
	}

	~EventSrc() {close_and_release();}

	operator cuddl_eventsrc() const {return eventsrc;}

	EventSrcFlags flags() const {return eventsrc.flags;}
	bool is_open() const {return opened_;}

	void claim_and_open(const cuddl_resource_id &id,
		           const EventSrcClaimFlags &claim_flags=0,
		           const EventSrcOpenFlags &open_flags=0) {
		int ret = cuddl_eventsrc_claim_and_open(
			&eventsrc, id.group, id.device, id.resource,
			id.instance, claim_flags.as_int(), 0);
		if (ret < 0) { throw_err(ret, __func__); }
		opened_ = true;
	}

	void close_and_release() {
		if (opened_) {
			cuddl_eventsrc_close_and_release(&eventsrc);
			opened_ = false;
		}
	}

	int wait() {return cuddl_eventsrc_wait(&eventsrc);}

	int timed_wait(const cuddl_timespec &timeout) {
		return cuddl_eventsrc_timed_wait(&eventsrc, &timeout);
	}

	int timed_wait(const std::chrono::nanoseconds &timeout) {
		auto s = std::chrono::duration_cast<std::chrono::seconds>(
			timeout);
		auto ns = timeout - s;
		cuddl_timespec ts;
		ts.tv_sec = s.count();
		ts.tv_nsec = ns.count();
		return cuddl_eventsrc_timed_wait(&eventsrc, &ts);
	}

	int enable() {return cuddl_eventsrc_enable(&eventsrc);}

	int disable() {return cuddl_eventsrc_disable(&eventsrc);}

	std::string get_driver_info() {
		char s[MAX_STR_LEN];
		int ret = cuddl_eventsrc_get_driver_info(
			&eventsrc, s, MAX_STR_LEN);
		if (ret < 0) { throw_err(ret, __func__); }
		return s;
	}

	std::string get_hw_info() {
		char s[MAX_STR_LEN];
		int ret = cuddl_eventsrc_get_hw_info(
			&eventsrc, s, MAX_STR_LEN);
		if (ret < 0) { throw_err(ret, __func__); }
		return s;
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

//----------------------------------------------------------------------------
// Manager
//----------------------------------------------------------------------------

inline int get_max_managed_devices()
{
	int ret = cuddl_get_max_managed_devices();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline int get_max_dev_mem_regions()
{
	int ret = cuddl_get_max_dev_mem_regions();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline int get_max_dev_events()
{
	int ret = cuddl_get_max_dev_events();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline std::string get_driver_info_for_slot(int device_slot)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_driver_info_for_slot(s, MAX_STR_LEN, device_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

inline std::string get_hw_info_for_slot(int device_slot)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_hw_info_for_slot(s, MAX_STR_LEN, device_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

inline ResourceID get_memregion_id_for_slot(int device_slot, int mem_slot)
{
	cuddl_resource_id id;

	int ret = cuddl_get_memregion_id_for_slot(&id, device_slot, mem_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return ResourceID(id);
}

inline ResourceID get_eventsrc_id_for_slot(int device_slot, int event_slot)
{
	cuddl_resource_id id;

	int ret = cuddl_get_eventsrc_id_for_slot(
		&id, device_slot, event_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return ResourceID(id);
}

inline MemRegionInfo get_memregion_info_for_id(const cuddl_resource_id &id)
{
	cuddl_memregion_info info;

	int ret = cuddl_get_memregion_info_for_id(&info, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return MemRegionInfo(info);
}

inline EventSrcInfo get_eventsrc_info_for_id(const cuddl_resource_id &id)
{
	cuddl_eventsrc_info info;

	int ret = cuddl_get_eventsrc_info_for_id(&info, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return EventSrcInfo(info);
}

inline int get_memregion_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_get_memregion_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline int get_eventsrc_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_get_eventsrc_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline int decrement_memregion_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_decrement_memregion_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

inline int decrement_eventsrc_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_decrement_eventsrc_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

} // namespace cuddl

#endif /* !_CUDDL_HPP */
