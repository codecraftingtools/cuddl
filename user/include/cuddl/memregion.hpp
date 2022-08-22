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

#ifndef _CUDDL_MEMREGION_HPP
#define _CUDDL_MEMREGION_HPP

// C++ memory region declarations.

#include <cuddl/general.hpp>
#include <cuddl/iomem.hpp>

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:enum:`cuddl_memregion_flags`.
///
/// The ``|`` operator is overloaded to return a `MemRegionFlags` instance.
/// The stream output operator is also overloaded.
///
/// \endverbatim
enum class MemRegionFlag {
	SHARED = CUDDL_MEMF_SHARED,
};

inline std::ostream &operator <<(std::ostream &os, const MemRegionFlag &f)
{
	if (f == MemRegionFlag::SHARED) os << "SHARED";
	else                            os << "INVALID_FLAG";
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more `MemRegionFlag` elements.
///
/// See the `Flags` class template documentation for member functions.
///
/// The ``|`` operator is overloaded for `MemRegionFlag` and `MemRegionFlags`
/// arguments.  The ``|=`` operator is overloaded as an alias for
/// `Flags::set`.  The stream output operator is also overloaded.
///
/// \endverbatim
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

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:enum:`cuddl_memregion_claim_flags`.
///
/// The ``|`` operator is overloaded to return a `MemRegionClaimFlags`
/// instance.  The stream output operator is also overloaded.
///
/// \endverbatim
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

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more `MemRegionClaimFlag`
/// elements.
///
/// See the `Flags` class template documentation for member functions.
///
/// The ``|`` operator is overloaded for `MemRegionClaimFlag` and
/// `MemRegionClaimFlags` arguments.  The ``|=`` operator is overloaded as an
/// alias for `Flags::set`.  The stream output operator is also overloaded.
///
/// \endverbatim
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

/// \verbatim embed:rst:leading-slashes
///
/// Placeholder for memory region mapping flags that may possibly be
/// implemented in the future.
///
/// The ``|`` operator is overloaded to return a `MemRegionMapFlags`
/// instance.  The stream output operator is also overloaded.
///
/// \endverbatim
enum class MemRegionMapFlag {
};

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionMapFlag &f)
{
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ class representing a set of one or more `MemRegionMapFlag` elements.
///
/// See the `Flags` class template documentation for member functions.
///
/// The ``|`` operator is overloaded for `MemRegionMapFlag` and
/// `MemRegionMapFlags` arguments.  The ``|=`` operator is overloaded as an
/// alias for `Flags::set`.  The stream output operator is also overloaded.
///
/// \endverbatim
using MemRegionMapFlags = Flags<MemRegionMapFlag>;

inline std::ostream &operator <<(
	std::ostream &os, const MemRegionMapFlags &f)
{
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_memregion_info`.
///
/// The stream output operator is overloaded for instances of this class.
///
/// \endverbatim
class MemRegionInfo
{
public:
	/// @name Constructors
	/// @{
	MemRegionInfo() {
		memset(&info, 0, sizeof(info));
	}
	MemRegionInfo(const cuddl_memregion_info &info): info(info) {}
        ///  @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_memregion_info() const {return info;}

	/// @name Getter Functions
	/// @{
	cuddl::size_t len() const {return info.len;}
	MemRegionFlags flags() const {return info.flags;}
        ///  @}

private:
	cuddl_memregion_info info;
};

inline std::ostream &operator <<(std::ostream &os, const MemRegionInfo &info)
{
	os << "len: "   << info.len() << ", "
	   << "flags: " << info.flags();
	return os;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_memregion`.
///
/// The stream output operator is overloaded for instances of this class.
///
/// \endverbatim
class MemRegion
{
private:
	MemRegion(const MemRegion&) = delete;
	MemRegion& operator=(const MemRegion&) = delete;
public:
	/// @name Constructors
	/// @{
	MemRegion() {
		memset(&mem, 0, sizeof(mem));
	}
	MemRegion(const cuddl_memregion &mem, bool mapped=false)
		: mem(mem), mapped_(mapped)
	{
	}
	/// @throws std::system_error Operation failed.
	MemRegion(const cuddl_resource_id &id,
		  const MemRegionClaimFlags &claim_flags=0,
		  const MemRegionMapFlags &map_flags=0) {
		memset(&mem, 0, sizeof(mem));
		claim_and_map(id, claim_flags, map_flags);
	}
	/// @throws std::system_error Operation failed.
	MemRegion(const char *group, const char *device="",
		  const char *resource="", int instance=0,
		  const MemRegionClaimFlags &claim_flags=0,
		  const MemRegionMapFlags &map_flags=0) {
		memset(&mem, 0, sizeof(mem));
		claim_and_map(
			ResourceID(group, device, resource, instance),
			claim_flags, map_flags);
	}
        ///  @}

	/// @name Destructor
	/// @{
	~MemRegion() {unmap_and_release();}
        /// @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_memregion() const {return mem;}

	/// @name Getter Functions
	/// @{
	cuddl::iomem_t* addr() const {return mem.addr;}
	cuddl::size_t len() const {return mem.len;}
	MemRegionFlags flags() const {return mem.flags;}
        ///  @}

	/// Test if the memory region has been successfully mapped.
	bool is_mapped() const {return mapped_;}

	/// @name Explicit Resource Management
	///
	/// Note that resource management can be performed explicitly, or via
	/// the constructor / destructor.
	/// @{

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_claim_and_map`.
	///
	/// \endverbatim
	/// @throws std::system_error Operation failed.
	void claim_and_map(const cuddl_resource_id &id,
		           const MemRegionClaimFlags &claim_flags=0,
		           const MemRegionMapFlags &map_flags=0) {
		int ret = cuddl_memregion_claim_and_map(
			&mem, id.group, id.device, id.resource,
			id.instance, claim_flags.as_int(), 0);
		if (ret < 0) { throw_err(ret, __func__); }
		mapped_ = true;
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_unmap_and_release`.
	///
        /// \endverbatim
	void unmap_and_release() {
		if (mapped_) {
			cuddl_memregion_unmap_and_release(&mem);
			mapped_ = false;
		}
	}
        ///  @}

	/// @name I/O Memory Access
	/// @{

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_ioread8`.
	///
        /// \endverbatim
	uint8_t ioread8(cuddl::size_t offset) {
		return cuddl_memregion_ioread8(&mem, offset);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_ioread16`.
	///
        /// \endverbatim
	uint16_t ioread16(cuddl::size_t offset) {
		return cuddl_memregion_ioread16(&mem, offset);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_ioread32`.
	///
        /// \endverbatim
	uint32_t ioread32(cuddl::size_t offset) {
		return cuddl_memregion_ioread32(&mem, offset);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_iowrite8`.
	///
        /// \endverbatim
	void iowrite8(uint8_t value, cuddl::size_t offset) {
		cuddl_memregion_iowrite8(&mem, value, offset);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_iowrite16`.
	///
        /// \endverbatim
	void iowrite16(uint16_t value, cuddl::size_t offset) {
		cuddl_memregion_iowrite16(&mem, value, offset);
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_iowrite32`.
	///
        /// \endverbatim
	void iowrite32(uint32_t value, cuddl::size_t offset) {
		cuddl_memregion_iowrite32(&mem, value, offset);
	}

        ///  @}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_get_driver_info`.
	///
        /// \endverbatim
	/// @throws std::system_error Operation failed.
	std::string get_driver_info() {
		char s[MAX_STR_LEN];
		int ret = cuddl_memregion_get_driver_info(
			&mem, s, MAX_STR_LEN);
		if (ret < 0) { throw_err(ret, __func__); }
		return s;
	}

	/// \verbatim embed:rst:leading-slashes
	///
	/// C++ wrapper for :c:func:`cuddl_memregion_get_hw_info`.
	///
        /// \endverbatim
	/// @throws std::system_error Operation failed.
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

} // namespace cuddl

#endif /* !_CUDDL_MEMREGION_HPP */
