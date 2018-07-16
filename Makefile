obj-m := vmlatency.o
vmlatency-objs := ./linux/module.o ./linux/api.o ./vmm/vmx.o ./linux/guest.o \
                  ./linux/vmentry.o

srctree := /lib/modules/$(shell uname -r)/build
HAS_BOOL := $(shell grep _Bool $(srctree)/include/linux/types.h \
			> /dev/null 2>&1 && echo -DHAS_BOOL)

INCLUDE_FLAGS:= -I$(PWD)/vmm
EXTRA_CFLAGS += $(HAS_BOOL) $(INCLUDE_FLAGS)
EXTRA_AFLAGS += $(INCLUDE_FLAGS)

all:
	make -C $(srctree) M=$(PWD) modules

clean:
	make -C $(srctree) M=$(PWD) clean
