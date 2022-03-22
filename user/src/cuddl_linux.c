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
