# @Author: qlc
# @Date:   2018-10-22T17:39:17+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-23T11:52:12+08:00
# @License: GPL
TAGET=mydev
TM=main
obj-m=$(TAGET).o
$(TAGET)-objs=chardev_lock.o led.o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/host/usr/bin/arm-cortex_a9-linux-gnueabi-
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/$(TAGET).ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
debug:
	$(CROSS_COMPILE)gcc $(TM).c -o $(TM)
	cp $(TM) /rootfs
debug_clean:
	rm -rf $(TM)
