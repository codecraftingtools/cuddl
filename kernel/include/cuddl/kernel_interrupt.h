/* SPDX-License-Identifier: (MIT OR GPL-2.0-or-later) */
/*
 * Cross-platform user-space device driver layer kernel interrupt decls.
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

#ifndef _CUDDL_KERNEL_INTERRUPT_H
#define _CUDDL_KERNEL_INTERRUPT_H

#include <cuddl/kernel_iomem.h>

/**
 * DOC: Kernel-space interrupt handler declarations.
 *
 * This part of the API is only applicable to kernel-space code.
 *
 * Note that the comments in this file regarding Linux UIO and Xenomai UDD
 * implementations are based on *linux-cip-4.19.94-cip18* and *xenomai-3.1*,
 * but other versions are probably very similar.
 */

/**
 * enum cuddlk_intr_handler_return_value - Interrupt handler return values.
 *
 * @CUDDLK_RET_INTR_NOT_HANDLED: Indicates that the interrupt was not handled
 *     (usually this means that the interrupt was generated by another
 *     device)
 *
 * @CUDDLK_RET_INTR_HANDLED: Indicates that the interrupt was handled.
 *
 * Possible return values for interrupt handlers.
 *
 * Similar to::
 *
 *   IRQ_*      (type irqreturn_t) in linux/irqreturn.h    for Linux   UIO
 *   RTDM_IRQ_* (#define)          in rtdm/driver.h        for Xenomai UDD
 *   XN_IRQ_*   (#define)          in cobalt/kernel/intr.h for Xenomai UDD
 *
 * Note that the equivalent Xenomai UDD and Linux UIO constants have
 * different values, but this should not matter for our purposes because a
 * single interrupt system will be selected at compile time.
 */
enum cuddlk_interrupt_handler_return_value {
	CUDDLK_RET_INTR_NOT_HANDLED = CUDDLK_IMPL_RET_INTR_NOT_HANDLED,
	CUDDLK_RET_INTR_HANDLED = CUDDLK_IMPL_RET_INTR_HANDLED,
};

/**
 * enum cuddlk_special_irq_value - Special-purpose irq values.
 *
 * @CUDDLK_IRQ_NONE: No hardware interrupts or user-space interrupt events are
 *                   associated with this device 
 *
 * @CUDDLK_IRQ_CUSTOM: No hardware interrupts are associated with this
 *                     device, but user-space interrupt events may be
 *                     generated by calling ``cuddlk_eventsrc_notify()``.
 *                     This value is only applicable to ``cuddlk_interrupt``
 *                     instances associated with an event source, not
 *                     stand-alone interrupt handlers.
 *
 * Special-purpose values for the ``irq`` member of the ``cuddlk_interrupt``
 * struct.
 *
 * Similar to::
 *
 *   UIO_IRQ_* (#define) in linux/uio_driver.h for Linux   UIO
 *   UDD_IRQ_* (#define) in rtdm/udd.h         for Xenomai UDD
 *
 * Note that the equivalent Xenomai UDD and Linux UIO constants have similar
 * values, but this is not strictly necessary for our purposes because a
 * single interrupt system will be selected at compile time.
 *
 * Our implementation does rely on the condition that ``CUDDLK_IRQ_NONE`` is
 * ``0`` for proper default initialization, so we confirm this via a
 * compile-time assertion in *cuddlk_linux.c*.
 */
enum cuddlk_special_irq_value {
	CUDDLK_IRQ_NONE = CUDDLK_IMPL_IRQ_NONE,
	CUDDLK_IRQ_CUSTOM = CUDDLK_IMPL_IRQ_CUSTOM,
};

/**
 * enum cuddlk_interrupt_flags - Interrupt flags for kernel space.
 *
 * @CUDDLK_IRQF_SHARED: Tell the kernel to enable interrupt sharing with
 *                      other drivers.
 *
 * Flags that describe the properties of an interrupt.  These may be used in
 * the ``flags`` member of the ``cuddlk_interrupt`` struct.
 *
 * Similar to::
 *
 *   IRQF_SHARED         (#define) in linux/interrupt.h for Linux   UIO
 *   RTDM_IRQTYPE_SHARED (#define) in rtdm/driver.h     for Xenomai UDD
 *   XN_IRQTYPE_SHARED   (#define) in kernel/intr.h     for Xenomai UDD
 *
 * Note that the equivalent Xenomai UDD and Linux UIO constants have
 * different values, but that is taken into account in the implementation.
 */
enum cuddlk_interrupt_flags {
	CUDDLK_IRQF_SHARED = (1 << 0),
};

/**
 * struct cuddlk_interrupt_kernel - Kernel-managed interrupt data members.
 *
 * Kernel-managed ``cuddlk_interrupt`` data that is available for use by
 * Cuddl drivers.
 */
struct cuddlk_interrupt_kernel {
};

