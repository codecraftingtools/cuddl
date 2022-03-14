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
   
============
Installation
============

Linux
=====

This section applies to all Linux-based systems, including those using
Xenomai.

Device Permissions
------------------

In order for Linux UIO devices and Cuddl devices to be accessible to non-root
users, the desired permissions must be established by providing a ``udev``
rules file.  This can be accomplished by creating a new file named (e.g.)
*99-cuddl.rules* in the */etc/udev/rules.d/* directory with the following
content::

  KERNEL=="uio*", MODE="0660", GROUP="sudo"
  SUBSYSTEM=="cuddl", MODE="0660", GROUP="sudo"

The ``GROUP`` may be changed to something more suitable, if desired.

On Xenomai systems, the UDD / RTDM devices should already be set up for
access by users in the ``xenomai`` group, but the above step must still be
performed to allow access to the Cuddl manager device node.

Getting the Source Code
-----------------------

The Cuddl source code can be checked out from GitHub like this::

  git clone https://github.com/codecraftingtools/cuddl.git

Building the Kernel Modules
---------------------------

The ``cuddl.ko`` and ``cuddl_manager.ko`` kernel modules can be built as
follows::

  cd cuddl
  cd kernel/linux
  make

Inserting the Kernel Modules
----------------------------

If the built-in Linux UIO code has been configured as a module::

  sudo modprobe uio

If running a Xenomai kernel and UDD is configured as a module::

  sudo modprobe udd

Now insert the kernel modules that were built in the previous step::

  sudo insmod cuddl.ko
  sudo insmod cuddl_manager.ko
