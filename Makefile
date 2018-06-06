KVERSION    = $(shell uname -r)
KMAKE       = $(MAKE) -C /lib/modules/$(KVERSION)/build

CC          = gcc
CFLAGS      = -Wall -Werror

MKDIR       = mkdir -p
LN          = ln -sf
RM          = rm -rf

ifeq ($(VERBOSE), yes)
QUIET=
else
QUIET=@
endif

DRIVER_DIR=build-vmlatency-$(KVERSION)

all: vmlatency-driver

prepare-driver-dir:
	$(QUIET)$(MKDIR) $(DRIVER_DIR)
	$(QUIET)$(LN) ../vmm/module.c $(DRIVER_DIR)
	$(QUIET)$(LN) ../vmm/vmx.c $(DRIVER_DIR)
	$(QUIET)$(LN) ../vmm/GNUMakefile $(DRIVER_DIR)/Makefile
	$(QUIET)for x in vmm/*.h ; do $(LN) ../$$x $(DRIVER_DIR) ; done

vmlatency-driver: prepare-driver-dir
	$(QUIET)$(KMAKE) M="$$PWD/$(DRIVER_DIR)"

clean-driver:
	$(QUIET)$(RM) "$(DRIVER_DIR)"

clean: clean-driver
