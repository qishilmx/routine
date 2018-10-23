# @Author: qlc
# @Date:   2018-10-23T16:48:59+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-23T17:17:36+08:00
# @License: GPL
TAGET=io
TM=debug_w
TM1=debug_r
obj-m=$(TAGET).o
$(TAGET)-objs= stack_r.o wait.o
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
	$(CROSS_COMPILE)gcc $(TM1).c -o $(TM1)
	cp $(TM) /rootfs
	cp $(TM1) /rootfs
debug_clean:
	rm -rf $(TM)
	rm -rf $(TM1)
