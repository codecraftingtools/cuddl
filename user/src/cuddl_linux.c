/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer Linux user-space impl.
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
 * Linux user-space implementation.
 */

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#include <cuddl.h>
#include <cuddl/common_impl_linux_ioctl.h>

#ifdef __XENO__
#define CUDDLCI_MEMREGION_CLAIM_IOCTL CUDDLCI_MEMREGION_CLAIM_UDD_IOCTL
#define CUDDLCI_EVENTSRC_CLAIM_IOCTL  CUDDLCI_EVENTSRC_CLAIM_UDD_IOCTL
#define CUDDLCI_MEMREGION_RELEASE_IOCTL CUDDLCI_MEMREGION_RELEASE_UDD_IOCTL
#define CUDDLCI_EVENTSRC_RELEASE_IOCTL  CUDDLCI_EVENTSRC_RELEASE_UDD_IOCTL
#else
#define CUDDLCI_MEMREGION_CLAIM_IOCTL CUDDLCI_MEMREGION_CLAIM_UIO_IOCTL
#define CUDDLCI_EVENTSRC_CLAIM_IOCTL  CUDDLCI_EVENTSRC_CLAIM_UIO_IOCTL
#define CUDDLCI_MEMREGION_RELEASE_IOCTL CUDDLCI_MEMREGION_RELEASE_UIO_IOCTL
#define CUDDLCI_EVENTSRC_RELEASE_IOCTL  CUDDLCI_EVENTSRC_RELEASE_UIO_IOCTL
#endif

#ifndef CUDDLI_REPO_IS_DIRTY
#define CUDDLI_REPO_IS_DIRTY 0
#endif
#ifndef CUDDLI_COMMIT_HASH
#define CUDDLI_COMMIT_HASH "UNKNOWN"
#endif

#include <stdio.h>

static int cuddli_janitor_fd;

