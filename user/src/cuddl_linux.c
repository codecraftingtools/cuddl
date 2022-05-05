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
#define CUDDL_MEMREGION_CLAIM_IOCTL CUDDL_MEMREGION_CLAIM_UDD_IOCTL
#define CUDDL_EVENTSRC_CLAIM_IOCTL  CUDDL_EVENTSRC_CLAIM_UDD_IOCTL
#define CUDDL_MEMREGION_RELEASE_IOCTL CUDDL_MEMREGION_RELEASE_UDD_IOCTL
#define CUDDL_EVENTSRC_RELEASE_IOCTL  CUDDL_EVENTSRC_RELEASE_UDD_IOCTL
#else
#define CUDDL_MEMREGION_CLAIM_IOCTL CUDDL_MEMREGION_CLAIM_UIO_IOCTL
#define CUDDL_EVENTSRC_CLAIM_IOCTL  CUDDL_EVENTSRC_CLAIM_UIO_IOCTL
#define CUDDL_MEMREGION_RELEASE_IOCTL CUDDL_MEMREGION_RELEASE_UIO_IOCTL
#define CUDDL_EVENTSRC_RELEASE_IOCTL  CUDDL_EVENTSRC_RELEASE_UIO_IOCTL
#endif

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
	struct cuddl_memregion_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	if (group)
		strncpy(s.id.group, group, CUDDL_MAX_STR_LEN);
	else
		s.id.group[0] = '\0';
	if (device)
		strncpy(s.id.device, device, CUDDL_MAX_STR_LEN);
	else
		s.id.device[0] = '\0';
	if (memregion)
		strncpy(s.id.resource, memregion, CUDDL_MAX_STR_LEN);
	else
		s.id.resource[0] = '\0';
	s.id.instance = instance;

	ret = ioctl(fd, CUDDL_MEMREGION_CLAIM_IOCTL, &s);
	if (ret) {
		close(fd);
		return -errno;
	}

	memcpy(meminfo, &s.info, sizeof(*meminfo));

	close(fd);
	return 0;
}

int cuddl_memregion_release(struct cuddl_memregion_info *meminfo)
{
	return cuddl_memregion_release_by_token(meminfo->token);
}

int cuddl_memregion_map(
	struct cuddl_memregion *memregion,
	const struct cuddl_memregion_info *meminfo,
	int options)
{
	int fd;
	void *addr;

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
		close(fd);
		return -errno;
	}

	memregion->priv.fd = fd;
	memregion->priv.pa_len = meminfo->priv.pa_len;
	memregion->priv.pa_addr = (unsigned long) addr;
	memregion->addr = (void *) (
		memregion->priv.pa_addr + meminfo->priv.start_offset);
	memregion->len = meminfo->len;
	memregion->flags = meminfo->flags;
	memregion->token = meminfo->token;
		
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
	if (err == -1)
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
	ret2 = cuddl_memregion_release_by_token(memregion->token);
	if (ret1)
		return ret1;
	return ret2;
}

int cuddl_memregion_release_by_token(cuddl_token_t token)
{
	int fd;
	int ret;
	struct cuddl_memregion_release_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.token = token;

	ret = ioctl(fd, CUDDL_MEMREGION_RELEASE_IOCTL, &s);
	if (ret) {
		close(fd);
		return -errno;
	}

	close(fd);
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
	struct cuddl_eventsrc_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	if (group)
		strncpy(s.id.group, group, CUDDL_MAX_STR_LEN);
	else
		s.id.group[0] = '\0';
	if (device)
		strncpy(s.id.device, device, CUDDL_MAX_STR_LEN);
	else
		s.id.device[0] = '\0';
	if (eventsrc)
		strncpy(s.id.resource, eventsrc, CUDDL_MAX_STR_LEN);
	else
		s.id.resource[0] = '\0';
	s.id.instance = instance;

	ret = ioctl(fd, CUDDL_EVENTSRC_CLAIM_IOCTL, &s);
	if (ret) {
		close(fd);
		return -errno;
	}

	memcpy(eventinfo, &s.info, sizeof(*eventinfo));

	close(fd);
	return 0;
}

int cuddl_eventsrc_release(struct cuddl_eventsrc_info *eventinfo)
{
	return cuddl_eventsrc_release_by_token(eventinfo->token);
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
	eventsrc->token = eventinfo->token;
	eventsrc->priv.fd = fd;

	return 0;
}

int cuddl_eventsrc_close(struct cuddl_eventsrc *eventsrc)
{
	close(eventsrc->priv.fd);
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
	ret2 = cuddl_eventsrc_release_by_token(eventsrc->token);
	if (ret1)
		return ret1;
	return ret2;
}

int cuddl_eventsrc_release_by_token(cuddl_token_t token)
{
	int fd;
	int ret;
	struct cuddl_eventsrc_release_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	s.token = token;

	ret = ioctl(fd, CUDDL_EVENTSRC_RELEASE_IOCTL, &s);
	if (ret) {
		close(fd);
		return -errno;
	}

	close(fd);
	return 0;
}

int cuddl_eventsrc_wait(struct cuddl_eventsrc *eventsrc)
{
	ssize_t n_bytes_read;
	uint32_t count = 0;

	n_bytes_read = read(eventsrc->priv.fd, &count, sizeof(count));
	if (n_bytes_read == -1)
		return -errno;

	return 0;
}

int cuddl_eventsrc_trywait(struct cuddl_eventsrc *eventsrc)
{
	struct cuddl_timespec timeout;

	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;

	return cuddl_eventsrc_timedwait(eventsrc, &timeout);
}

int cuddl_eventsrc_timedwait(
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

	return 0;
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
