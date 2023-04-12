KERNELDIR := /home/qybs/Alpha/kernel/alientek_kernel
CROSS_COMPILE := arm-linux-gnueabihf-
CURRENT_PATH := $(shell pwd)
obj-m := led.o

build: kernel_modules all

all:
	$(CROSS_COMPILE)gcc -o ledApp ledApp.c
kernel_modules: 
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	