/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel mem region decls.
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

#ifndef _CUDDLK_MEMREGION_H
#define _CUDDLK_MEMREGION_H

#include <cuddlk/general.h>

/**
 * DOC: Kernel-space memory region declarations.
 *
 * Memory regions are commonly used to expose device registers to user-space
 * applications for reading and writing.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * Note that the comments in this file regarding Linux UIO and Xenomai UDD
 * implementations are based on *linux-cip-4.19.94-cip18* and *xenomai-3.1*,
 * but other versions are probably very similar.
 */

/**
 * enum cuddlk_memregion_type - Memory region types for kernel space.
 *
 * @CUDDLK_MEMT_NONE: No memory region is associated with this
 *                    ``cuddlk_memregion`` instance (i.e. this array slot is
 *                    not used).
 *
 * @CUDDLK_MEMT_PHYS: Physical device I/O memory region.
 *
 * @CUDDLK_MEMT_LOGICAL: Logical memory region (e.g. from ``kmalloc()``).
 *
 * @CUDDLK_MEMT_VIRTUAL: Virtual memory region (e.g. from ``vmalloc()``).
 *
 * This type enumerates the types of memory regions that may be exposed to
 * user-space code.  These are the possible values for the ``type`` member of
 * the ``cuddlk_memregion`` struct.
 *
 * Similar to::
 *
 *   UIO_MEM_* (#define) in linux/uio_driver.h for Linux   UIO
 *   UDD_MEM_* (#define) in rtdm/udd.h         for Xenomai UDD
 *
 * Note that the equivalent Xenomai UDD and Linux UIO constants have similar
 * values.  Our implementation currently relies on this condition, so we
 * confirm this via compile-time assertions in *cuddlk_linux.c*.  We also
 * assume ``CUDDLK_MEMT_NONE`` is ``0`` for proper default initialization, so
 * that condition is checked as well.
 */
enum cuddlk_memregion_type {
	CUDDLK_MEMT_NONE    = CUDDLK_IMPL_MEMT_NONE,
	CUDDLK_MEMT_PHYS    = CUDDLK_IMPL_MEMT_PHYS,
	CUDDLK_MEMT_LOGICAL = CUDDLK_IMPL_MEMT_LOGICAL,
	CUDDLK_MEMT_VIRTUAL = CUDDLK_IMPL_MEMT_VIRTUAL,
};

/** 
 * enum cuddlk_memregion_flags - Memory region flags for kernel space.
 *
 * @CUDDLK_MEMF_SHARED: Indicates that the associated memory region may be
 *                      claimed by more than one user-space application
 *                      simultaneously.
 *
 * Flags that describe the properties of a memory region.  These may be used
 * in the ``flags`` member of the ``cuddlk_memregion`` struct.
 */
enum cuddlk_memregion_flags {
	CUDDLK_MEMF_SHARED = (1 << 0),
};

/**
 * struct cuddlk_memregion_kernel - Kernel-managed memory region data members.
 *
 * @ref_count: Number of user-space applications that have claimed this
 *             memory region.  This should be either ``0`` or ``1`` unless
 *             the memory region has the ``CUDDLK_MEMF_SHARED`` flag set.
 *
 * Kernel-managed ``cuddlk_memregion`` data members that are available for
 * use by Cuddl drivers.
 */
struct cuddlk_memregion_kernel {
	int ref_count;
};

/**
 * struct cuddlk_memregion - Memory region information (kernel-space).
 *
 * @name: Name used to identify the memory region.
 *
 *        Under Linux UIO, the value of this field can be read from a file::
 *         
 *          e.g. /sys/class/uio/uio0/maps/map0/name
 *
 * @pa_addr: Page-aligned address.
 *
 *           This is the starting address for the memory region to be mapped
 *           and must be must be ``CUDDLK_PAGE_SIZE``-aligned.
 *            
 *           Under Linux UIO, the field value can be read from a file::
 *            
 *             e.g. /sys/class/uio/uio0/maps/map0/addr
 *            
 *           The associated UIO/UDD field declarations look like this::
 *            
 *             UIO: phys_addr_t addr;
 *             UDD: unsigned long addr;
 *
 * @pa_len: Page-aligned length.
 *
 *          The size of the memory region to be mapped, in bytes.  This value
 *          must be a multiple of ``CUDDLK_PAGE_SIZE``.  If this field is not
 *          specified (i.e. ``0``), it will be set to the value of ``(len +
 *          start_offset)`` rounded up to the next ``CUDDLK_PAGE_SIZE`` when
 *          the parent device is registered.
 *           
 *          Under Linux UIO, the field value can be read from a file::
 *           
 *            e.g. /sys/class/uio/uio0/maps/map0/size
 *           
 *          The associated UIO/UDD field declarations look like this::
 *           
 *            UIO: resource_size_t size;
 *            UDD: size_t len;
 *
 * @start_offset:
 *
 *     Starting offset of the memory region to be mapped (relative to
 *     ``pa_addr``), in bytes.  This value does NOT need to be
 *     ``CUDDLK_PAGE_SIZE``-aligned, so it is used when defining
 *     non-``CUDDLK_PAGE_SIZE``-aligned memory regions.
 *      
 *     Under Linux UIO, the field value can be read from a file::
 *      
 *       e.g. /sys/class/uio/uio0/maps/map0/offset
 *      
 *     The associated UIO/UDD field declarations look like this::
 *      
 *       UIO: unsigned long offs;
 *       UDD: N/A
 *
 * @len: The exact size of the memory region to be mapped, in bytes.  This
 *       value does NOT need to be a multiple of ``CUDDLK_PAGE_SIZE``.  If
 *       this field is not specified (i.e. ``0``), it will be set to the
 *       value of ``pa_len`` when the parent device is registered
 *
 * @type: The type of memory region to be mapped.  This should be one of the
 *        ``cuddlk_memregion_type`` enumeration values described elsewhere.
 *
 * @flags: Flags that describe the properties of the memory region.  This
 *         field may be a set of ``cuddlk_memregion_flags`` ORed together.
 *
 * @kernel: Kernel-managed memory region data that is available for use by
 *          Cuddl drivers.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Represents a memory region that may be mapped by user-space applications.
 *         
 * Under Linux UIO and Xenomai UDD, memory regions are mapped via a
 * ``mmap()`` system call on a device file descriptor.
 *
 * Under RTEMS, the kernel and applications share a common address
 * space, so ``mmap()`` is not required.
 *
 * Similar to::
 *
 *   struct uio_mem       in linux/uio_driver.h for Linux   UIO
 *   struct udd_memregion in rtdm/udd.h         for Xenomai UDD
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_memregion {
	char *name;
	unsigned long pa_addr;
	cuddlk_size_t pa_len;
	cuddlk_size_t start_offset;
	cuddlk_size_t len;
	int type;
	int flags;
	struct cuddlk_memregion_kernel kernel;
	struct cuddlk_memregion_priv priv;
};

#endif /* !_CUDDLK_MEMREGION_H */
