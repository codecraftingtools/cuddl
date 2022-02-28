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
static int cuddlk_udd_interrupt_handler(struct udd_device *udd_dev)
{
	struct cuddlk_device *dev;
	struct cuddlk_eventsrc *eventsrc;
	struct cuddlk_interrupt *intr;
	int ret;
	
	dev = container_of(udd_dev, struct cuddlk_device, priv.udd);
	eventsrc = &dev->events[0];
	intr = &eventsrc->intr;
	ret = intr->handler(intr);

	if (eventsrc->priv.uio_open_count > 0) {
		rtdm_nrtsig_pend(&eventsrc->priv.nrt_sig);
	}
	
	/* udd_notify_event() is triggered by caller */
	return ret;
}

#else /* UIO */
static irqreturn_t cuddlk_uio_interrupt_handler(
	int irq, struct uio_info *uinfo)
{
	struct cuddlk_device *dev;
	struct cuddlk_interrupt *intr;
	
	dev = container_of(uinfo, struct cuddlk_device, priv.uio);
	intr = &dev->events[0].intr;
	
	/* uio_event_notify() is triggered by caller */
	return intr->handler(intr);
}
#endif

#if defined(CUDDLK_USE_UDD)
void event_nrtsig_handler(rtdm_nrtsig_t *nrt_sig, void *data)
{
	struct uio_info *uinfo = (struct uio_info *)data;
	uio_event_notify(uinfo);
}
#endif

#if defined(CUDDLK_USE_UDD)
static int cuddlk_udd_eventsrc_open(struct rtdm_fd *fd, int oflags)
{
	struct udd_device *udd_dev;
	struct cuddlk_device *dev;
	struct cuddlk_eventsrc *eventsrc;
	
	udd_dev = container_of(
		rtdm_fd_device(fd), struct udd_device, __reserved.device);
	dev = container_of(udd_dev, struct cuddlk_device, priv.udd);
	eventsrc = &dev->events[0];
	mutex_lock(&eventsrc->priv.mut);
	eventsrc->priv.udd_open_count += 1;
	mutex_unlock(&eventsrc->priv.mut);
	return 0;
}
#endif

static int cuddlk_uio_eventsrc_or_mem_open(
	struct uio_info *uinfo, struct inode *inode)
{
	struct cuddlk_device *dev;
	struct cuddlk_eventsrc *eventsrc;

	dev = container_of(uinfo, struct cuddlk_device, priv.uio);
	eventsrc = &dev->events[0];
	mutex_lock(&eventsrc->priv.mut);
	eventsrc->priv.uio_open_count += 1;
	mutex_unlock(&eventsrc->priv.mut);
	return 0;
}

#if defined(CUDDLK_USE_UDD)
static void cuddlk_udd_eventsrc_close(struct rtdm_fd *fd)
{
	struct udd_device *udd_dev;
	struct cuddlk_device *dev;
	struct cuddlk_eventsrc *eventsrc;
	
	udd_dev = container_of(
		rtdm_fd_device(fd), struct udd_device, __reserved.device);
	dev = container_of(udd_dev, struct cuddlk_device, priv.udd);
	eventsrc = &dev->events[0];
	mutex_lock(&eventsrc->priv.mut);
	eventsrc->priv.udd_open_count -= 1;
	mutex_unlock(&eventsrc->priv.mut);
}
#endif

static int cuddlk_uio_eventsrc_or_mem_close(
	struct uio_info *uinfo, struct inode *inode)
{
	struct cuddlk_device *dev;
	struct cuddlk_eventsrc *eventsrc;

	dev = container_of(uinfo, struct cuddlk_device, priv.uio);
	eventsrc = &dev->events[0];
	mutex_lock(&eventsrc->priv.mut);
	eventsrc->priv.uio_open_count -= 1;
	mutex_unlock(&eventsrc->priv.mut);
	return 0;
}

