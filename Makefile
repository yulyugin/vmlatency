obj-m := vmlatency.o
vmlatency-objs := module.o vmx.o

# Do not change flags for module.o!
# It might cause compatibility problems with kernel.
CFLAGS_vmx.o := -Wno-declaration-after-statement -std=gnu99

HAS_BOOL := $(shell grep _Bool $(srctree)/include/linux/types.h \
			>& /dev/null && echo -DHAS_BOOL)

EXTRA_CFLAGS += $(HAS_BOOL)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
