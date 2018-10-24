# @Author: qlc
# @Date:   2018-10-24T14:58:31+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-24T15:00:54+08:00
# @License: GPL
#TAGET=bus
TM=debug
obj-m=bus.o dev.o dri.o
#$(TAGET)-objs=Basics.o #可以写多个文件，将多个文件编译成一个modules
#obj-m=编译多个模块
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/host/usr/bin/arm-cortex_a9-linux-gnueabi-
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/*.ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
debug:
	$(CROSS_COMPILE)gcc $(TM).c -o $(TM)
	cp $(TM) /rootfs
debug_clean:
	rm -rf $(TM)
