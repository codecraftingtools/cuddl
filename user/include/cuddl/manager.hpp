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

#ifndef _CUDDL_MANAGER_HPP
#define _CUDDL_MANAGER_HPP

// C++ device manager declarations.

#include <cuddl/memregion.hpp>
#include <cuddl/eventsrc.hpp>

namespace cuddl {

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_max_managed_devices`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int get_max_managed_devices()
{
	int ret = cuddl_get_max_managed_devices();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_max_dev_mem_regions`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int get_max_dev_mem_regions()
{
	int ret = cuddl_get_max_dev_mem_regions();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_max_dev_events`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int get_max_dev_events()
{
	int ret = cuddl_get_max_dev_events();
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_memregion_id_for_slot`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline ResourceID get_memregion_id_for_slot(int device_slot, int mem_slot)
{
	cuddl_resource_id id;

	int ret = cuddl_get_memregion_id_for_slot(&id, device_slot, mem_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return ResourceID(id);
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_eventsrc_id_for_slot`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline ResourceID get_eventsrc_id_for_slot(int device_slot, int event_slot)
{
	cuddl_resource_id id;

	int ret = cuddl_get_eventsrc_id_for_slot(
		&id, device_slot, event_slot);
	if (ret < 0) { throw_err(ret, __func__); }
	return ResourceID(id);
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_memregion_info_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline MemRegionInfo get_memregion_info_for_id(const cuddl_resource_id &id)
{
	cuddl_memregion_info info;

	int ret = cuddl_get_memregion_info_for_id(&info, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return MemRegionInfo(info);
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_eventsrc_info_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline EventSrcInfo get_eventsrc_info_for_id(const cuddl_resource_id &id)
{
	cuddl_eventsrc_info info;

	int ret = cuddl_get_eventsrc_info_for_id(&info, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return EventSrcInfo(info);
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_memregion_ref_count_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int get_memregion_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_get_memregion_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_eventsrc_ref_count_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int get_eventsrc_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_get_eventsrc_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_decrement_memregion_ref_count_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int decrement_memregion_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_decrement_memregion_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_decrement_eventsrc_ref_count_for_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline int decrement_eventsrc_ref_count_for_id(const cuddl_resource_id &id)
{
	int ret = cuddl_decrement_eventsrc_ref_count_for_id(&id);
	if (ret < 0) { throw_err(ret, __func__); }
	return ret;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_driver_info_for_memregion_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_driver_info_for_memregion_id(
	const cuddl_resource_id &id)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_driver_info_for_memregion_id(
		s, MAX_STR_LEN, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_driver_info_for_eventsrc_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_driver_info_for_eventsrc_id(
	const cuddl_resource_id &id)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_driver_info_for_eventsrc_id(
		s, MAX_STR_LEN, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_hw_info_for_memregion_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_hw_info_for_memregion_id(const cuddl_resource_id &id)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_hw_info_for_memregion_id(
		s, MAX_STR_LEN, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

/// \verbatim embed:rst:leading-slashes
///
/// C++ wrapper for :c:func:`cuddl_get_hw_info_for_eventsrc_id`.
///
/// \endverbatim
/// @throws std::system_error Operation failed.
inline std::string get_hw_info_for_eventsrc_id(const cuddl_resource_id &id)
{
	char s[MAX_STR_LEN];
	int ret = cuddl_get_hw_info_for_eventsrc_id(
		s, MAX_STR_LEN, &id);
	if (ret < 0) { throw_err(ret, __func__); }
	return s;
}

} // namespace cuddl

#endif /* !_CUDDL_MANAGER_HPP */
