/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cross-platform user-space device driver layer Linux kernel declarations.
 *
 * Copyright (C) 2022 Jeff Webb <jeff.webb@codecraftsmen.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _CUDDL_KERNEL_IMPL_LINUX_H
#define _CUDDL_KERNEL_IMPL_LINUX_H
/**
 * DOC: Kernel-space declarations for the Linux implementation.
 *
 * These declarations apply to both Linux UIO and Xenomai UDD. They are only
 * available to kernel-space code.
 */

#ifdef CUDDL_BUILD_WARN_TARGET
  #warning Compiling for Linux kernel
  #if defined(CUDDLK_USE_UDD)
    #warning Compiling for Xenomai UDD (kernel)
  #else /* UIO */
    #warning Compiling for Linux UIO (kernel)
  #endif
#endif /* CUDDL_BUILD_WARN_TARGET */

#include <linux/uio_driver.h>

#define CUDDLK_IMPL_MEMT_NONE    UIO_MEM_NONE
#define CUDDLK_IMPL_MEMT_PHYS    UIO_MEM_PHYS
#define CUDDLK_IMPL_MEMT_LOGICAL UIO_MEM_LOGICAL
#define CUDDLK_IMPL_MEMT_VIRTUAL UIO_MEM_VIRTUAL

#if defined(CUDDLK_USE_UDD)
  #include <rtdm/udd.h>

  #define CUDDLK_IMPL_VARIANT_STR "Xenomai UDD"

  #define CUDDLK_IMPL_RET_INTR_NOT_HANDLED RTDM_IRQ_NONE
  #define CUDDLK_IMPL_RET_INTR_HANDLED     RTDM_IRQ_HANDLED

  #define CUDDLK_IMPL_IRQ_NONE   UDD_IRQ_NONE
  #define CUDDLK_IMPL_IRQ_CUSTOM UDD_IRQ_CUSTOM

#else /* Linux UIO */
  #define CUDDLK_IMPL_VARIANT_STR "Linux UIO"

  #define CUDDLK_IMPL_RET_INTR_NOT_HANDLED IRQ_NONE
  #define CUDDLK_IMPL_RET_INTR_HANDLED     IRQ_HANDLED

  #define CUDDLK_IMPL_IRQ_NONE   UIO_IRQ_NONE
  #define CUDDLK_IMPL_IRQ_CUSTOM UIO_IRQ_CUSTOM
#endif /* defined(CUDDLK_USE_UDD) */

typedef void __iomem cuddlk_impl_iomem_t;

typedef struct device cuddlk_impl_parent_device_t;

/**
 * struct cuddlk_memregion_priv - Private kernel memory region data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_memregion_priv {
};

/**
 * struct cuddlk_interrupt_priv - Private kernel interrupt handler data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_interrupt_priv {
};

/**
 * struct cuddlk_eventsrc_priv - Private kernel event source data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_eventsrc_priv {
	int uio_open_count;
#if defined(CUDDLK_USE_UDD)
	int udd_open_count;
	rtdm_nrtsig_t nrt_sig;
#endif
	struct mutex mut;
};

/**
 * struct cuddlk_device_priv - Private kernel device data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_device_priv {
	struct uio_info uio;
#if defined(CUDDLK_USE_UDD)
	struct udd_device udd;
#endif
};

#endif /* !_CUDDL_KERNEL_IMPL_LINUX_H */
