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
		   const char *resource="", int instance=0) {
		memset(&id, 0, sizeof(id));
		this->group(group);
		this->device(device);
		this->resource(resource);
		this->instance(instance);
	}
	ResourceID(const std::string &full_name) {
		this->full_name(full_name);
	}
        ///  @}

	/// Cast operator for converting class instances to the equivalent C
	/// structure.
	operator cuddl_resource_id() const {return id;}

	/// @name Getter Functions
	/// @{
	std::string group()     const {return id.group;}
	std::string device()    const {return id.device;}
	std::string resource()  const {return id.resource;}
	int         instance()  const {return id.instance;}
	inline std::string full_name() const;
        ///  @}

	/// @name Setter Functions
	/// @{
	void group   (const char *s) {strncpy(id.group,    s, MAX_STR_LEN-1);}
	void device  (const char *s) {strncpy(id.device,   s, MAX_STR_LEN-1);}
	void resource(const char *s) {strncpy(id.resource, s, MAX_STR_LEN-1);}
	void instance(int i)         {id.instance = i;}
	inline void full_name(const std::string &name);
        ///  @}

private:
	cuddl_resource_id id;
};

inline std::string ResourceID::full_name() const
{
	std::string group    {this->group() };
	std::string device   {this->device()};
	std::string resource {this->resource()};
	int         instance {this->instance()};

	if (group    == "") group="*";
	if (device   == "") device="*";
	if (resource == "") resource="*";

	std::string full = group + "/" + device + "/" + resource + "/";

	if (instance)
		full += std::to_string(instance);
	else
		full += "*";

	return full;
}

inline void ResourceID::full_name(const std::string &name)
{
	using namespace std;
	memset(&id, 0, sizeof(id));

	auto i1 = name.find('/');
	if (i1 == string::npos) {
		device(name.c_str());
	} else {
		group(name.substr(0, i1).c_str());
		auto i2 = name.find('/', i1 + 1);
		if (i2 == string::npos) {
			device(name.substr(i1 + 1, string::npos)
			       .c_str());
		} else {
			device(name.substr(i1 + 1, i2-i1-1).c_str());
			i1 = i2;
			i2 = name.find('/', i1 + 1);
			if (i2 == string::npos) {
				resource(
					name.substr(i1 + 1, string::npos)
					.c_str());
			} else {
				resource(
					name.substr(i1 + 1, i2-i1-1)
					.c_str());
				auto instance_str = name.substr(
					i2 + 1,
					string::npos);
				if (instance_str.length() > 0) {
					try {
						instance(stoi(instance_str
							      .c_str()));
					} catch (exception &e) {
						instance(-1);
					}
				}
			}
		}
	}
}

inline std::ostream &operator <<(std::ostream &os, const ResourceID &id)
{
	os << id.full_name();
	return os;
}

//----------------------------------------------------------------------------
// C++ Utility Code for Internal Use
//----------------------------------------------------------------------------

inline void throw_resource_id_err(int code, std::string const &msg,
                                  const cuddl_resource_id &id)
{
	throw_err(code, msg + " for cuddl resource '" +
		  ResourceID(id).full_name() + "'");
}

} // namespace cuddl

#endif /* !_CUDDL_GENERAL_HPP */
