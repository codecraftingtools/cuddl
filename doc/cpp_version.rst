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
   
.. highlight:: C++

=======
Version
=======

**C++ declarations related to version information.**

User-space C++ applications may use this API to retrieve information about
the version of the source code used to build the Cuddl kernel modules and
user-space interface code.  The following entities are defined in the
``cuddl`` namespace.

.. doxygenclass:: cuddl::Version
   :undoc-members:
   :members:

.. doxygenvariable:: cuddl::version

.. doxygenfunction:: cuddl::get_kernel_version

.. doxygenfunction:: cuddl::get_kernel_commit_id

.. doxygenfunction:: cuddl::get_userspace_commit_id

.. doxygenfunction:: cuddl::get_kernel_variant
