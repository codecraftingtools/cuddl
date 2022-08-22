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

#ifndef _CUDDL_VERSION_HPP
#define _CUDDL_VERSION_HPP

// C++ declarations related to version information.

#include <cuddl/general.hpp>

// Remove conflicting declarations from <sys/types.h>
// If you need these, #include <sys/sysmacros.h> after this header
#undef major
#undef minor

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for the C-API `version macros<user_version>`.
///
/// The stream output and comparison operators are overloaded for instances
/// of this class.
///
/// \endverbatim
class Version
{
public:
	/// @name Constructors
	/// @{
	Version(int code=0) : code_(code) {}
	Version(int maj, int min, int rev) :
		code_(CUDDL_VERSION(maj,min,rev)) {}
        ///  @}

	/// @name Getter Functions
	/// @{
	int code()     const {return code_;};
	int major()    const {return CUDDL_MAJOR_VERSION_FROM_CODE (code_);}
	int minor()    const {return CUDDL_MINOR_VERSION_FROM_CODE (code_);}
	int revision() const {return CUDDL_REVISION_LEVEL_FROM_CODE(code_);}
        ///  @}

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

/// \verbatim embed:rst:leading-slashes
///
/// C++ :cpp:class:`Version` class instance corresponding to the
/// :c:macro:`CUDDL_VERSION_CODE` C macro.
///
/// \endverbatim
const Version version{
	CUDDL_VERSION_MAJOR, CUDDL_VERSION_MINOR, CUDDL_REVISION_LEVEL};

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_kernel_version_code` that returns a
/// :cpp:class:`Version` class instance.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline Version get_kernel_version()
{
	int ret = cuddl_get_kernel_version_code();
	if (ret < 0) { throw_err(ret, __func__); }
	return Version(ret);
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_kernel_commit_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_kernel_commit_id()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_kernel_commit_id(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_userspace_commit_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_userspace_commit_id()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_userspace_commit_id(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_kernel_variant`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_kernel_variant()
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_kernel_variant(s, MAX_STR_LEN);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

} // namespace cuddl

#endif /* !_CUDDL_VERSION_HPP */
