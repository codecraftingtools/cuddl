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

#include <cuddlk.h>

int cuddlk_manager_find_device_slot_matching(
	struct cuddlk_manager *manager,
	const char *group, const char *name, const char *resource,
	int instance, enum cuddlk_resource type, int start_index)
{
	int i;
	int found_match = 0;
	struct cuddlk_device *dev = NULL;

	for (i=start_index; i<CUDDLK_MAX_MANAGED_DEVICES; i++) {
		dev = manager->devices[i];
		if (!dev)
			continue;
		if (instance)
			if (dev->instance != instance)
				continue;
		if (group && (strnlen(group, CUDDLK_MAX_STR_LEN) > 0))
			if (strncmp(dev->group, group, CUDDLK_MAX_STR_LEN)
			    != 0) continue;
		if (name && (strnlen(name, CUDDLK_MAX_STR_LEN) > 0))
			if (strncmp(dev->name, name, CUDDLK_MAX_STR_LEN)
			    != 0) continue;
		if (resource && (strnlen(resource, CUDDLK_MAX_STR_LEN) > 0)) {
			if (type == CUDDLK_RESOURCE_MEMREGION) {
				if (cuddlk_device_find_memregion_slot(
					    dev, resource) < 0)
					continue;
			} else if (type == CUDDLK_RESOURCE_EVENTSRC) {
				if (cuddlk_device_find_eventsrc_slot(
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

int cuddlk_manager_find_device_slot(
	struct cuddlk_manager *manager, struct cuddlk_device *dev)
{
	int i;
	int found_slot = 0;

	for (i=0; i<CUDDLK_MAX_MANAGED_DEVICES; i++) {
		if (manager->devices[i] != dev)
			continue;
		found_slot = 1;
		break;
	}
	if (!found_slot)
		return -ENXIO;

	return i;
}

int cuddlk_manager_find_empty_slot(struct cuddlk_manager *manager)
{
	return cuddlk_manager_find_device_slot(manager, NULL);
}

int  cuddlk_manager_next_available_instance_id(
	struct cuddlk_manager *manager, struct cuddlk_device *dev)
{
	int instance;
	int slot;

	instance = 1;
	slot = 0;
	while (1) {
		slot = cuddlk_manager_find_device_slot_matching(
			manager, dev->group, dev->name,  NULL, instance, 0,
			slot);
		if (slot < 0)
			break;
		if (slot < CUDDLK_MAX_MANAGED_DEVICES - 1) {
			slot += 1;
			instance += 1;
		} else {
			instance = -ENOMEM;
			break;
		}
	}

	return instance;
}

int cuddlk_manager_add_device(
	struct cuddlk_manager * manager, struct cuddlk_device *dev)
{
	int slot;

	slot = cuddlk_manager_find_empty_slot(manager);
	if (slot < 0)
		return -ENOMEM;

	manager->devices[slot] = dev;

	return 0;
}

int cuddlk_manager_remove_device(
	struct cuddlk_manager * manager, struct cuddlk_device *dev)
{
	int slot;

	slot = cuddlk_manager_find_device_slot(manager, dev);
	if (slot < 0)
		return slot;
	
	manager->devices[slot] = NULL;

	return 0;
}

int cuddlk_device_manage(struct cuddlk_device *dev)
{
	int ret;
	struct cuddlk_manager *manager;

	manager = cuddlk_manager_get();

	if (!dev->group) {
		ret = -EINVAL;
		goto fail_null_group;
	}
	if (!dev->name) {
		ret = -EINVAL;
		goto fail_null_name;
	}
	if (!dev->instance) {
		ret = cuddlk_manager_next_available_instance_id(
		      manager, dev);
		if (ret < 0) {
			goto fail_auto_instance;
		}
		dev->instance = ret;
	}

	ret = cuddlk_device_register(dev);
	if (ret)
		goto fail_register;

	ret = cuddlk_manager_add_device(manager, dev);
	if (ret)
		goto fail_manager_add_device;

	return 0;

fail_manager_add_device:
	cuddlk_device_unregister(dev);
fail_register:
fail_auto_instance:
fail_null_name:
fail_null_group:
	return ret;
}

int cuddlk_device_release(struct cuddlk_device *dev)
{
	int ret1, ret2;
	struct cuddlk_manager *manager;

	manager = cuddlk_manager_get();
	ret1 = cuddlk_manager_remove_device(manager, dev);
	ret2 = cuddlk_device_unregister(dev);
	if (ret1)
		return ret1;
	return ret2;
}
