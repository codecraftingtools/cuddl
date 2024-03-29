Building the Kernel Modules
---------------------------

..  sphinx-include-build-modules-start

The ``cuddl.ko``, ``cuddl_manager.ko``, and ``cuddl_janitor.ko`` kernel
modules can be built by executing the following commands from the root
directory of the ``cuddl`` source tree::

  cd kernel/linux
  make

Note that verbose build output may be shown by running ``make V=1`` instead
of ``make``.  For more details on building out-of-tree kernel modules, you
can refer to the `official kernel.org documentation`_.

If you want to clean up the generated files so that you can recompile
everything from scratch, you can run this command in the ``kernel/linux``
subdirectory ::

  make clean

.. _official kernel.org documentation:
   https://www.kernel.org/doc/html/latest/kbuild/modules.html

..  sphinx-include-build-modules-end

Inserting the Kernel Modules
----------------------------

..  sphinx-include-insert-modules-start

If the UIO subsystem in the Linux kernel you are running was configured as a
module, you will need to insert it before attempting to insert the Cuddl
modules::

  sudo modprobe uio

Similarly, if you are running a Xenomai kernel with UDD configured as a
module, you will need to insert it as well::

  sudo modprobe xeno_udd

Now you can insert the Cuddl kernel modules that were built in a previous
step by executing the following commands from the ``kernel/linux``
subdirectory in this order::

  sudo insmod cuddl.ko
  sudo insmod cuddl_manager.ko
  sudo insmod cuddl_janitor.ko

After inserting (or attempting to insert) kernel modules, you may want to
inspect the kernel ring buffer for any warning or informational messages that
may be relevant.  You can run a command like this to see the last few
messages::

  dmesg | tail -n 40

or you can run this command in a separate terminal to get updates
on-the-fly::

  dmesg -w

.. note::

  Kernel modules can be removed from the running kernel using the ``rmmod``
  command::
   
    sudo rmmod cuddl_janitor cuddl_manager cuddl
   
  Note that multiple kernel modules may be specified, and that the ``.ko``
  file extension is omitted for each module.  Also note that this command can
  be run from any directory (i.e. the ``.ko`` don't need to be present).

..  sphinx-include-insert-modules-end
