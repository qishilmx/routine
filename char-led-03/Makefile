# @Author: qlc
# @Date:   2018-10-18T14:47:26+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-18T14:50:51+08:00
# @License: GPL
TAGET=led
TM=debug
obj-m=$(TAGET).o
$(TAGET)-objs=x6818_led.o char_leds.o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/host/usr/bin/arm-cortex_a9-linux-gnueabi-
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	$(CROSS_COMPILE)gcc $(TM).c -o $(TM)
	cp $(PWD)/$(TAGET).ko /rootfs
	cp $(TM) /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
	rm -rf $(TM)
