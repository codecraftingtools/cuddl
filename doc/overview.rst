.. SPDX-License-Identifier: (MIT OR GPL-2.0-or-later)
..
   Copyright (C) 2022 Jeff Webb <jeff.webb@codecraftsmen.org>
   
   This software and the associated documentation files are dual-licensed and
   are made available under the terms of the MIT License or under the terms
   of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.  You may select (at your option) either of the licenses listed
   above.  See the LICENSE.MIT and LICENSE.GPL-2.0 files in the top-level
   directory of this distribution for copyright information and license
   terms.
   
========
Overview
========

The Cuddl project provides a cross-platform API for writing user-space device
drivers that works under `Linux`_, `Xenomai`_, and `RTEMS`_.

Status
======

`Xenomai`_ and standard `Linux`_ systems are fully supported.  Support for
`RTEMS`_ has not been implemented at this time.

.. _Linux: https://www.kernel.org
.. _Xenomai: https://www.xenomai.org
.. _RTEMS: https://www.rtems.org

Background
==========

..  sphinx-include-background-start

Before we dive into the details of the software, let us take a moment to
discuss the architecture of device drivers.

Conventional UNIX Device Driver Architecture
--------------------------------------------

The UNIX philosophy could perhaps be summarized as *everything is a file*.
Given the UNIX mindset, it should come as no surprise that under UNIX,
user-space applications access hardware devices through special device nodes
that appear in the filesystem tree.  Applications can transfer data to and
from hardware devices using these device nodes in the same way that they read
and write standard data files.  The user interface for other operations (such
as device configuration or special functions) is not as standardized, and
usually involves device-specific `ioctl()`_ calls.

UNIX device drivers are typically implemented entirely in kernel space, and
as such, are only accessible to user-space applications via system calls
provided by the operating system.  This level of abstraction provides a great
deal of portability between POSIX (the UNIX standard) operating systems, and
also between hardware devices, provided that the hardware devices have
similar functionality.  It is worth noting, however, that although POSIX
*applications* can be fairly portable, kernel-space *device drivers* are
usually not.

UNIX device drivers typically contain code to handle device interrupts, code
to configure and control the device using memory-mapped I/O registers, and
code to create and interact with device nodes that expose the hardware to
user-space applications.  The interrupt handling code is typically broken up
into a top half and a bottom half.  The top half executes quickly, handling
the urgent tasks with interrupts disabled, and then it schedules the bottom
half to be executed a bit later, with interrupts enabled.

.. raw:: html

   <!-- special case code for clickable svg files -->
   <object data="images/unix_device_driver_arch.svg"
           width=60% type="image/svg+xml">
   </object>

.. only:: latex

   .. image:: images/unix_device_driver_arch.*
      :align: center
      :scale: 60%

Advantages of the conventional UNIX device driver architecture:

- Devices are exposed via a familiar file-like interface.
- Application code is often portable to other POSIX-compliant operating
  systems.
- Application code is often portable between devices with similar
  functionality.
- No extra user-space libraries are required.

Disadvantages of the conventional UNIX device driver architecture:

- Clunky interface for non-standard devices or functions.
- Application code is not portable to non-POSIX systems.
- Driver code is usually not portable.
- Driver development must be done in kernel space, which requires admin
  privileges and makes debugging more difficult.
- Loading new drivers requires special procedures and admin privileges.
- In some cases (especially for non-standard functionality), kernel-driver
  and user-space code may need to be kept in-sync.


User-Space Device Driver Architecture
-------------------------------------

The user-space device driver concept attempts to address some of the
disadvantages of kernel-space device drivers by moving as much code into user
space as possible.  Depending on the particular operating system, some
aspects of device-driver functionality (such as interrupt handling) may only
be available in kernel space, so in those cases a small kernel device driver
stub must be implemented in addition to the user-space device driver code.
Typically, the top half of the interrupt handler must be implemented in
kernel space.  Sometimes it is also convenient to detect, map, and export the
a device's registers from kernel space.

Because drivers are implemented in a user-space library, and not as part of
the kernel, user-space device driver architectures typically implement a
functional or object-oriented end-user API instead of using the POSIX file
convention.  Depending on your preferences, and on the type of device you are
dealing with, this may be an advantage or a disadvantage.

