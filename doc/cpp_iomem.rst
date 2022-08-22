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

=================
I/O Memory Access
=================

**C++ declarations for I/O memory access.**

User-space C++ applications may use this API to read from (or write to) a
peripheral’s hardware registers in a platform-independent manner.  The
following entities are defined in the ``cuddl`` namespace.

.. doxygentypedef:: cuddl::iomem_t

.. doxygenvariable:: cuddl::ioread8

.. doxygenvariable:: cuddl::ioread16

.. doxygenvariable:: cuddl::ioread32

.. doxygenvariable:: cuddl::iowrite8

.. doxygenvariable:: cuddl::iowrite16

.. doxygenvariable:: cuddl::iowrite32
