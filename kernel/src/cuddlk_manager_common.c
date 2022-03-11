/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer common kernel manager impl.
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
 * Manager code common to all kernel implementations.
 */

#include <cuddl/kernel.h>

struct cuddlk_manager *cuddlk_global_manager;

int cuddlk_manager_find_device_matching(
	const char *group, const char *name, const char *resource,
	int instance, enum cuddlk_resource type, int start_index)
{
	int i;
	int found_match = 0;
	struct cuddlk_device *dev = NULL;
	
	for (i=start_index; i<CUDDLK_MAX_MANAGED_DEVICES; i++) {
		dev = cuddlk_global_manager->devices[i];
		if (!dev)
			continue;
		if (instance)
			if (dev->instance != instance)
				continue;
		if (group)
			if (strncmp(dev->group, group, CUDDLK_MAX_STR_LEN)
			    != 0) continue;
		if (name)
			if (strncmp(dev->name, name, CUDDLK_MAX_STR_LEN)
			    != 0) continue;
		if (resource) {
			if (type == CUDDLK_RESOURCE_MEMREGION) {
				if (cuddlk_device_find_memregion(
					    dev, resource) < 0)
					continue;
			} else if (type == CUDDLK_RESOURCE_EVENTSRC) {
				if (cuddlk_device_find_eventsrc(
					    dev, resource) < 0)
					continue;
			} else {
				continue;
			}
		}
		found_match = 1;
		break;
	}
	if (!found_match)
		return -ENXIO;

	return i;
}

int cuddlk_manager_find_device(struct cuddlk_device *dev)
{
	int i;
	int found_slot = 0;

	for (i=0; i<CUDDLK_MAX_MANAGED_DEVICES; i++) {
		if (cuddlk_global_manager->devices[i] != dev)
			continue;
		found_slot = 1;
		break;
	}
	if (!found_slot)
		return -ENXIO;

	return i;
}

int cuddlk_manager_find_empty_slot(void)
{
	return cuddlk_manager_find_device(NULL);
}


int cuddlk_manager_add_device(struct cuddlk_device *dev)
{
	int slot;

	printk("Cuddl managing %s %s %d\n",
	       dev->group, dev->name, dev->instance);

	slot = cuddlk_manager_find_empty_slot();
	if (slot < 0)
		return -ENOMEM;

	cuddlk_global_manager->devices[slot] = dev;

	return 0;
}

int cuddlk_manager_remove_device(struct cuddlk_device *dev)
{
	int slot;

	printk("Cuddl releasing %s %s %d\n",
	       dev->group, dev->name, dev->instance);

	slot = cuddlk_manager_find_device(dev);
	if (slot < 0)
		return slot;
	
	cuddlk_global_manager->devices[slot] = NULL;

	return 0;
}

int cuddlk_device_manage(struct cuddlk_device *dev)
{
	int ret;

	ret = cuddlk_device_register(dev);
	if (ret)
		goto fail_register;

	ret = cuddlk_manager_add_device(dev);
	if (ret)
		goto fail_manager_add_device;

	return 0;

fail_manager_add_device:
	cuddlk_device_unregister(dev);
fail_register:
	return ret;
}

int cuddlk_device_release(struct cuddlk_device *dev)
{
	int ret;

	ret = cuddlk_manager_remove_device(dev);
	ret = cuddlk_device_unregister(dev);
	return ret;
}
