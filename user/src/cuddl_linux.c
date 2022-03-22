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

#include <cuddl.h>
#include <cuddl/common_impl_linux_ioctl.h>

#ifdef __XENO__
#define CUDDL_MEMREGION_CLAIM_IOCTL CUDDL_MEMREGION_CLAIM_UDD_IOCTL
#else
#define CUDDL_MEMREGION_CLAIM_IOCTL CUDDL_MEMREGION_CLAIM_UIO_IOCTL
#endif

int cuddl_memregion_claim(
	struct cuddl_memregion_info *meminfo,
	const struct cuddl_resource_id *id,
	int options)
{
	int fd;
	int ret;
	struct cuddl_memregion_claim_ioctl_data s;

	fd = open("/dev/cuddl", O_RDWR);
	if (fd == -1)
		return -errno;

	memcpy(&s.id, id, sizeof(s.id));

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
	return 0;
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
		meminfo->pa_len,
		PROT_READ | PROT_WRITE,
		MAP_SHARED,
		fd,
		meminfo->priv.pa_mmap_offset);
	if (addr == (void *) -1) {
		close(fd);
		return -errno;
	}

	memregion->priv.fd = fd;
	memregion->priv.pa_len = meminfo->pa_len;
	memregion->priv.pa_addr = (unsigned long) addr;
	memregion->addr = (void *) (
		memregion->priv.pa_addr + meminfo->start_offset);
	memregion->len = meminfo->len;
	memregion->flags = meminfo->flags;
		
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
