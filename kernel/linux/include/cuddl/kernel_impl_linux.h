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

#define CUDDLK_PAGE_SIZE PAGE_SIZE

#define cuddlk_ioread8  ioread8
#define cuddlk_ioread16 ioread16
#define cuddlk_ioread32 ioread32

#define cuddlk_iowrite8  iowrite8
#define cuddlk_iowrite16 iowrite16
#define cuddlk_iowrite32 iowrite32

#define CUDDLK_IMPL_MEMT_NONE    UIO_MEM_NONE
#define CUDDLK_IMPL_MEMT_PHYS    UIO_MEM_PHYS
#define CUDDLK_IMPL_MEMT_LOGICAL UIO_MEM_LOGICAL
#define CUDDLK_IMPL_MEMT_VIRTUAL UIO_MEM_VIRTUAL

#if defined(CUDDLK_USE_UDD)
  #include <rtdm/udd.h>

  #define CUDDLK_IMPL_VARIANT_STR "Xenomai UDD"

  #define cuddlk_impl_print(...) rtdm_printk(__VA_ARGS__)

  #define CUDDLK_IMPL_RET_INTR_NOT_HANDLED RTDM_IRQ_NONE
  #define CUDDLK_IMPL_RET_INTR_HANDLED     RTDM_IRQ_HANDLED

  #define CUDDLK_IMPL_IRQ_NONE   UDD_IRQ_NONE
  #define CUDDLK_IMPL_IRQ_CUSTOM UDD_IRQ_CUSTOM

#else /* Linux UIO */
  #define CUDDLK_IMPL_VARIANT_STR "Linux UIO"

  #define cuddlk_impl_print(...) printk(__VA_ARGS__)

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
 * @irqh: Xenomai RTDM interrupt data structure.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_interrupt_priv {
#if defined(CUDDLK_USE_UDD)
	rtdm_irq_t irqh;
#endif
};

/**
 * struct cuddlk_eventsrc_priv - Private kernel event source data.
 *
 * @uio_open_count: Count of open Linux UIO file descriptors.
 * @uio_ptr: Pointer to the associated Linux UIO device.
 * @udd_open_count: Count of open Xenomai UDD file descriptors.
 * @udd_ptr: Pointer to the associated Xenoami UDD device.
 * @nrt_sig: Xenomai real-time/non-real-time signaling mechanism.
 * @mut: Mutex protecting the open counts.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_eventsrc_priv {
	int uio_open_count;
	struct uio_info *uio_ptr;
#if defined(CUDDLK_USE_UDD)
	int udd_open_count;
	struct udd_device *udd_ptr;
	rtdm_nrtsig_t nrt_sig;
#endif
	struct mutex mut;
};

/**
 * struct cuddlk_device_priv - Private kernel device data.
 *
 * @unique_name: Unique base name for use when creating UDD/UIO device nodes.
 * @uio: The associate Linux UIO device.
 * @udd: The associate Xenomai UDD device.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_device_priv {
	char *unique_name;
	struct uio_info uio;
#if defined(CUDDLK_USE_UDD)
	struct udd_device udd;
#endif
};

/**
 * struct cuddlk_manager_priv - Private kernel device data.
 *
 * This data structure contains private, platform-specific data members
 * reserved for internal use by the Cuddl implementation.
 */
struct cuddlk_manager_priv {
};

#endif /* !_CUDDL_KERNEL_IMPL_LINUX_H */