/**
 * struct cuddlk_interrupt - Interrupt handler/masking data structure.
 *
 * @handler: Pointer to the interrupt handler routine to be called when a
 *           hardware interrupt occurs.  This routine should check for and
 *           mask any pending interrupts and then return the applicable value
 *           from the ``cuddlk_intr_handler_return_value`` enumeration.  If
 *           this field is set to ``NULL``, then no interrupt handler will be
 *           installed.  Typically a value of ``NULL`` only makes sense when
 *           setting ``irq`` to ``CUDDLK_IRQ_NONE`` or ``CUDDLK_IRQ_CUSTOM``.
 *
 *           Linux UIO equivalent in *linux/uio_driver.h*::
 *
 *             irqreturn_t (*handler)(int irq, struct uio_info *dev_info);
 *
 *           Xenomai UDD equivalent in *rtdm/udd.h*::
 *
 *             int (*interrupt)(struct udd_device *udd);
 *
 * @disable: Pointer to a routine that will disable the interrupt source. A
 *           return value of ``0`` indicates success and a negative value
 *           indicates failure.  This field may be set to ``NULL`` if
 *           external interrupt disabling control is not required.
 *
 * @enable: Pointer to a routine that will enable the interrupt source. A
 *          return value of ``0`` indicates success and a negative value
 *          indicates failure.  This field may be set to ``NULL`` if external
 *          interrupt enabling control is not required.
 *
 * @iomem_ptr: This field is available for use by Cuddl drivers to store a
 *             pointer to mapped I/O memory.  This field is not managed (or
 *             used) by the Cuddl implementation, so this field must set up
 *             (if desired) manually during driver initialization.
 *
 * @extra_ptr: This field is available for use by Cuddl drivers to store a
 *             pointer to a custom data structure for use by the driver. This
 *             field is not manged or used by the Cuddl implementation.
 *
 * @irq: IRQ number for which the associated handler will be installed, or
 *       one of the ``cuddlk_special_irq_value`` enumerations.
 *
 *       Under Linux UIO, the device IRQ number can be read from a file such
 *       as::
 *
 *         /sys/class/uio/uio0/device/irq
 *        
 *       The associated UIO/UDD field declarations look like this::
 *
 *         UIO: long irq;
 *         UDD: int irq;
 *
 * @flags: Flags that describe the properties of the interrupt.  This field
 *         may be a set of ``cuddlk_interrupt_flags`` ORed together.
 *
 * @kernel: Kernel-managed memory region data that is available for use by
 *          Cuddl drivers.
 *
 * @priv: Private memory region data reserved for internal use by the Cuddl
 *        implementation.
 *
 * Data structure for setting up an interrupt handler in kernel space.
 *
 * Unused members of this data structure must be set to zero.  This is
 * typically done by allocating this structure via ``kzalloc()`` or using
 * ``memset()`` to zeroize the structure after allocation.
 */
struct cuddlk_interrupt {
	int (*handler)(struct cuddlk_interrupt *intr);
	int (*disable)   (struct cuddlk_interrupt *intr);
	int (*enable) (struct cuddlk_interrupt *intr);
	cuddlk_iomem_t *iomem_ptr;
	void *extra_ptr;
	int irq;
	int flags;
	struct cuddlk_interrupt_kernel kernel;
	struct cuddlk_interrupt_priv priv;
};

/**
 * cuddlk_interrupt_register() - Register a Cuddl interrupt handler.
 *
 * @intr: Cuddl interrupt structure describing the interrupt to register.
 *
 * Linux UIO equivalent in *linux/interrupt.h*::
 *
 *   int request_irq(unsigned int irq, irq_handler_t handler, unsigned long
 *                   flags, const char *name, void *dev);
 *
 * Xenomai UDD equivalent in *rtdm/driver.h*::
 *
 *   int rtdm_irq_request(rtdm_irq_t *irq_handle, unsigned int irq_no,
 *                        rtdm_irq_handler_t handler, unsigned long flags,
 *                        const char *device_name, void *arg);
 *
 * Hardware interrupts may be handled by populating the ``events`` member of
 * a ``cuddlk_device``, or by installing a separate handler that is not
 * associated with one particular device.  This function accomplishes the
 * latter.
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_interrupt_register(
	struct cuddlk_interrupt *intr, const char *name);

/**
 * cuddlk_interrupt_unregister() - Unregister a Cuddl interrupt handler.
 *
 * @intr: Cuddl interrupt structure describing the interrupt to unregister.
 *
 * Linux UIO equivalent in *linux/interrupt.h*::
 *
 *   void *free_irq(unsigned int, void *);
 *
 * Xenomai UDD equivalent in *rtdm/driver.h*::
 *
 *   int rtdm_irq_free(rtdm_irq_t *irq_handle);
 *
 * Return: ``0`` on success, or a negative error code.
 */
int cuddlk_interrupt_unregister(struct cuddlk_interrupt *intr);

#endif /* !_CUDDL_KERNEL_INTERRUPT_H */
