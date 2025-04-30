# SPDX-License-Identifier: GPL-2.0-only
# Copyright (C) 2025 DreamLin <xiao_xuan2020@163.com>
# FYKernelPLUS - Advanced Resource Manager for Linux 6.x+

obj-m := fykernelplus.o
fykernelplus-objs := main.o cpu_scheduler.o gpu_manager.o memory_manager.o procfs_interface.o sysfs_interface.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -std=gnu11 -Wall -Wextra -Wno-unused-parameter -DVERSION=\"1.1.1\"
ccflags-y += -I$(src)/nvidia

ifneq ($(wildcard /usr/src/nvidia),)
ccflags-y += -DENABLE_NVIDIA_OPTIMIZATION=1
fykernelplus-objs += nvidia/nvidia_optimization.o
endif

all: module cli

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

cli: fykernelplus-cli.c
	gcc -o FYKernelPLUS fykernelplus-cli.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f FYKernelPLUS

install: module cli
	sudo insmod fykernelplus.ko
	sudo cp FYKernelPLUS /usr/local/bin/

uninstall:
	sudo rmmod fykernelplus
	sudo rm -f /usr/local/bin/FYKernelPLUS