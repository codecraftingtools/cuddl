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

============
Event Source
============

**C++ event source declarations.**

Event sources are used to wake up user-space tasks when a specific event
(such as a hardware interrupt from a specific peripheral device) occurs.  The
following entities are defined in the ``cuddl`` namespace.

.. doxygenenum:: cuddl::EventSrcFlag

.. doxygentypedef:: cuddl::EventSrcFlags

.. doxygenenum:: cuddl::EventSrcClaimFlag

.. doxygentypedef:: cuddl::EventSrcClaimFlags

.. doxygenenum:: cuddl::EventSrcOpenFlag

.. doxygentypedef:: cuddl::EventSrcOpenFlags

.. doxygenclass:: cuddl::EventSrcInfo
   :undoc-members:
   :members:

.. doxygenclass:: cuddl::TimeSpec
   :undoc-members:
   :members:

.. doxygenclass:: cuddl::EventSrc
   :undoc-members:
   :members:

.. doxygenclass:: cuddl::EventSrcSet
   :undoc-members:
   :members:
