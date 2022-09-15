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
Manager
=======

**C++ device manager declarations.**

The Cuddl device manager exposes device memory regions and event sources to
user-space applications. The routines described here may be used to query the
device memory regions and event sources that have been made available to
user-space applications by Cuddl kernel drivers.  The following entities are
defined in the ``cuddl`` namespace.

.. doxygenfunction:: cuddl::get_max_managed_devices

.. doxygenfunction:: cuddl::get_max_dev_mem_regions

.. doxygenfunction:: cuddl::get_max_dev_events

.. doxygenfunction:: cuddl::get_memregion_id_for_slot

.. doxygenfunction:: cuddl::get_eventsrc_id_for_slot

.. doxygenfunction:: cuddl::get_memregion_info_for_id

.. doxygenfunction:: cuddl::get_eventsrc_info_for_id

.. doxygenfunction:: cuddl::get_memregion_ref_count_for_id

.. doxygenfunction:: cuddl::get_eventsrc_ref_count_for_id

.. doxygenfunction:: cuddl::decrement_memregion_ref_count_for_id

.. doxygenfunction:: cuddl::decrement_eventsrc_ref_count_for_id

.. doxygenfunction:: cuddl::get_driver_info_for_memregion_id

.. doxygenfunction:: cuddl::get_driver_info_for_eventsrc_id

.. doxygenfunction:: cuddl::get_hw_info_for_memregion_id

.. doxygenfunction:: cuddl::get_hw_info_for_eventsrc_id

