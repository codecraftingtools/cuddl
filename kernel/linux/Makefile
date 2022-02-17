# Cross-platform user-space device driver layer Linux kernel makefile
#
# See https://www.kernel.org/doc/html/latest/kbuild/modules.html

KDIR ?= /lib/modules/`uname -r`/build
CUDDL_TOP = ../..

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD quiet= clean
	/usr/bin/find $(CUDDL_TOP) -name '*~' | xargs $(RM)