int cuddlk_register_device(struct cuddlk_device *dev)
{
	int i;
	int ret = 0;
	struct uio_info *uio;
	struct cuddlk_memregion *mem_i;
	struct cuddlk_eventsrc *eventsrc;
	struct cuddlk_interrupt *intr;

#if defined(CUDDLK_USE_UDD)
	struct udd_device *udd;
#endif

	eventsrc = &dev->events[0];
	intr = &eventsrc->intr;

	if (dev->group)
		strncpy(dev->kernel.group, dev->group, CUDDLK_MAX_STR_LEN);
	dev->group = dev->kernel.group;
	if (dev->name)
		strncpy(dev->kernel.name, dev->name, CUDDLK_MAX_STR_LEN);
	dev->name = dev->kernel.name;
	snprintf(dev->priv.unique_name, CUDDLK_MAX_STR_LEN, "%s.%s.%d",
	         dev->group, dev->name, dev->instance);

	mutex_init(&eventsrc->priv.mut);

	uio = &dev->priv.uio;
	uio->name = dev->priv.unique_name;
	uio->version = "0.0.1";

#if defined(CUDDLK_USE_UDD)
	udd = &dev->priv.udd;
	udd->device_name = dev->priv.unique_name;
	udd->device_flags = RTDM_NAMED_DEVICE;
#endif

	for (i=0; i<CUDDLK_MAX_DEV_MEM_REGIONS; i++) {
		mem_i = &dev->mem[i];
		if (mem_i->name)
			strncpy(mem_i->kernel.name, mem_i->name,
				CUDDLK_MAX_STR_LEN);
		mem_i->name = mem_i->kernel.name;

		if (i<MAX_UIO_MAPS) {
			uio->mem[i].name    = mem_i->name;
			uio->mem[i].addr    = mem_i->pa_addr;
			uio->mem[i].offs    = mem_i->start_offset;
			uio->mem[i].size    = mem_i->pa_len;
			uio->mem[i].memtype = mem_i->type;
		}

#if defined(CUDDLK_USE_UDD)
		if (i<UDD_NR_MAPS) {
			udd->mem_regions[i].name = mem_i->name;
			udd->mem_regions[i].addr = mem_i->pa_addr;
			udd->mem_regions[i].len  = mem_i->pa_len;
			udd->mem_regions[i].type = mem_i->type;
		}
#endif
	}

	if (eventsrc->name)
		strncpy(eventsrc->kernel.name, eventsrc->name,
			CUDDLK_MAX_STR_LEN);
	eventsrc->name = eventsrc->kernel.name;

	if ((intr->irq > 0) && (intr->handler)) {
#if defined(CUDDLK_USE_UDD)
		udd->irq = intr->irq;
		udd->ops.interrupt = cuddlk_udd_interrupt_handler;
		uio->irq = UIO_IRQ_CUSTOM;

#else /* UIO */
		uio->irq = intr->irq;
		if (intr->flags & CUDDLK_IRQF_SHARED) {
			uio->irq_flags |= IRQF_SHARED;
		}
		uio->handler = cuddlk_uio_interrupt_handler;
#endif
	}

	if (intr->irq == CUDDLK_IRQ_CUSTOM) {
		uio->irq = UIO_IRQ_CUSTOM;
#if defined(CUDDLK_USE_UDD)
		udd->irq = UDD_IRQ_CUSTOM;
#endif
	}
	
	if ((intr->irq > 0) || (intr->irq == CUDDLK_IRQ_CUSTOM)) {
		uio->open = cuddlk_uio_eventsrc_or_mem_open;
		uio->release = cuddlk_uio_eventsrc_or_mem_close;
#if defined(CUDDLK_USE_UDD)
		udd->ops.open = cuddlk_udd_eventsrc_open;
		udd->ops.close = cuddlk_udd_eventsrc_close;
#endif
	}
	
	ret = uio_register_device(dev->parent_dev_ptr, uio);
	if (ret)
		goto fail_uio_register;

#if defined(CUDDLK_USE_UDD)
	rtdm_nrtsig_init(&eventsrc->priv.nrt_sig,
			 event_nrtsig_handler, uio);
	
	ret = udd_register_device(udd);
	if (ret)
		goto fail_udd_register;
#endif

	return 0;

#if defined(CUDDLK_USE_UDD)
fail_udd_register:
#endif

	uio_unregister_device(uio);

fail_uio_register:
	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_register_device);

int cuddlk_unregister_device(struct cuddlk_device *dev)
{
	int ret = 0;
	
#if defined(CUDDLK_USE_UDD)
	ret = udd_unregister_device(&dev->priv.udd);
	rtdm_nrtsig_destroy(&dev->events[0].priv.nrt_sig);
#endif

	uio_unregister_device(&dev->priv.uio);

	return ret;
}
EXPORT_SYMBOL_GPL(cuddlk_unregister_device);

static int __init cuddlk_init(void)
{
	/* This must be called somewhere for assertions to trigger. */
	check_assertions();
	return 0;
}

static void __exit cuddlk_exit(void)
{
}

module_init(cuddlk_init)
module_exit(cuddlk_exit)
MODULE_LICENSE("GPL");
