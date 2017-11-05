obj-m := vmlatency.o
vmlatency-objs := module.o vmx.o

ccflags-y := -Wno-declaration-after-statement -std=gnu99

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