int cuddl_get_kernel_version_code(void)
{
	int fd;
	int ret, ret2;
	struct cuddlci_void_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_KERNEL_VERSION_CODE_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_get_kernel_commit_id(char *id_str, cuddl_size_t id_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_kernel_commit_id_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_KERNEL_COMMIT_ID_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (id_len > CUDDLCI_MAX_STR_LEN) {
		id_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = id_len;
	}
	strncpy(id_str, s.id_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_userspace_commit_id(char *id_str, cuddl_size_t id_len)
{
	int remaining_space;

	if (CUDDLI_REPO_IS_DIRTY) {
		strncpy(id_str, "M", id_len);
	} else {
		strncpy(id_str, "", id_len);
	}
	remaining_space = id_len - strnlen(id_str, id_len);
	if (remaining_space > 0) {
		remaining_space -= 1;
		strncat(id_str, CUDDLI_COMMIT_HASH, remaining_space);
	}
	return 0;
}

int cuddl_get_kernel_variant(char *str, cuddl_size_t id_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_kernel_commit_id_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_KERNEL_VARIANT_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (id_len > CUDDLCI_MAX_STR_LEN) {
		str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = id_len;
	}
	strncpy(str, s.id_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

static void populate_id_from_args(
	struct cuddl_resource_id *id,
	const char *group,
	const char *device,
	const char *resource,
	int instance)
{
	if (group)
		strncpy(id->group, group, CUDDL_MAX_STR_LEN);
	else
		id->group[0] = '\0';
	if (device)
		strncpy(id->device, device, CUDDL_MAX_STR_LEN);
	else
		id->device[0] = '\0';
	if (resource)
		strncpy(id->resource, resource, CUDDL_MAX_STR_LEN);
	else
		id->resource[0] = '\0';
	id->instance = instance;
}

int cuddl_memregion_claim(
	struct cuddl_memregion_info *meminfo,
	const char *group,
	const char *device,
	const char *memregion,
	int instance,
	int options)
{
	int fd;
	int ret;
	struct cuddlci_memregion_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	populate_id_from_args(&s.id, group, device, memregion, instance);
	s.pid = getpid();
	s.options = options;

	ret = ioctl(fd, CUDDLCI_MEMREGION_CLAIM_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(meminfo, &s.info, sizeof(*meminfo));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_memregion_release(struct cuddl_memregion_info *meminfo)
{
	return cuddli_memregion_release_by_token(meminfo->priv.token);
}

int cuddl_memregion_map(
	struct cuddl_memregion *memregion,
	const struct cuddl_memregion_info *meminfo,
	int options)
{
	int fd;
	void *addr;
	int ret;

	fd = open(meminfo->priv.device_name, O_RDWR);
	if (fd < 0)
		return -errno;

	addr = mmap(
		NULL,
		meminfo->priv.pa_len,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,
		fd,
		meminfo->priv.pa_mmap_offset);
	if (addr == (void *) -1) {
		ret = -errno;
		close(fd);
		return ret;
	}

	memregion->priv.fd = fd;
	memregion->priv.pa_len = meminfo->priv.pa_len;
	memregion->priv.pa_addr = (unsigned long) addr;
	memregion->addr = (void *) (
		memregion->priv.pa_addr + meminfo->priv.start_offset);
	memregion->len = meminfo->len;
	memregion->flags = meminfo->flags;
	memregion->priv.token = meminfo->priv.token;

	return 0;
}


int cuddl_memregion_unmap(struct cuddl_memregion *memregion)
{
	int ret = 0;
	int err;

	err = munmap((void*)memregion->priv.pa_addr, memregion->priv.pa_len);
	if (err == -1)
		ret = -errno;

	err = close(memregion->priv.fd);
	if ((err == -1) && (ret == 0))
		ret = -errno;

	return ret;
}

int cuddl_memregion_claim_and_map(
	struct cuddl_memregion *memregion,
	const char *group,
	const char *device,
	const char *resource,
	int instance,
	int claim_options,
	int map_options)
{
	int ret;
	struct cuddl_memregion_info meminfo;

	ret = cuddl_memregion_claim(
		&meminfo, group, device, resource, instance, claim_options);
	if (ret)
		return ret;

	ret = cuddl_memregion_map(memregion, &meminfo, map_options);
	if (ret)
		cuddl_memregion_release(&meminfo);
	return ret;
}

int cuddl_memregion_unmap_and_release(struct cuddl_memregion *memregion)
{
	int ret1, ret2;

	ret1 = cuddl_memregion_unmap(memregion);
	ret2 = cuddli_memregion_release_by_token(memregion->priv.token);
	if (ret1)
		return ret1;
	return ret2;
}

int cuddli_memregion_release_by_token(struct cuddlci_token token)
{
	int fd;
	int ret;
	struct cuddlci_memregion_release_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.token = token;
	s.pid = getpid();

	ret = ioctl(fd, CUDDLCI_MEMREGION_RELEASE_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_memregion_get_driver_info(
	struct cuddl_memregion *memregion, char *info_str,
	cuddl_size_t info_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = memregion->priv.token.device_index;

	ret = ioctl(fd, CUDDLCI_GET_DRIVER_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_memregion_get_hw_info(
	struct cuddl_memregion *memregion, char *info_str,
	cuddl_size_t info_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = memregion->priv.token.device_index;

	ret = ioctl(fd, CUDDLCI_GET_HW_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_claim(
	struct cuddl_eventsrc_info *eventinfo,
	const char *group,
	const char *device,
	const char *eventsrc,
	int instance,
	int options)
{
	int fd;
	int ret;
	struct cuddlci_eventsrc_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	populate_id_from_args(&s.id, group, device, eventsrc, instance);
	s.pid = getpid();
	s.options = options;

	ret = ioctl(fd, CUDDLCI_EVENTSRC_CLAIM_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(eventinfo, &s.info, sizeof(*eventinfo));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_release(struct cuddl_eventsrc_info *eventinfo)
{
	return cuddli_eventsrc_release_by_token(eventinfo->priv.token);
}

int cuddl_eventsrc_open(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_eventsrc_info *eventinfo,
	int options)
{
	int fd;

	fd = open(eventinfo->priv.device_name, O_RDWR);
	if (fd < 0)
		return -errno;

	eventsrc->flags = eventinfo->flags;
	eventsrc->priv.token = eventinfo->priv.token;
	eventsrc->priv.fd = fd;

	return 0;
}

int cuddl_eventsrc_close(struct cuddl_eventsrc *eventsrc)
{
	int ret;

	ret = close(eventsrc->priv.fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_claim_and_open(
	struct cuddl_eventsrc *eventsrc,
	const char *group,
	const char *device,
	const char *resource,
	int instance,
	int claim_options,
	int open_options)
{
	int ret;
	struct cuddl_eventsrc_info eventinfo;

	ret = cuddl_eventsrc_claim(
		&eventinfo, group, device, resource, instance, claim_options);
	if (ret)
		return ret;

	ret = cuddl_eventsrc_open(eventsrc, &eventinfo, open_options);
	if (ret)
		cuddl_eventsrc_release(&eventinfo);
	return ret;
}

int cuddl_eventsrc_close_and_release(struct cuddl_eventsrc *eventsrc)
{
	int ret1, ret2;

	ret1 = cuddl_eventsrc_close(eventsrc);
	ret2 = cuddli_eventsrc_release_by_token(eventsrc->priv.token);
	if (ret1)
		return ret1;
	return ret2;
}

int cuddli_eventsrc_release_by_token(struct cuddlci_token token)
{
	int fd;
	int ret;
	struct cuddlci_eventsrc_release_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.token = token;
	s.pid = getpid();

	ret = ioctl(fd, CUDDLCI_EVENTSRC_RELEASE_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_wait(struct cuddl_eventsrc *eventsrc)
{
	ssize_t n_bytes_read;
	uint32_t count = 0;

	n_bytes_read = read(eventsrc->priv.fd, &count, sizeof(count));
	if (n_bytes_read == -1)
		return -errno;

	return n_bytes_read;
}

int cuddl_eventsrc_trywait(struct cuddl_eventsrc *eventsrc)
{
	struct cuddl_timespec timeout;

	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;

	return cuddl_eventsrc_timed_wait(eventsrc, &timeout);
}

int cuddl_eventsrc_timed_wait(
	struct cuddl_eventsrc *eventsrc,
	const struct cuddl_timespec *timeout)
{
	fd_set fds;
	int ret;
	ssize_t n_bytes_read;
	uint32_t count = 0;

	FD_ZERO(&fds);
	FD_SET(eventsrc->priv.fd, &fds);

	struct timeval tv;
	tv.tv_sec = timeout->tv_sec;
	tv.tv_usec = timeout->tv_nsec/1000;

	ret = select(eventsrc->priv.fd + 1, &fds, NULL, NULL, &tv);
	if (ret == -1)
		return -errno;

	if (!FD_ISSET(eventsrc->priv.fd, &fds))
		return -ETIMEDOUT;

	n_bytes_read = read(eventsrc->priv.fd, &count, sizeof(count));
	if (n_bytes_read == -1)
		return -errno;

	return n_bytes_read;
}

int cuddl_eventsrc_enable(struct cuddl_eventsrc *eventsrc)
{
	int ret;
	uint32_t value = 1;

	ret = write(eventsrc->priv.fd, &value, 4);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_disable(struct cuddl_eventsrc *eventsrc)
{
	int ret;
	uint32_t value = 0;

	ret = write(eventsrc->priv.fd, &value, 4);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_get_driver_info(
	struct cuddl_eventsrc *eventsrc, char *info_str,
	cuddl_size_t info_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = eventsrc->priv.token.device_index;

	ret = ioctl(fd, CUDDLCI_GET_DRIVER_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_get_hw_info(
	struct cuddl_eventsrc *eventsrc, char *info_str,
	cuddl_size_t info_len)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = eventsrc->priv.token.device_index;

	ret = ioctl(fd, CUDDLCI_GET_HW_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_max_managed_devices(void)
{
	int fd;
	int ret, ret2;
	struct cuddlci_void_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_MAX_MANAGED_DEVICES_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_get_max_dev_mem_regions(void)
{
	int fd;
	int ret, ret2;
	struct cuddlci_void_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_MAX_DEV_MEM_REGIONS_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_get_max_dev_events(void)
{
	int fd;
	int ret, ret2;
	struct cuddlci_void_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;

	ret = ioctl(fd, CUDDLCI_GET_MAX_DEV_EVENTS_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_get_driver_info_for_slot(
	char *info_str, cuddl_size_t info_len, int device_slot)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = device_slot;

	ret = ioctl(fd, CUDDLCI_GET_DRIVER_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_hw_info_for_slot(
	char *info_str, cuddl_size_t info_len, int device_slot)
{
	int fd;
	int ret;
	int len;
	struct cuddlci_get_driver_info_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = device_slot;

	ret = ioctl(fd, CUDDLCI_GET_HW_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	if (info_len > CUDDLCI_MAX_STR_LEN) {
		info_str[CUDDLCI_MAX_STR_LEN] = '\0';
		len = CUDDLCI_MAX_STR_LEN;
	} else {
		len = info_len;
	}
	strncpy(info_str, s.info_str, len);

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_memregion_id_for_slot(
	struct cuddl_resource_id *id, int device_slot, int mem_slot)
{
	int fd;
	int ret;
	struct cuddlci_get_resource_id_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = device_slot;
	s.resource_slot = mem_slot;

	ret = ioctl(fd, CUDDLCI_GET_MEMREGION_ID_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(id, &s.id, sizeof(*id));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_eventsrc_id_for_slot(
	struct cuddl_resource_id *id, int device_slot, int event_slot)
{
	int fd;
	int ret;
	struct cuddlci_get_resource_id_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	s.device_slot = device_slot;
	s.resource_slot = event_slot;

	ret = ioctl(fd, CUDDLCI_GET_EVENTSRC_ID_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(id, &s.id, sizeof(*id));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_memregion_info_for_id(
	struct cuddl_memregion_info *meminfo,
	const struct cuddl_resource_id *memregion_id)
{
	int fd;
	int ret;
	struct cuddlci_memregion_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, memregion_id, sizeof(s.id));
	s.pid = getpid();

	ret = ioctl(fd, CUDDLCI_GET_MEMREGION_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(meminfo, &s.info, sizeof(*meminfo));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_eventsrc_info_for_id(
	struct cuddl_eventsrc_info *eventinfo,
	const struct cuddl_resource_id *eventsrc_id)
{
	int fd;
	int ret;
	struct cuddlci_eventsrc_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, eventsrc_id, sizeof(s.id));
	s.pid = getpid();

	ret = ioctl(fd, CUDDLCI_GET_EVENTSRC_INFO_IOCTL, &s);
	if (ret) {
		if ((ret == -1) && errno)
			ret = -errno;
		close(fd);
		return ret;
	}

	memcpy(eventinfo, &s.info, sizeof(*eventinfo));

	ret = close(fd);
	if (ret == -1)
		return -errno;

	return 0;
}

int cuddl_get_memregion_ref_count_for_id(
	const struct cuddl_resource_id *memregion_id)
{
	int fd;
	int ret, ret2;
	struct cuddlci_ref_count_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, memregion_id, sizeof(s.id));

	ret = ioctl(fd, CUDDLCI_GET_MEMREGION_REF_COUNT_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_get_eventsrc_ref_count_for_id(
	const struct cuddl_resource_id *eventsrc_id)
{
	int fd;
	int ret, ret2;
	struct cuddlci_ref_count_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, eventsrc_id, sizeof(s.id));

	ret = ioctl(fd, CUDDLCI_GET_EVENTSRC_REF_COUNT_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_decrement_memregion_ref_count_for_id(
	const struct cuddl_resource_id *memregion_id)
{
	int fd;
	int ret, ret2;
	struct cuddlci_ref_count_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, memregion_id, sizeof(s.id));

	ret = ioctl(
		fd, CUDDLCI_DECREMENT_MEMREGION_REF_COUNT_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddl_decrement_eventsrc_ref_count_for_id(
	const struct cuddl_resource_id *eventsrc_id)
{
	int fd;
	int ret, ret2;
	struct cuddlci_ref_count_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.version_code = CUDDL_VERSION_CODE;
	memcpy(&s.id, eventsrc_id, sizeof(s.id));

	ret = ioctl(fd, CUDDLCI_DECREMENT_EVENTSRC_REF_COUNT_IOCTL, &s);
	if ((ret == -1) && errno)
		ret = -errno;

	ret2 = close(fd);
	if ((ret2 == -1) && (ret >= 0))
		return -errno;

	return ret;
}

int cuddli_open_janitor(void)
{
	const char *janitor_dev_name = "/dev/cuddl_janitor";
	int fd;
	int ret;
	struct cuddlci_janitor_pid_ioctl_data s;

#ifdef __XENO__
	fd = __real_open(janitor_dev_name, O_RDWR);
#else
	fd = open(janitor_dev_name, O_RDWR);
#endif
	if (fd == -1) {
		printf("warning: could not open janitor device: %s\n",
		       janitor_dev_name);
		return -errno;
	}

	s.version_code = CUDDL_VERSION_CODE;
	s.pid = getpid();

#ifdef __XENO__
	ret = __real_ioctl(fd, CUDDLCI_JANITOR_REGISTER_PID_IOCTL, &s);
#else
	ret = ioctl(fd, CUDDLCI_JANITOR_REGISTER_PID_IOCTL, &s);
#endif
	if (ret == -1) {
		printf("warning: IOCTL error on janitor device\n");
		return -errno;
	}

	// Leave the janitor file descriptor open
	return fd;
}

int cuddli_close_janitor(int fd)
{
	int ret = 0;

	if (fd > 0) {
#ifdef __XENO__
		ret = __real_close(fd);
#else
		ret = close(fd);
#endif
	}

	return ret;
}

__attribute__((constructor)) void cuddl_startup(void)
{
	cuddli_janitor_fd = cuddli_open_janitor();
	if (cuddli_janitor_fd == -1) {
		printf("warning: could not close janitor device\n");
		return;
	}
}

__attribute__((destructor)) void cuddl_cleanup(void)
{
	cuddli_close_janitor(cuddli_janitor_fd);
}
