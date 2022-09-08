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
   
===============
Getting Started
===============

Downloading the Source Code
===========================

The Cuddl source code can be checked out from GitHub using this command::

  git clone https://github.com/codecraftingtools/cuddl.git

Linux Installation
==================

This section applies to all Linux-based systems, including those using
Xenomai.

Setting Up Device Permissions
-----------------------------

..  sphinx-include-device-permissions-start

In order for Linux UIO devices and Cuddl devices to be accessible to non-root
users, the desired permissions must be established by providing a ``udev``
rules file.  This can be accomplished by creating a new file named (e.g.)
``99-cuddl.rules`` in the ``/etc/udev/rules.d/`` directory with the following
content::

  KERNEL=="uio*", MODE="0660", GROUP="sudo"
  SUBSYSTEM=="cuddl*", MODE="0660", GROUP="sudo"

The ``GROUP`` may be changed to something more suitable, if desired.  On
Xenomai systems, for example, the following rules are probably more
appropriate::

  KERNEL=="uio*", MODE="0660", GROUP="xenomai"
  SUBSYSTEM=="cuddl*", MODE="0660", GROUP="xenomai"

On Xenomai systems, the UDD / RTDM device nodes should already be set up for
access by users in the ``xenomai`` group, but the above step must still be
performed to allow access to the Cuddl manager and janitor device nodes.

..  sphinx-include-device-permissions-end

.. include:: ../kernel/linux/README_KBUILD.rst

Setting up Modprobe
-------------------

..  sphinx-include-modprobe-start

Manually loading the required kernel modules in the correct order (as
demonstrated above) can be tedious and error prone.  Thankfully, the Linux
kernel developers have provided a way to automatically load a module's
dependencies automatically.  This does, however, require a little bit of
setup.

The first step is to determine the kernel release we are running::

  uname -r

The result should be something like ``5.4.0-117-generic``.

The next step is to create a subdirectory to hold our new out-of-tree
modules under the corresponding system ``/lib/modules`` directory::

  sudo mkdir /lib/modules/5.4.0-117-generic/kernel/cuddl

or, more elegantly::

  sudo mkdir /lib/modules/`uname -r`/kernel/cuddl

Next, copy the modules we built into this new directory::

  sudo cp *.ko /lib/modules/`uname -r`/kernel/cuddl

Now adjust the permissions::

  sudo chmod -R o+rX /lib/modules/`uname -r`/kernel/cuddl

Finally, run ``depmod`` to re-compute the module dependencies for the running
kernel::

  sudo /sbin/depmod -a

Now you should be able to load our new out-of-tree kernel modules using
modprobe::

  sudo modprobe cuddl_janitor
  
Note that this command can be run from any directory, that the ``.ko``
extension is not specified, and any prerequisite kernel modules are
automatically loaded (i.e. ``cuddl_manager``, ``cuddl``, and ``uio`` in this
case).

The ``modprobe`` command can also be used to remove a module, along with any
prerequisite kernel modules that are no longer being used::

  sudo modprobe -r cuddl_janitor
  
..  sphinx-include-modprobe-end

Loading Modules on Boot
-----------------------

..  sphinx-include-boot-start

When deploying a set of user-space device drivers to an operational system,
you probably want them to be automatically loaded when the system boots up.
This can be achieved by creating a new file named (e.g.)  ``cuddl.conf`` in
the ``/etc/modules-load.d/`` directory with content like this::

  cuddl_janitor
  other_module_1
  other_module_2

The format is simply the name of the modules to be loaded, one per line.
Note that dependencies should be loaded automatically, like with
``modprobe``.

..  sphinx-include-boot-end

Building a User-Space Application
---------------------------------

There are currently no makefiles or build scripts provided for the Cuddl
user-space code, so users will need to compile and link the required source
files into their applications manually.

Assuming that ``cuddl_DIR`` is set to the root directory of the Cuddl source
repository, the following include directories should be added to the
C-preprocessor search path::

  $(cuddl_DIR)/user/include
  $(cuddl_DIR)/common/include

The following source files also need to be compiled and linked with the
application::

  $(cuddl_DIR)/user/src/cuddl_linux.c

For Xenomai applications, the Cuddl source files need to be compiled and
linked with the appropriate flags (as supplied by ``xeno-config``).

In order to get a meaningful result from ``cuddl_get_userspace_commit_id()``,
the following c-preprocessor flags need to be added::

  -DCUDDLI_COMMIT_HASH=\"`git -C $(cuddl_DIR) rev-parse HEAD`\"
  -DCUDDLI_REPO_IS_DIRTY=`git -C $(cuddl_DIR) diff --quiet HEAD && echo 0 || echo 1`

Otherwise, the result will yield ``"UNKNOWN"``.

RTEMS Installation
==================

RTEMS support has not yet been implemented.


.. include:: README_DOCS.rst
