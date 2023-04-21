KERNELDIR := /home/qybs/Alpha/kernel/alientek_kernel
CROSS_COMPILE := arm-linux-gnueabihf-
CURRENT_PATH := $(shell pwd)
obj-m := newchrdev.o

build: kernel_modules all

all:
	$(CROSS_COMPILE)gcc -o newchrdevApp newchrdevApp.c
kernel_modules: 
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	rm -rf newchrdevApp
	