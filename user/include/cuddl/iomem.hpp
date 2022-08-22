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

#ifndef _CUDDL_IOMEM_HPP
#define _CUDDL_IOMEM_HPP

// C++ declarations for I/O memory access.

#include <cuddl/utility.hpp>

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// Alias for :c:type:`cuddl_iomem_t`.
///
/// \endverbatim
using iomem_t = cuddl_iomem_t;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_ioread8`.
///
/// \endverbatim
const auto ioread8  = cuddl_ioread8;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_ioread16`.
///
/// \endverbatim
const auto ioread16 = cuddl_ioread16;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_ioread32`.
///
/// \endverbatim
const auto ioread32 = cuddl_ioread32;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_iowrite8`.
///
/// \endverbatim
const auto iowrite8  = cuddl_iowrite8;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_iowrite16`.
///
/// \endverbatim
const auto iowrite16 = cuddl_iowrite16;

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_iowrite32`.
///
/// \endverbatim
const auto iowrite32 = cuddl_iowrite32;

} // namespace cuddl

#endif /* !_CUDDL_IOMEM_HPP */
