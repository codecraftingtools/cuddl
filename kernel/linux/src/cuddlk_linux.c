/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cross-platform user-space device driver layer Linux kernel implementation.
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

/*
 * Linux kernel module implementation.
 *
 * This code implements both Linux UIO and Xenomai UDD functionality (based
 * on the ``CUDDLK_USE_UDD`` *#define*).
 */

#include <linux/module.h>
#include <cuddl/kernel.h>

/* Note that static_assert() works on newer kernels, but
 * BUILD_BUG_ON() is required for older kernels.  Note that
 * static_assert() can be used at file scope, whereas BUILD_BUG_ON()
 * cannot. */
static void check_assertions(void){
	BUILD_BUG_ON(CUDDLK_MEMT_NONE != 0);
	BUILD_BUG_ON(CUDDLK_IRQ_NONE != 0);
#if defined(CUDDLK_USE_UDD)
	BUILD_BUG_ON(CUDDLK_MEMT_NONE    != UDD_MEM_NONE);
	BUILD_BUG_ON(CUDDLK_MEMT_PHYS    != UDD_MEM_PHYS);
	BUILD_BUG_ON(CUDDLK_MEMT_LOGICAL != UDD_MEM_LOGICAL);
	BUILD_BUG_ON(CUDDLK_MEMT_VIRTUAL != UDD_MEM_VIRTUAL);
#endif
}

#if defined(CUDDLK_USE_UDD)
static int cuddl_udd_interrupt_handler(struct udd_device *udd_dev)
{
	struct cuddlk_device *dev;
	struct cuddlk_interrupt *intr;
	
	dev = container_of(udd_dev, struct cuddlk_device, priv.udd);
	intr = &dev->events[0].intr;
	return intr->handler(intr);
}

#else /* UIO */
static irqreturn_t cuddl_uio_interrupt_handler(
	int irq, struct uio_info *uinfo)
{
	struct cuddlk_device *dev;
	struct cuddlk_interrupt *intr;
	
	dev = container_of(uinfo, struct cuddlk_device, priv.uio);
	intr = &dev->events[0].intr;
	return intr->handler(intr);
}
#endif

int cuddlk_register_device(struct cuddlk_device *dev)
{
	int i;
	int ret = 0;
	struct uio_info *uio;

#if defined(CUDDLK_USE_UDD)
	struct udd_device *udd;
#endif

	uio = &dev->priv.uio;
	uio->name = dev->name;
	uio->version = "0.0.1";

#if defined(CUDDLK_USE_UDD)
	udd = &dev->priv.udd;
	udd->device_name = dev->name;
	udd->device_flags = RTDM_NAMED_DEVICE;
#endif

	for (i=0; i<CUDDLK_MAX_DEV_MEM_REGIONS; i++) {
		if (i<MAX_UIO_MAPS) {
			uio->mem[i].name    = dev->mem[i].name;
			uio->mem[i].addr    = dev->mem[i].pa_addr;
			uio->mem[i].offs    = dev->mem[i].start_offset;
			uio->mem[i].size    = dev->mem[i].pa_len;
			uio->mem[i].memtype = dev->mem[i].type;
		}

#if defined(CUDDLK_USE_UDD)
		if (i<UDD_NR_MAPS) {
			udd->mem_regions[i].name = dev->mem[i].name;
			udd->mem_regions[i].addr = dev->mem[i].pa_addr;
			udd->mem_regions[i].len  = dev->mem[i].pa_len;
			udd->mem_regions[i].type = dev->mem[i].type;
		}
#endif
	}

	if ((dev->events[0].intr.irq > 0) && (dev->events[0].intr.handler)) {
#if defined(CUDDLK_USE_UDD)
		udd->irq = dev->events[0].intr.irq;
		udd->ops.interrupt = cuddl_udd_interrupt_handler;
		uio->irq = UIO_IRQ_CUSTOM;

#else /* UIO */
		uio->irq = dev->events[0].intr.irq;
		if (dev->events[0].intr.flags & CUDDLK_IRQF_SHARED) {
			uio->irq_flags |= IRQF_SHARED;
		}
		uio->handler = cuddl_uio_interrupt_handler;
#endif
	}

	if (dev->events[0].intr.irq == CUDDLK_IRQ_CUSTOM) {
		uio->irq = UIO_IRQ_CUSTOM;
#if defined(CUDDLK_USE_UDD)
		udd->irq = UDD_IRQ_CUSTOM;
#endif
	}
	
	ret = uio_register_device(dev->parent_dev_ptr, uio);
	if (ret)
		goto fail_uio_register;

#if defined(CUDDLK_USE_UDD)
	ret = udd_register_device(udd);
	if (ret)
		goto fail_udd_register;
#endif

	return 0;

fail_udd_register:
	uio_unregister_device(uio);

fail_uio_register:
	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_register_device);

int cuddlk_unregister_device(struct cuddlk_device *dev)
{
	int ret;

	uio_unregister_device(&dev->priv.uio);
	ret = 0;
	
#if defined(CUDDLK_USE_UDD)
	ret = udd_unregister_device(&dev->priv.udd);
#endif

	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_unregister_device);

static int __init cuddl_init(void)
{
	/* This must be called somewhere for assertions to trigger. */
	check_assertions();
	return 0;
}

static void __exit cuddl_exit(void)
{
}

module_init(cuddl_init)
module_exit(cuddl_exit)
MODULE_LICENSE("GPL");
