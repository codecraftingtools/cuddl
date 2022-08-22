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

#ifndef _CUDDL_GENERAL_HPP
#define _CUDDL_GENERAL_HPP

// General-purpose C++ declarations.

#include <cuddl/utility.hpp>
#include <cstring>

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// Alias for :c:macro:`CUDDL_MAX_STR_LEN`.
///
/// \endverbatim
const int MAX_STR_LEN = CUDDL_MAX_STR_LEN;

/// \verbatim embed:rst:leading-slashes
///
/// Alias for :c:type:`cuddl_size_t`.
///
/// \endverbatim
using size_t = cuddl_size_t;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper class for :c:type:`cuddl_resource_id`.
///
/// The stream output operator is overloaded for instances of this class.
///
/// \endverbatim
class ResourceID
{
public:
	/// @name Constructors
	/// @{
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
        ///  @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_resource_id() const {return id;}

	/// @name Getter Functions
	/// @{
	std::string group()    const {return id.group;}
	std::string device()   const {return id.device;}
	std::string resource() const {return id.resource;}
	int         instance() const {return id.instance;}
        ///  @}

	/// @name Setter Functions
	/// @{
	void group   (const char *s) {strncpy(id.group,    s, MAX_STR_LEN);}
	void device  (const char *s) {strncpy(id.device,   s, MAX_STR_LEN);}
	void resource(const char *s) {strncpy(id.resource, s, MAX_STR_LEN);}
	void instance(int i)         {id.instance = i;}
        ///  @}

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

} // namespace cuddl

#endif /* !_CUDDL_GENERAL_HPP */
