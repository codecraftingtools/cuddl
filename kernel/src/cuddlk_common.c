/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common kernel implementation.
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

/*
 * Code common to all kernel implementations.
 */

#include <cuddl/kernel.h>

int cuddlk_get_commit_id(char *id_str, cuddlk_size_t id_len)
{
	int remaining_space;

	if (CUDDLK_IMPL_REPO_IS_DIRTY) {
		strncpy(id_str, "M", id_len);
	} else {
		strncpy(id_str, "", id_len);
	}
	remaining_space = id_len - strnlen(id_str, id_len);
	if (remaining_space > 0) {
		remaining_space -= 1;
		strncat(id_str, CUDDLK_IMPL_COMMIT_HASH, remaining_space);
	}
	return 0;
}

int cuddlk_device_find_memregion(
	struct cuddlk_device *dev, const char *name)
{
	int i;
	int found_match = 0;
	struct cuddlk_memregion *mem = NULL;

	for (i=0; i<CUDDLK_MAX_DEV_MEM_REGIONS; i++) {
		mem = &dev->mem[i];
		if (!mem->name)
			continue;
		if (strncmp(mem->name, name, CUDDLK_MAX_STR_LEN) != 0)
			continue;
		found_match = 1;
		break;
	}
	if (!found_match)
		return -ENXIO;
	
	return i;
}

int cuddlk_device_find_eventsrc(
	struct cuddlk_device *dev, const char *name)
{
	int i;
	int found_match = 0;
	struct cuddlk_eventsrc *eventsrc = NULL;

	for (i=0; i<CUDDLK_MAX_DEV_EVENTS; i++) {
		eventsrc = &dev->events[i];
		if (!eventsrc->name)
			continue;
		if (strncmp(eventsrc->name, name, CUDDLK_MAX_STR_LEN) != 0)
			continue;
		found_match = 1;
		break;
	}
	if (!found_match)
		return -ENXIO;

	return i;
}