.. raw:: html

   <!-- special case code for clickable svg files -->
   <object data="images/userspace_device_driver_arch.svg"
           width=60% type="image/svg+xml">
   </object>

.. only:: latex

   .. image:: images/userspace_device_driver_arch.*
      :align: center
      :scale: 60%

Advantages of the user-space device driver architecture:

- Custom object-oriented or functional end-user APIs can be implemented.
- Device drivers can be implemented in any programming language.
- Application code can be made portable across many (even non-POSIX)
  operating systems via a compatibility layer.
- Devices with non-standard functionality can be more easily accommodated.
- Most of the driver code should be portable across many operating systems.
- The majority of driver development is done in user space.
- The kernel device driver stub code is minimal, and its interface is simple,
  which reduces the need to change the kernel-space code.  This simplicity
  makes it easier to keep the user-space and kernel-space code in sync.

Disadvantages of the user-space device driver architecture:

- Non-standard approach, so it is less widely known and understood.
- Not quite as fast as native drivers.
- A POSIX file-like interface is not practical with this approach, so
  user-space devices cannot work in the same way as OS-provided devices on
  POSIX systems.
- Some sort of wrapper would be required to write applications that could use
  POSIX device nodes and user-space devices interchangeably.
- Applications must link against a user-space device driver library.

If these concepts are new to you, it may be helpful to read this
`introductory article`_ on user-space device drivers, and possibly the `Linux
UIO`_ documentation as well.

.. _ioctl(): https://en.wikipedia.org/wiki/Ioctl
.. _introductory article:
    https://www.embedded.com/device-drivers-in-user-space
.. _Linux UIO:
    https://www.kernel.org/doc/html/latest/driver-api/uio-howto.html
.. _Xenomai UDD: https://xenomai.org/documentation/xenomai-3/html/xeno3prm/group__rtdm__udd.html

..  sphinx-include-background-end

Architecture
============

..  sphinx-include-architecture-start

The purpose of the Cuddl project is to make writing portable user-space
device drivers as easy as possible.  This is done by providing a common set
of user-space and kernel-space programming interfaces that hide the operating
system and platform-specific implementation details, as shown below.


..  sphinx-include-arch-diagram-start

.. raw:: html

   <!-- special case code for clickable svg files -->
   <object data="images/cuddl_arch.svg"
           width=100% type="image/svg+xml">
   </object>

.. only:: latex

   .. image:: images/cuddl_arch.*
      :align: center
      :scale: 100%

..  sphinx-include-arch-diagram-end

As you can see from the diagram above, most of the OS-dependent code is
provided by Cuddl.  This allows driver developers to focus on writing
portable hardware-interface code without having to wade through a lot of
platform-specific details.  Driver developers must implement an OS-dependent
"shell" for their kernel driver stubs, but the hardware-specific code should
be fairly portable.

In addition to memory region mapping and interrupt event notification
functionality, Cuddl also provides an interface to request and query the
hardware peripherals that have been registered.

Cuddl currently aims to support the `Linux`_, `Xenomai`_, and `RTEMS`_
operating systems, and the implementation details are a little different for
each one.

Xenomai
-------

One thing to note is that the Xenomai implemention is a superset of the Linux
implementation, meaning that real-time Xenomai applications and standard
Linux applications may both access Cuddl devices.

The following software-stack diagram shows how the various pieces of software
come together for the Xenomai implementation.

..  sphinx-include-xeno-arch-diagram-start

.. raw:: html

   <!-- special case code for clickable svg files -->
   <object data="images/cuddl_xeno_sw_stack.svg"
           width=80% type="image/svg+xml">
   </object>

.. only:: latex

   .. image:: images/cuddl_xeno_sw_stack.*
      :align: center
      :scale: 80%

..  sphinx-include-xeno-arch-diagram-end

As you can see from the diagram, this particular implementation is built on
top of the `Xenomai UDD`_ and `Linux UIO`_ frameworks.

Linux
-----

The Linux implementation is very similar to the Xenomai implementation
described in the previous section.  The software stack diagram is applicable
if you ignore the Cobalt-related blocks in the diagram.

RTEMS
-----

RTEMS support has not yet been implemented.

..  sphinx-include-architecture-end
