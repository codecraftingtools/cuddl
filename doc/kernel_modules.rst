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

.. highlight:: shell

==============
Kernel Modules
==============

..  sphinx-include-modules-doc-start

The following Linux kernel modules are provided by the ``cuddl`` repository
for operational usage on Linux UIO / Xenomai UDD systems.

cuddl
  Provides core Cuddl functionality.  This excludes the device manager API.

  Requires: ``uio``, ``xeno_udd`` (Xenomai UDD only)

cuddl_manager
  Provides Cuddl device manager functionality.  This module implements the
  device manager API, which allows user-space applications to query and claim
  memory regions and event sources of managed devices.

  Requires: ``cuddl``, ``uio``, ``xeno_udd`` (Xenomai UDD only)

cuddl_janitor
  Provides Cuddl janitor functionality.  This module attempts to catch and
  clean up after user-space programs that do not properly release all of
  their Cuddl resources when they terminate.

  Requires: ``cuddl_manager``, ``cuddl``, ``uio``, ``xeno_udd`` (Xenomai UDD
  only)

..  sphinx-include-modules-doc-end
