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

#ifndef _CUDDL_UTILITY_HPP
#define _CUDDL_UTILITY_HPP

// C++ utility declarations.

extern "C" {
#include <cuddl.h>
}

#include <string>
#include <system_error>
#include <sstream>
#include <ostream>

// Xenomai-specific code
#ifdef __XENO__
#  include <pthread.h>
#else
#  define pthread_setmode_np(a,b,c)
#  define PTHREAD_WARNSW 0
#endif

///
/// Provides the Cuddl C++ API.
///
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

/// \verbatim embed:rst:leading-slashes
///
/// C++ class template used to implement :cpp:type:`MemRegionFlags`,
/// :cpp:type:`MemRegionClaimFlags`, :cpp:type:`MemRegionMapFlags`,
/// :cpp:type:`EventSrcFlags`, :cpp:type:`EventSrcClaimFlags`, and
/// :cpp:type:`EventSrcOpenFlags`.
///
/// \endverbatim
template<class F>
class Flags {
public:
	/// @name Constructors
	/// @{
	Flags(int flags=0) : flags_{flags} {}
	Flags(const F &f) {set(f);}
        ///  @}

	/// Return an integer representation of the flags.
	int as_int() const {return flags_;}

	/// Test if a specified flag bit is set.
	bool is_set(const F &f) const {
		return flags_ & static_cast<int>(f);
	}

	/// Set the specified flag bit.
	void set(const F &f) {
		flags_ |= static_cast<int>(f);
	}
	/// Clear the specified flag bit.
	void clear(const F &f) {
		flags_ &= ~static_cast<int>(f);
	}

	/// Set the specified flag bits.
	void set(const Flags &f) {
		flags_ |= f.as_int();
	}
	/// Clear the specified flag bits.
	void clear(const Flags &f) {
		flags_ &= ~f.as_int();
	}

        /// \verbatim embed:rst:leading-slashes
        ///
        /// Alias for `set`.
        ///
        /// \endverbatim
	Flags &operator |=(const F &r) {
		flags_ |= static_cast<int>(r);
		return *this;
	}

        /// \verbatim embed:rst:leading-slashes
        ///
        /// Alias for `set`.
        ///
        /// \endverbatim
	Flags &operator |=(const Flags &r) {
		flags_ |= r.flags_;
		return *this;
	}

        /// Combine flags together.
	Flags operator | (const F &r) {
		Flags e;
		e |= *this;
		e |= r;
		return e;
	}

        /// Combine sets of flags together.
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

} // namespace cuddl

#endif /* !_CUDDL_UTILITY_HPP */
