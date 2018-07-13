obj-m := vmlatency.o
vmlatency-objs := ./linux/module.o ./linux/api.o ./vmm/vmx.o ./linux/guest.o

# Do not change flags for module.o!
# It might cause compatibility problems with kernel.
CFLAGS_vmx.o := -Wno-declaration-after-statement -std=gnu99

srctree := /lib/modules/$(shell uname -r)/build
HAS_BOOL := $(shell grep _Bool $(srctree)/include/linux/types.h \
			> /dev/null 2>&1 && echo -DHAS_BOOL)

EXTRA_CFLAGS += $(HAS_BOOL) -I$(PWD)/vmm

all:
	make -C $(srctree) M=$(PWD) modules

clean:
	make -C $(srctree) M=$(PWD) clean
