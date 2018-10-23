# ARM课程笔记

[TOC]

## 软件安装

## 嵌入式开发简介

### arm历史版本

arm 公司不做芯片，只研`cpu`架构

arm1、arm2、arm3  .... arm6

| version | cpu            | soc          | cpu num |
| ------- | -------------- | ------------ | ------- |
| v4      | arm7TDMI       | S3C44B0      | 1       |
|         | arm920T        | s3C2440      | 1       |
| V6      | arm1176        | S3C6410      | 1       |
| V7      | arm-contex-a8  | S5PV210      | 1       |
|         |                | AM335X       | 1       |
|         | arm-contex-a9  | Exyonsar4412 | 4       |
|         |                | IMXA6Q       | 4       |
|         |                | IMXA6D       | 2       |
|         |                | OMAP4460     | 2       |
|         |                | S5P4418      | 4       |
|         | arm-contex-r   | r4/r6        |         |
|         | arm-contex-m3  | STM32F103    | 1       |
|         | arm-contex-m4  | STM32F407    | 1       |
| V8      | arm-contex-a53 | S5P6818      | 8       |

## Linux 区别与 Android

`app` 			 **QQ 微信 游戏 控制界面** 

`GUI` 			 **QT(C++) Android(Java) GTK(C) MiniGUI(C)** 

`本地可执行程序`      **ls mkdir sudo mv ....** 

`本地库` 			 **libc.so.6 libjpeg.so libstdc++ ....** 

`文件系统`

`kernel`			 **Linux Android windows** 

`bootloader`		 **uboot ** 

------

`板卡` =SOC+DDR+EMMC(NAND)+LCD+TS+SOUND+NET.....

`SOC` =CPU+总线+外设(控制器：uart、gpio)

`cpu` = arm-cortex+cache(哈佛结构：数据cache 指令cache;冯诺伊曼：统一cache )+mmu+GIC+.....

------

`Linux`

​	uboot---->uImage/zImage----->rootfs

​	emmc[uboot uImage |rootfs]

`Android` 最低arm-contex-a8

​	Uboot----->uImage/zImage----->ramdisk /

​								---->system 	root权限(出场软件)

​								---->data		家目录(安装的软件)

​								---->cache	缓存

​								---->storage	存储内容

​	emmc[uboot uImage ramdisk|system|data|cache|storage]

## 开发板和SDK

### sdk目录介绍

```shell
├── s5p6818sdk_lzy1
│   ├── extern
│   ├── Linux
│   └── package
```

> `s5p6818sdk_lzy1` 为`SDK`根目录
>
>  `extern`存储一些其他资源
>
> `Linux` 存储`SDK`的文件
>
> `package` 存储手册

### extern目录介绍

```shell
extern/
├── arm-linux-gcc-4.5.1.tar.bz2 #交叉编译器
├── ARM架构手册.pdf 
├── jdk-7u80-linux-x64.tar.gz
├── lamp_off1.png
├── lamp_off.png
├── lamp_on1.png
├── lamp_on.png
├── sqlite-autoconf-3170000.tar.gz #sqlite数据库源码
└── x6818bv3.pdf #开发板原理图
```

### Linux目录介绍

```shell
Linux/
├── images #存储现在已经有的一些镜像文件
├── prebuilts #编译内核和uboot的交叉比编译器目录
├── src #源码目录
└── tools #制作boot.img文件的目录
```

## package 目录介绍

```shell
package/ 
├── DataSheet #芯片数据手册
├── dic  
├── hardware #硬件原理图
├── source 
└── tool
```

### package/src目录介绍

```shell
src/
├── buildroot_dl.tar.gz #buidroot 需要的软件包
├── buildroot.tar.bz2 #buildroot 源码文件
├── linux 
├── linux-3.4.tar.bz2 #linux 源码文件
├── prototype
└── uboot6818.tar.bz2 #uboot源码文件
```

## 搭建嵌入式开发环境

### 配置交叉编译器

解压交叉编译器

```shell
cd ~/s5p6818sdk_lzy1/extern/
tar -xvf arm-linux-gcc-4.5.1.tar.bz2 -C /opt
```

配置环境变量

```shell
vim ~/.bashrc
#文件末尾添加
export PATH=$PATH:/opt/4.5.1/bin
```

刷新环境变量

```shell
source ~/.bashrc
```

## 创建sd卡分区

1. 将内存卡插入`读卡器`接入电脑

2. 卸载U盘

   ```shell
   sudo umount /dev/sdb1
   ```

   >  **注：`/dev/sdb` 为存储设备,`/dev/sdb1`、`/dev/sdb2` ..... 为该存储设备的分区**

3. 执行分区命令

   > 计算分区的大小 :
   > $$
   > (end-start)*0.1+start
   > $$
   > 例子：
   >
   > First sector (2048-62963711, default 2048):
   >
   > （62963711-2048）×0.1 +  2048=6298214
   >
   > 注：
   >
   > 分区的最小为 uboot大小+512Kb

   ```shell
   sudo fdisk /dev/sdb
   Welcome to fdisk (util-linux 2.27.1).
   Changes will remain in memory only, until you decide to write them.
   Be careful before using the write command.
   
   
   命令(输入 m 获取帮助)：p
   Disk /dev/sdb: 30 GiB, 32237420544 bytes, 62963712 sectors
   Units: sectors of 1 * 512 = 512 bytes
   Sector size (logical/physical): 512 bytes / 512 bytes
   I/O size (minimum/optimal): 512 bytes / 512 bytes
   Disklabel type: dos
   Disk identifier: 0x00000000
   
   设备       启动   Start   末尾   扇区 Size Id 类型
   /dev/sdb1       6298214 62963711 56665498  27G  b W95 FAT32
   
   命令(输入 m 获取帮助)：d
   Selected partition 1
   Partition 1 has been deleted.
   
   命令(输入 m 获取帮助)： p
   
   Disk /dev/sdb: 30 GiB, 32237420544 bytes, 62963712 sectors
   Units: sectors of 1 * 512 = 512 bytes
   Sector size (logical/physical): 512 bytes / 512 bytes
   I/O size (minimum/optimal): 512 bytes / 512 bytes
   Disklabel type: dos
   Disk identifier: 0x00000000
   
   命令(输入 m 获取帮助)： n
   Partition type
      p   primary (0 primary, 0 extended, 4 free)
      e   extended (container for logical partitions)
   Select (default p): p
   分区号 (1-4, default 1):
   First sector (2048-62963711, default 2048): 6298214
   Last sector, +sectors or +size{K,M,G,T,P} (6288214-62963711, default 62963711):
   Created a new partition 1 of type 'Linux' and of size 27 GiB.
   
   命令(输入 m 获取帮助):t
   Partition type (type L to list all types): b
   Changed type of partition 'Linux' to 'W95 FAT32'.
   
   命令(输入 m 获取帮助)：w
   The partition table has been altered.
   Calling ioctl() to re-read partition table.
   Syncing disks.
   ```

4. 格式化SD卡分区

   ```shell
   sudo mkfs.vfat /dev/sdb1
   ```

   > 注意：`/dev/sdb` 为SD卡设备，在自己电脑上名字有可能改变
   >
   > 格式化时一定要格式化分区，即`/dev/sdb1` 

## 烧写uboot和配置minicom

### 烧写uboot

```shell
 sudo dd if=ubootpak.bin of=/dev/sdb seek=1
 sync
```

> 注 ubootpak.bin 文件在 `$SDK_PATH/s5p6818sdk_lzy1/Linux/images`中

### 配置minicom

```shell
sudo minicom -s

    +-----[configuration]------+
    | Filenames and paths      |
    | File transfer protocols  |
    | Serial port setup        |
    | Modem and dialing        |
    | Screen and keyboard      |
    | Save setup as dfl        |
    | Save setup as..          |
    | Exit                     |
    | Exit from Minicom        |
    +--------------------------+
    # 选择 Serial port setup 按 Enter 按键进入下面的界面
 	+-----------------------------------------------------------------------+
    | A -    Serial Device      : /dev/ttyUSB0                              |
    | B - Lockfile Location     : /var/lock                                 |
    | C -   Callin Program      :                                           |
    | D -  Callout Program      :                                           |
    | E -    Bps/Par/Bits       : 115200 8N1                                |
    | F - Hardware Flow Control : No                                        |
    | G - Software Flow Control : No                                        |
    |                                                                       |
    |    Change which setting?                                              |
    +-----------------------------------------------------------------------+
    # 输入 A 修改 Serial Device 为 /dev/ttyUSB0
    # 输入 E 进入下面的界面
   +---------[Comm Parameters]----------+
   |                                    |
   |     Current: 115200 8N1            |
   | Speed            Parity      Data  |
   | A: <next>        L: None     S: 5  |
   | B: <prev>        M: Even     T: 6  |
   | C:   9600        N: Odd      U: 7  |
   | D:  38400        O: Mark     V: 8  |
   | E: 115200        P: Space          |
   |                                    |
   | Stopbits                           |
   | W: 1             Q: 8-N-1          |
   | X: 2             R: 7-E-1          |
   |                                    |
   |                                    |
   | Choice, or <Enter> to exit?        |
   +------------------------------------+
   #输入 E 按 Enter 回到
	+-----------------------------------------------------------------------+
    | A -    Serial Device      : /dev/ttyUSB0                              |
    | B - Lockfile Location     : /var/lock                                 |
    | C -   Callin Program      :                                           |
    | D -  Callout Program      :                                           |
    | E -    Bps/Par/Bits       : 115200 8N1                                |
    | F - Hardware Flow Control : Yes                                       |
    | G - Software Flow Control : No                                        |
    |                                                                       |
    |    Change which setting?                                              |
    +-----------------------------------------------------------------------+
  # 输入 F 关闭硬件流控
  	+-----------------------------------------------------------------------+
    | A -    Serial Device      : /dev/ttyUSB0                              |
    | B - Lockfile Location     : /var/lock                                 |
    | C -   Callin Program      :                                           |
    | D -  Callout Program      :                                           |
    | E -    Bps/Par/Bits       : 115200 8N1                                |
    | F - Hardware Flow Control : No                                        |
    | G - Software Flow Control : No                                        |
    |                                                                       |
    |    Change which setting?                                              |
    +-----------------------------------------------------------------------+
   # 按 Enter 按键保存 ，回到主菜单
   +-----[configuration]------+                                     
   | Filenames and paths      |                                     
   | File transfer protocols  |                                     
   | Serial port setup        |                                     
   | Modem and dialing        |                                     
   | Screen and keyboard      |                    
   | Save setup as dfl        |                    
   | Save setup as..          |                    
   | Exit                     |                    
   | Exit from Minicom        |                    
   +--------------------------+
   # 选择 Save setup as dfl 按 Enter 保存为默认配置
   # 选择 Exit from Minicom 按 Enter 退出配置
```

### 开发板刷机

1. 下载软件包

   ```shell
   sudo apt install android-tools-fastboot
   ```

2. 将usb转串口和开发板连接

3. 将Android线和电脑连接

4. 打开`minicom`

   ```shell
   sudo minicom
   ```

5. 打开开发板，然后计入`uboot` 命令终端

   - 看着minicom
   - 开发板上电，等待`minicom`上打印的消息出现倒计时，按`空格` 键进入 `uboot` 命令终端
   - 执行`fastboot` 命令

6. 进入到 `$SDK_PATH/Linux/images`,执行刷机脚本

   ```shell
   sudo ./flush.sh
   ```
   刷机脚背详解

   > 烧写uboot

   ```shell
   sudo fastboot flash ubootpak ubootpak.bin
   ```

   > 烧写boot.img(有kernel、启动图片、刷机图片、ramdisk、ramdisk-recovery.img ....)

   ```shell
   sudo fastboot flash boot boot.img
   ```

   > 烧写文件系统

   ```shell
   sudo fastboot flash system rootfs.ext2
   ```

   

7. 命令执行完毕之后，重启开发板

8. 设置开发的内核启动参数，在`uboot`中执行命令

   ```
   setenv bootargs "root=/dev/mmcblk0p2 rootfstype=ext4 console=ttySAC0,115200 lcd=wy070ml  tp=gslx680-linux"
   ```

   ```shell
   setenv bootcmd "ext4load mmc 2:1 0x48000000 uImage;bootm 0x48000000"
   ```

   ```shell
   save
   ```

   ```shell
   reset
   ```

## 编译`uboot`

1. 解压源码

   ```shell
   cd /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src
   tar -xvf uboot6818.tar.bz2
   ```

2. 进入 `uboot` 源码目录修改 `Makefile`

   ```shell
   cd uboot
   vim Makefile
   ```

   ```shell
   # 将 203 行修改为下面的值
   CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
   ```

3. 修改 `board/s5p6818/x6818/config.mk` 文件

   ```shell
   vim board/s5p6818/x6818/config.mk
   ```

   ```shell
   # 将 27 行修改为下面的值
   CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
   ```

4. 花屏再做如下修改

   ```shell
   vim board/s5p6818/x6818/x6818-lcds.c
   ```

   ```shell
   #将 215行替换为如下内容
   CFG_DISP_MIPI_PLLPMS = 0x2281;
   #将 216行替换为如下内容
   CFG_DISP_MIPI_BANDCTL = 0x8;
   ```

5. 重新编译

   ```shell
   make x6818_config
   make -j4
   ```

6. 烧写 uboot

   ```shell
   sudo fastboot flash ubootpak ubootpak.bin
   ```

## 编译内核

1. 解压内核源码

   ```shell
   cd /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/
   tar -xvf linux-3.4.tar.bz2
   ```

2. 进入内核目录，修改`Makefile`

   ```shell
   cd kernel/
   vim Makefile
   ```

   ```makefile
   #将 Makefile 的 196 行替换为下面的内容
   CROSS_COMPILE   ?= /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
   ```

3. 修改屏幕花屏问题

   ```shell
   vim arch/arm/plat-s5p6818/x6818/x6818-lcds.c
   ```

   ```c
   // 将208行替换为以下内容
   CFG_DISP_MIPI_BANDCTL = 0x8;
   ```

4. 修改触摸屏幕的触摸点与实际相反的问题

   ```shell
   vim drivers/input/touchscreen/gslX680.c
   ```

   ```c
   //在36行下面添加
   extern unsigned char lcdname[32];
   ```

   ```c
   // 在50行下面添加
   static int is_rotate= 0;
   ```

   ```c
   /*
   删除 703 行附近的
   id = cinfo.id[i];
   x =  cinfo.x[i];
   y =  cinfo.y[i];
   替换为
   */
   if(is_rotate > 0){
   	id = cinfo.id[i];
   	x =  SCREEN_MAX_Y - cinfo.x[i];
   	y =  SCREEN_MAX_X - cinfo.y[i];
   }
   else{
   	id = cinfo.id[i];
   	x =  cinfo.x[i];
   	y =  cinfo.y[i];
   }
   ```

   ```c
   // 在1153 行下面添加以下内容
   if(strcasecmp(lcdname, "wy070ml") == 0){
   	printk("wy070ml: gslx680 Touch Driver\n");
   	is_rotate =1;
   }
   else{
   	is_rotate = 0;
   }
   ```

5. 修改内核编译过程中找不到 `lcdname` 变量的问题

   ```shell
   vim arch/arm/plat-s5p6818/x6818/x6818-lcds.c
   ```

   ```c
   // 替换59行为
   unsigned char lcdname[32] = "vs070cxn";
   ```

   ```c
   // 在 59 行下添加
   EXPORT_SYMBOL(lcdname);
   ```

6. 使用默认配置，并编译内核

   ```
   make x6818_defconfig
   make -j4
   ```

7. 生成`uImge`镜像

   ```shell
   make uImage
   ```

   >注：如何执行出错，修要下载
   >
   >```shell
   >sudo apt install u-boot-tools
   >```

### 生成boot.img

1. 进入存在boot的文件夹

   ```shell
   cd /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/tools
   ```

2. 解压 `boot.tar.bz2`

   ```shell
   tar -xvf boot.tar.bz2
   ```

3. 使用自己编译出来的`uImage`替换`boot`文件夹中的内容

   ```shell
   cd boot/
   cp /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/kernel/arch/arm/boot/uImage ./
   ```

4. 制作`boot.img`

   ```shell
   cd ../
   ./make_ext4fs -s -l 60M -a boot boot.img ./boot
   ```
   > 注：出错 下载
   >
   > ```shell
   > sudo apt install lib32stdc++
   > ```
   >
   > 

5. 烧写 boot.img（跟新内核）

   ```shell
    sudo fastboot flash boot boot.img
   ```


## 编译文件系统

先下载一些软件包

```shell
sudo apt install texinfo
```

1. 进入源码目录,解压源码

   ```shell
   cd/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src
   tar -xvf buildroot.tar.bz2
   cd buildroot/
   ```

2. 使用默认配置

   ```shell
   make x6818_defconfig
   make
   ```

3. 等待出现`../include/curses.h:1594:56: note: in definition of macro ‘mouse_trafo’` 错误

   ```shell
   vim ./output/build/host-ncurses-5.9/include/curses.h
   ```

   ```c
   // 替换 1584 行为
   extern NCURSES_EXPORT(bool)    mouse_trafo (int*, int*, bool);
   ```

   ```shell
   vim ./output/build/host-ncurses-5.9/include/curses.tail
   ```

   ```c
   // 替换 104 行为
   extern NCURSES_EXPORT(bool)    mouse_trafo (int*, int*, bool);
   ```

4. 重新编译

   ```shell
   make
   ```


## 配置网络下载内核启动

1. 下载软件

   ```shell
   sudo apt install tftpd-hpa
   ```

2. 配置`tftpd-hpa`软件

   ```shell
   vim /etc/default/tftpd-hpa
   ```

   ```
   TFTP_USERNAME="tftp"
   TFTP_DIRECTORY="/tftpboot"
   TFTP_ADDRESS=":69"
   TFTP_OPTIONS="-l -c -s"
   ```

3. 创建 `/tftpboot`文件夹，并修改所属用户和组,并重启tftp服务

   ```shell
   sudo mkdir /tftpboot
   sudo chown `whoami`:`whoami` /tftpboot
   sudo service tftpd-hpa restart
   ```

4. 设置pc的ip地址

   >网关为 192.168.100.254
   >
   >ip为 192.168.100.155
   >
   >掩码为 255.255.255.0

5. 进入uboot，设置uboot环境变量

   ```shell
   setenv gatewayip "192.168.100.254"
   setenv ipaddr "192.168.100.123"
   setenv netmask "255.255.255.0"
   setenv serverip "192.168.100.155"
   save
   ```

6. 开发板`ping` 电脑

   ```
   ping 电脑ip
   ```

   > 注： 
   >
   > 电脑如果和开发板可以通信，命令的执行结果为 `host 192.168.100.155 is alive`
   >
   > 电脑如果和开发板不可以通信，命令的执行结果为 `ping failed; host 192.168.100.155 is not alive`
   >
   > 第一次`ping` 不会成功，一般最少需要两次

7. 网络下载内核

   1. 将 uImage文件发到 `/tftpboot` 目录下

     ```shell
     cp /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/kernel/arch/arm/boot/uImage /tftpboot/
     ```

   2. 在uboot中执行以下的任何一条命令即可

     ```shell
     tftpboot 0x48000000 uImage
     ```

     或

     ```shell
     tftp 0x48000000 uImage
     ```

     或

     ```shell
     tftp
     ```

     > 注：三条命令都可以，但是在设置环境变量时，必须使用第一条命令

8. 启动内核

   ```shell
   bootm 0x48000000
   ```

9. 设置 uboot 的开机自动下载内核并启动`bootcmd`参数

   ```shell
   setenv bootcmd "ping 192.168.6.6;ping 192.168.6.6;tftpboot 0x48000000 uImage;bootm 0x48000000"
   save
   ```

   > 注 192.168.6.6 为自己tftp服务器的地址

## 网络挂在文件系统启动开发板

1. 下载软件

   ```shell
   sudo apt install nfs-kernel-server
   ```

2. 配置`nfs`

   ```shell
   sudo vim /etc/exports
   ```

   ```sehll
   # 在文件最后，添加以下内容
   /rootfs *(rw,sync,no_root_squash)
   ```

3. 创建文件夹,并修改所属用户和组

   ```shell
   sudo mkdir /rootfs
   sudo chown `whoami`:`whoami` /rootfs
   ```

4. 重启nfs服务

   ```shell
   sudo service nfs-kernel-server restart
   ```

5. 解压文件系统

   ```shell
   cp /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/images/rootfs.tar /rootfs/
   cd /rootfs
   sudo tar -xvf rootfs.tar
   ```

6. 修改文件内容

   ```shell
   sudo vim etc/network/interfaces
   ```

   ```
   #将文件内容替换为以下内容
   # Configure Loopback
   auto lo
   iface lo inet loopback
   
   auto eth0
   #iface eth0 inet dhcp
   ```

7. 修改 uboot的`bootargs`

   ```shell
   setenv bootargs "root=/dev/nfs nfsroot=192.168.6.6:/rootfs ip=192.168.6.15 console=ttySAC0,115200 lcd=wy070ml tp=gslx680-linux"
   save
   ```

## 配置交叉编译器

1. 修改`~/.bashrc `

   ```shell
   vim ~/.bashrc
   ```

   ```shell
   #在最后添加
   export PATH=$PATH:/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/host/usr/bin
   ```

2. 更新环境变量

   ```shell
    source ~/.bashrc
   ```

## 添加系统调用

一切都在编译的内核目录修改的，目录是 `/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4`

1. 修改 `arch/arm/include/asm/unistd.h`

   ```shell
   vim arch/arm/include/asm/unistd.h
   ```

   ```c
   // 在 #define __NR_process_vm_writev          (__NR_SYSCALL_BASE+377) 下添加内容
   #define __NR_myadd          (__NR_SYSCALL_BASE+378)
   ```

2. 修改 `arch/arm/kernel/calls.S`

    ```shell
      vim arch/arm/kernel/calls.S
    ```

   ```c
   // 在 CALL(sys_process_vm_writev) 下添加
   CALL(sys_myadd)
   ```

3. 修改 `arch/arm/kernel/sys_arm.c`

   ```shell
   vim arch/arm/kernel/sys_arm.c
   ```

   ```c
   //在文件最后添加
   asmlinkage int sys_myadd(int a, int b)
   {
           return a+b;
   }
   ```

4. 重新编译内核

   ```shell
   make -j4
   make uImage
   ```

5. 将内核放到 tftpboot 目录下

   ```shell
   cp /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4/arch/arm/boot/uImage /tftpboot/
   ```

6. [通过网络下载内核启动](## 配置网络下载内核启动)

7. 编译例子，并将程序放入pc `/rootfs` 目录下

8. 开发板挂在PC共享的目录

   ```shell
   #设置 开发板ip
   ifconfig eth0 192.168.100.12 up
   #开发板挂在PC目录
   mount -t nfs -o nolock 192.168.100.155:/rootfs /mnt
   ```

9. 运行验证程序

## 在kernel中添加module

1. 在内核源码目录中的`drivers`下创建 `uplooking` 目录

   ```shell
   cd /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
   mkdir drivers/uplooking
   ```

2. 在 `drivers/uplooking` 添加`Makefile` 、`uplooking.c` 文件

   ```shell
   touch drivers/uplooking/Makefile drivers/uplooking/uplooking.c
   ```

3. 在 `drivers/uplooking/Makefile` 中添加如下内容

   ```makefile
   obj-y=uplooking.o
   ```

4. 在 `drivers/uplooking/uplooking.c` 中添加如下内容

   ```c
   #define PERR(fmt, args...)                                                     \
     do {                                                                         \
       printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
     } while (0)
   
   #include <linux/init.h>
   #include <linux/module.h>
   
   /**
    * [MODULE_LICENSE 声明该模块遵循的协议]
    */
   MODULE_LICENSE("GPL");
   /**
    * [uplooking_init 内核模块的初始化函数,__init
    * 关键字表明该函数是在模块初始化的时候执行的，模块初始化完成之后，会释放该函数所占的内存]
    * @return  [成功返回 0,失败返回非 0 ]
    */
   static __init int uplooking_init(void) {
     int i = 0;
     for (i = 0; i < 100; i++)
       PERR("INIT\n");
     return 0;
   }
   
   /**
    * [uplooking_exit 内核模块的退出函数,__exit
    * 关键字表明该函数是在模块退出的时候执行的，如果模块被编译到内核中，该代码段不会被编译]
    */
   static __exit void uplooking_exit(void) {
     PERR("EXIT\n");
     return;
   }
   
   /**
    * [module_init 注册模块初始化函数]
    */
   module_init(uplooking_init);
   /**
    * [module_exit 注册模块退出函数]
    */
   module_exit(uplooking_exit);
   ```

5. 修改 `drivers/Makefile` 文件

   ```shell
   vim drivers/Makefile
   ```

   ```makefile
   #在 obj-y += pinctrl/ 下一行添加如下内容
   obj-y                           += uplooking/
   ```

6. 编译内核

   ```shell
   make -j4
   make uImage
   ```

7. 将内核放到tftpboot目录

8. 网络下载内核启动开发板

## 编写`menuconfig` 的配置文件`Kconfig`

### Kconfig 语法

- config

  config <symbol>

   <config options>

  config是关键字，表示一个配置选项的开始；紧跟着的<symbol>是配置选项的名称，省略了前缀"CONFIG_"。

  例如：

> config TMPFS_POSIX_ACL
>
>  bool "Tmpfs POSIX Access Control Lists"
> 
>  ​	depends on TMPFS
> 
>  ​	select GENERIC_ACL
> 
>  ​	help
> 
>  POSIX Access Control Lists (ACLs) support permissions for users and
> 
>  groups beyond the owner/group/world scheme.
> 
>  To learn more about Access Control Lists, visit the POSIX ACLs for
> 
>  Linux website <http://acl.bestbits.at/>.
> 
>  解析：config是关键字，表示一个配置选项的开始；紧跟着的TMPFS_POSIX_ACL是配置选项的名称，省略了前缀"CONFIG_"
> 
>  bool表示变量类型，即"CONFIG_ TMPFS_POSIX_ACL "的类型，有5种类型：bool、tristate、string、hex和int，其中tristate和string是基本的类型
> 
>  ​ bool变量的值：     y和n
> 
>  tristate变量的值： y、n和m
> 
>  ​ string变量的值：   字符串
> 
>  ​ bool之后的字符串“Tmpfs POSIX Access Control Lists”是提示信息，在配置界面中上下移动光标选中它时，就可以通过按空格或回车键来设置CONFIG_ TMPFS_POSIX_ACL的值。
> 
>  depends on：表示依赖于XXX，“depends on TMPFS”表示只有当TMPFS配置选项被选中时，当前配置选项的提示信息才会出现，才能设置当前配置选项。

- menuconfig

  menuconfig <symbol>

    <config options>

  此关键字和前面的关键字很相似，但它在前面的基础上要求所有的子选项作为独立的行显示。

- choice/endchoice

  choice

     <choice options>

     <choice block>

  endchoice

  该关键字定义了一组选择项，并且选项可以是前面描述的任何属性。尽管boolean只允许选择一个配置选项，tristate可以抒多个配置选项设为'm'，但选项只能是boolean或tristate类型。这可以在一个硬件有多个驱动的情况下使用，最终只有一个驱动被编译进/加载到内核，但所有的驱动都可以编译成模块。选项可以接受的另一个选项是"optional"，这样选项就被设置为'n'，没有被选中的。

  例如：

  > choice条目将多个类似的配置选项组合在一起，供用户单选或多选
  >
  > ​       choice
  >
  > ​             prompt "ARM system type"
  >
  > ​             default ARCH_VERSATILE
  >
  > ​             config ARCH_AAEC2000
  >
  > ​                  .........
  >
  > ​             config ARCH_REALVIEW
  >
  > ​                  .........
  >
  > ​        endchoice
  >
  > ​      prompt "ARM system type"给出提示信息“ARM system type”，光标选中后回车进入就可以看到多个config条目定义的配置选项。choice条目中定义的变量只有bool和tristate。

- comment

  "comment" <prompt>

   <comment options>

  这里定义了在配置过程中显示给用户的注释，该注释还将写进输出文件中。唯一可用的可选项是依赖关系。

  例如：

> comment条目用于定义一些帮助信息，出现在界面的第一行，如在arch/arm/Kconifg中有如下代码：
>
>  
> 
>  
> 
>   menu "Floating point emulation"
> 
>  ```
>  	comment "At least one emulation must be selected"
>  
>  	config FPE_NWFPE
>  
>  	.........                                                                              
>  
>  	config FPE_NWFPE_XP
>  
>  ```
> 
>    endmenu

  

- menu/endmenu

  "menu" <prompt>

    <menu options>

    <menu block>

    "endmenu"

  这里定义了一个菜单，详细信息请看前面的"菜单结构"。唯一可用的可选项是依赖关系。

  例如：

> menu条目用于生成菜单，其格式如下：
>
>  ```
>    Floating poing emulation--->
>  
>  ​	[] FPE_NWFPE
>  
>   	[] FPE_NWFPE_XP              
>  
>  ```

- if/endif

  "if" <expr>

     <if block>

     "endif"

  这里定义了if结构。依赖关系<expr>被加到所有在if ... endif 中的菜单选项中。

- source

  source条目用于读取另一个Kconfig文件，如：

  > source "net/Kconifg"

### 根据`Kconfig` 生成的文件

1. `include/config/auto.conf`

   内核中所有的`Makefile` 文件都会引用该文件

2. `include/generated/autoconf.h` 

   内核中所有代码都会包含此文件

### 使用编写`Kconfig` 文件

例如，在uplooking下创建了`kconfig` 文件修要在上一级目录的kconfig文件

```shell
cd /opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4/drivers/uplooking
vim ../Kconfig
```

```makefile
# 在 source "drivers/base/Kconfig" 下体添加
source "drivers/uplooking/Kconfig"
```

## 编写内核模块

### 编译单个内核模块

```makefile
# @Author: fjk
# @Date:   2018-10-11T14:05:49+08:00
# @Email:  sunnyfjk@gmai.com
# @Filename: Makefile
# @Last modified by:   fjk
# @Last modified time: 2018-10-11T14:26:32+08:00
# @License: GPL
TAGET=Kernel01
obj-m=$(TAGET).o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/$(TAGET).ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean

```

> TAGET需要生成ko的名字，需要有和它同名 `.c` 文件
>
> obj-m 他的值为编译成内核模块的`.c` 文件所对应的`.o` 文件
>
> KERN_PATH **编译过的内核源码**的路径
>
> PWD 内核模块源码的路径

```shell
make -C $(KERN_PATH) M=$(PWD) modules
# 去指定的$(KERN_PATH)目录找 Makefile 执行
# M=$(PWD)  让内核的Makefile找到当前的内核源码编译
# modules 编译内核模块的命令
```

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:05:58+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel01.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:13:53+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

static __init int Kernel01_init(void) {
  PERR("INIT\n");
  return 0;
}

static __exit void Kernel01_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel01_init);
module_exit(Kernel01_exit);
```

### 编译多个模块

```makefile
# @Author: fjk
# @Date:   2018-10-11T14:05:49+08:00
# @Email:  sunnyfjk@gmai.com
# @Filename: Makefile
# @Last modified by:   fjk
# @Last modified time: 2018-10-11T14:22:45+08:00
# @License: GPL
TAGET=Kernel02
TAGET01=Kernel02_01
obj-m=$(TAGET).o $(TAGET01).o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/*.ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
```

> obj-m 如果会面有多个 `.o` 则将`.o` 所对应的`.c` 文件都编译为内核模块

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:20:05+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel02_01.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:21:28+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

static __init int Kernel02_01_init(void) {
  PERR("INIT\n");
  return 0;
}

static __exit void Kernel02_01_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel02_01_init);
module_exit(Kernel02_01_exit);
```

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:19:12+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel02.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:20:53+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

static __init int Kernel02_init(void) {
  PERR("INIT\n");
  return 0;
}

static __exit void Kernel02_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel02_init);
module_exit(Kernel02_exit);
```

### 将多个`.c` 文件编译成为一个内核模块文件

多个文件编译为同一个内核模块时，只能其中一个文件中存在 `module_init` 和 `module_exit` 函数

```makefile
# @Author: fjk
# @Date:   2018-10-11T14:05:49+08:00
# @Email:  sunnyfjk@gmai.com
# @Filename: Makefile
# @Last modified by:   fjk
# @Last modified time: 2018-10-11T14:26:18+08:00
# @License: GPL
TAGET=Kernel03
obj-m=$(TAGET).o
$(TAGET)-objs=Kernel_main.o Kernel_other.o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/$(TAGET).ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
```

> 多个文件编译成为一个内核模块文件时，`obj-m` 后指定的`.o`文件名字 不能时 但前文件夹中的任何`.c` 所对应的 `.o` 文件
>
> $(TAGET)-objs 后面填写需要将那些`.c` 文件编译成一个内核模块。需要填写`.c` 文件对应的`.o` 文件

````c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:25:18+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel_other.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:32:03+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>
#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

void my_printk(void) {
  int i = 0;
  for (i = 0; i < 10; i++)
    PERR("\n");
}
EXPORT_SYMBOL(my_printk);
````

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:25:03+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel_main.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:31:46+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

extern void my_printk(void);

static __init int Kernel03_init(void) {
  PERR("INIT\n");
  my_printk();
  return 0;
}

static __exit void Kernel03_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel03_init);
module_exit(Kernel03_exit);
```

### 模块之间的依赖关系

```makefile
# @Author: fjk
# @Date:   2018-10-11T14:05:49+08:00
# @Email:  sunnyfjk@gmai.com
# @Filename: Makefile
# @Last modified by:   fjk
# @Last modified time: 2018-10-11T14:34:51+08:00
# @License: GPL
TAGET=Kernel04
TAGET01=Kernel04_01
obj-m=$(TAGET).o $(TAGET01).o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/*.ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
```

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:19:12+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel04.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:35:23+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

extern void my_printk(void);

static __init int Kernel04_init(void) {
  PERR("INIT\n");
  my_printk();
  return 0;
}

static __exit void Kernel04_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel04_init);
module_exit(Kernel04_exit);
```

```c
/**
 * @Author: fjk
 * @Date:   2018-10-11T14:20:05+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: Kernel04_01.c
 * @Last modified by:   fjk
 * @Last modified time: 2018-10-11T14:34:39+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");

void my_printk(void) {
  int i = 0;
  for (i = 0; i < 10; i++)
    PERR("\n");
}
EXPORT_SYMBOL(my_printk);

static __init int Kernel04_01_init(void) {
  PERR("INIT\n");
  return 0;
}

static __exit void Kernel04_01_exit(void) {
  PERR("EXIT\n");
  return;
}

module_init(Kernel04_01_init);
module_exit(Kernel04_01_exit);

```

> **如果需要跨模块引用 函数或变量 需要使用 EXPORT_SYMBOL 导出函数或变量的名称**

### 使用 `modprobe` 安装模块时Makefile的写法

```makefile
# @Author: fjk
# @Date:   2018-10-11T14:05:49+08:00
# @Email:  sunnyfjk@gmai.com
# @Filename: Makefile
# @Last modified by:   fjk
# @Last modified time: 2018-10-11T14:41:45+08:00
# @License: GPL
TAGET=Kernel05
TAGET01=Kernel05_01
obj-m=$(TAGET).o $(TAGET01).o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/*.ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
install:
	make -C $(KERN_PATH) M=$(PWD) modules_install INSTALL_MOD_PATH=/rootfs
```

> INSTALL_MOD_PATH 指定要将内核模块下载的根目录

## 内存地址分布(虚拟地址)

### MMU 映射关系

1. 段映射

   查找页表条目：虚拟地址的 [31:20]可找到页表的条目

   条目中保存真实的物理地址的[31:20]

   条目的[31:20] 和 虚拟地址的[19:0]组合找到真实的物理地址

2. 小页映射（最常用的）

   通过虚拟地址的[31:20]找到一级页表的条目

   一级页表条目中的[31:20]位保存二级页表的基地址

   根据居虚拟地址的[19:12]位找到二级页表的条目

   二级页表条目中保存真实的物理地址[31:12]

   二级页表条目[31:12] 和 虚拟地址的[11:0]组合找到真实的物理地址

### 内存管理系统

为了减少内存碎片，保留尽可能多的连续的物理内存

1. buddy
2. slab

[内存映射目录](Documentation/arm/memory.txt)

| Start         | End           | Use                                                          |
| ------------- | ------------- | ------------------------------------------------------------ |
| ffff8000      | ffffffff      |                                                              |
| ffff4000      | ffffffff      |                                                              |
| ffff1000      | ffff7fff      |                                                              |
| ffff0000      | ffff0fff      | arm 异常向两表                                               |
| fffe0000      | fffeffff      | 用于 XScale 芯片                                             |
| fffe8000      | fffeffff      | D_Cache 仅耦合性内存                                         |
| fffe0000      | fffe7fff      | I_Cache 仅耦合性内存                                         |
| fff00000      | fffdffff      | 固定映射区，映射高端内存 kmap_atomic                         |
| ffc00000      | ffefffff      | DMA使用，dma_alloc_xxx 函数申请的内存                        |
| ff000000      | ffbfffff      | 保留的                                                       |
| VMALLOC_START | VMALLOC_END-1 | vmalloc 申请的内存<br />ioremap 物理地址转换为虚拟地址       |
| PAGE_OFFSET   | high_memory-1 | 直接映射区，把普通区映射到次，1:1映射                        |
| PKMAP_BASE    | PAGE_OFFSET-1 | 把高端内存，映射到内核空间，可以使用 kmap                    |
| MODULES_VADDR | MODULES_END-1 | 模块存在的地址。                                             |
| 00001000      | TASK_SIZE-1   | 用户空间使用，每一个线程将空间映射到此。每个用户3-4G的映射都是一样的，0-3G是不一样的 |
| 00000000      | 00000fff      | 没有被映射，不能使用                                         |



### 内存函数

在normal区 

alloc_page

\__free_page 

alloc_pages 

\__free_pages    

\__get_free_page 

free_page

\__get_free_pages 

free_pages   

page_address   获取页的虚拟地址   

kmap

kunmap

kmap_atomic 

\__kummap_atomic 

------

常用的

kmalloc     normal  物理和虚拟都是连续的   字节为单位申请

 kzalloc

 kfree

 vmalloc   只能保证虚拟连续   字节申请  分配大的内存，不要求物理上连续  能够把不连续的物理内存，映射到一个连续的虚拟空间 

vzalloc 

vfree    

dma_alloc_coherent 

dma_free_coherent  

缓存 

kmem_cache_create 

kmem_cache_destory 

kmem_cache_alloc 

kmem_cache_free  

ioremap   把物理地址映射为虚拟地址

iounmap

## proc 文件系统

- proc 在内存当中。是开机自动挂载的。
- proc sysfs tmpfs ext4 ext2 NTFS VFAT32 
- proc 文件系统的头文件为 `include/linux/proc_fs.h`
- 在proc文件系统中 `struct proc_dir_entry` 结构可以表示为 `目录` 和 `文件`
- 会调用 `read_proc_t` 函数的上级函数为 `__proc_file_read` 

### 函数

**struct proc_dir_entry *proc_mkdir(const char * name,struct proc_dir_entry *parent);**

|          | 介绍                                                | 备注 |
| -------- | --------------------------------------------------- | ---- |
| 函数功能 | 在proc 文件系统中创建文件夹                         |      |
| 函数参数 | `name` 文件的名字  `parent` 父目录                  |      |
| 返回值   | 成功返回 `创建的目录的结构体的指针` 失败返回 `NULL` |      |

**void remove_proc_entry(const char *name, struct proc_dir_entry *parent);**

|          | 介绍                                          | 备注                            |
| -------- | --------------------------------------------- | ------------------------------- |
| 函数功能 | 删除 proc 中的`目录`或`文件`                  |                                 |
| 函数参数 | `name` `目录`或`文件` 的名字，`parent` 父目录 | 如果没有 父目录 `parent`写 NULL |
| 返回值   | 无                                            |                                 |

**struct proc_dir_entry *create_proc_read_entry(const char *name,umode_t mode, struct proc_dir_entry *base, read_proc_t *read_proc, void * data)**

|          | 介绍                                                         | 备注                               |
| -------- | ------------------------------------------------------------ | ---------------------------------- |
| 函数功能 | 创建只读的proc文件                                           | 内核的旧接口，新版本内核已经不用了 |
| 函数参数 | `name` `文件` 的名字，`mode` 文件的权限 `base` 父目录 `read_proc` 读取的函数 `data` 私有数据 | 如果没有 父目录 `parent`写 NULL    |
| 返回值   | 成功返回 `创建的文件的结构体的指针` 失败返回 `NULL`          |                                    |

**struct proc_dir_entry *create_proc_entry(const char *name, umode_t mode, struct proc_dir_entry *parent)**

|          | 介绍                                                | 备注                                                         |
| -------- | --------------------------------------------------- | ------------------------------------------------------------ |
| 函数功能 | 创建proc文件节点                                    | 需要自己为 `struct proc_dir_entry` 中的 `read_proc`、`write_proc` 、`data` 成员进行赋值 |
| 函数参数 | `name` 文件名字 `mode` 文件权限 `parent` 父级目录   |                                                              |
| 返回值   | 成功返回 `创建的文件的结构体的指针` 失败返回 `NULL` |                                                              |

**struct proc_dir_entry *proc_create_data(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct file_operations *proc_fops, void *data);**

|          | 介绍                                                         | 备注 |
| -------- | ------------------------------------------------------------ | ---- |
| 函数功能 | 创建proc文件节点                                             |      |
| 函数参数 | `name` 文件名字 `mode` 文件权限 `parent` 父级目录 `proc_fops` proc文件操作的接口结构体 `data` 私有数据 |      |
| 返回值   | 成功返回 `创建的文件的结构体的指针` 失败返回 `NULL`          |      |

### 数据结构

**struct file_operations**

```c
struct file_operations {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        int (*readdir) (struct file *, void *, filldir_t);
        unsigned int (*poll) (struct file *, struct poll_table_struct *);
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);
        int (*fsync) (struct file *, loff_t, loff_t, int datasync);
        int (*aio_fsync) (struct kiocb *, int datasync);
        int (*fasync) (int, struct file *, int);
        int (*lock) (struct file *, int, struct file_lock *);
        ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        int (*check_flags)(int);
        int (*flock) (struct file *, int, struct file_lock *);
        ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
        ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
        int (*setlease)(struct file *, long, struct file_lock **);
        long (*fallocate)(struct file *file, int mode, loff_t offset,
                          loff_t len);
};
```

| 接口                                                         | 介绍                                              |      |
| ------------------------------------------------------------ | ------------------------------------------------- | ---- |
| int (*open) (struct inode *, struct file *);                 | 在用户层使用系统调用`open`函数的时会调用到该结构  |      |
| int (*release) (struct inode *, struct file *);              | 在用户层使用系统调用`close`函数的时会调用到该结构 |      |
| ssize_t (*read) (struct file *, char __user *, size_t, loff_t *); | 在用户层使用系统调用`read`函数的时会调用到该结构  |      |
| ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *); | 在用户层使用系统调用`write`函数的时会调用到该结构 |      |
| long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long); | 在用户层使用系统调用`ioctl`函数的时会调用到该结构 |      |

### proc 文件使用流程

用户层的 	open 			close 			read 			write

​			|				|				|				|

***

​			一系列的其他操作找到 proc 文件结构体对应的 struct file_operations中

内核		|				|				|				|

​			open			release			read			write

​			|				|				|				
			seq_open		seq_release		seq_read

​											|

​											start->show->next->show->next

​																	|next==NULL	->stop

​																	|next!=NULL		->show	

***

## 设备驱动

char 设备驱动 

网络设备驱动

块设备驱动

### 字符设备驱动

input驱动（键盘、鼠标、触摸屏 等等）、sound、fb（显示屏）、 misc设备

#### 基础char设备的写法

1. 查看现在已经存在的设备号`cat /proc/devices`
2. char 设备的核心结构体为 struct cdev
3. struct module 结构体指针赋值为 `THIS_MODULE`

* 注册设备的流程

  1. 创建设备号
  2. 注册设备号
  3. 初始化struct cdev结构体
  4. 注册字符备

* 注销流程

  1. 注销字符设备
  2. 注销设备号

* 创字符设备文件

  >  mknod -m 0644 mycdev c 243 0
  >
  > -m 指定创建的文件的权限
  >
  > mycdev 设备名字
  >
  > c 文件类型
  >
  > 240 主设备号
  >
  > 0 次设备号

### char 驱动函数

#### 1.查看没有使用设备号

> cat /proc/devices

#### 2.#define MKDEV(ma,mi)

| 名字   | 说明                        | 备注          |
| ------ | --------------------------- | ------------- |
| 功能   | 合成设备号                  |               |
| 参数   | `ma` 主设备号 `mi` 次设备号 |               |
| 返回值 | 设备号                      | 类型：`dev_t` |

#### 3.#define MAJOR(dev)

| 名字   | 说明         | 备注          |
| ------ | ------------ | ------------- |
| 功能   | 取出主设备号 |               |
| 参数   | `dev` 设备号 | 类型：`dev_t` |
| 返回值 | 主设备号     |               |

#### 4.#define MINOR(dev)

| 名字   | 说明         | 备注          |
| ------ | ------------ | ------------- |
| 功能   | 取出次设备号 |               |
| 参数   | `dev` 设备号 | 类型：`dev_t` |
| 返回值 | 次设备号     |               |

#### 5.int register_chrdev_region(dev_t from, unsigned count, const char *name);

| 名字   | 说明                                           | 备注 |
| ------ | ---------------------------------------------- | ---- |
| 功能   | 注册设备号                                     |      |
| 参数   | `from` 起始设备号 `count` 注册个数 `name` 名字 |      |
| 返回值 | 成功：`0` 失败：`非0`                          |      |

#### 6.int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,  const char *name);

| 名字   | 说明                                                         | 备注 |
| ------ | ------------------------------------------------------------ | ---- |
| 功能   | 申请设备号并注册                                             |      |
| 参数   | `dev` 存放设备号 `baseminor` 次设备号起始 `count` 注册设备号个数 `name` 名字 |      |
| 返回值 | 成功：`0` 失败：`非0`                                        |      |

#### 7.void unregister_chrdev_region(dev_t from, unsigned count);

| 名字   | 说明                                     | 备注 |
| ------ | ---------------------------------------- | ---- |
| 功能   | 删除注册的设备号                         |      |
| 参数   | `from` 起始设备号 `count` 删除注册的个数 |      |
| 返回值 | 无                                       |      |

### 自动创建设备文件

#### 1.#define class_create(owner, name);

| 名字   | 说明                            | 备注           |
| ------ | ------------------------------- | -------------- |
| 功能   | 创建类                          |                |
| 参数   | `owner` THIS_MODULE `name` 名字 |                |
| 返回值 | 成功：`结构体指针` 失败：`NULL` | `struct class` |

#### 2.void class_destroy(struct class *cls);

| 名字   | 说明               | 备注 |
| ------ | ------------------ | ---- |
| 功能   | 删除类             |      |
| 参数   | `cls` 类结构体指针 |      |
| 返回值 | 无                 |      |

#### 3.struct device *device_create(struct class *class, struct device *parent,dev_t devt, void *drvdata, const char *fmt, ...);

| 名字   | 说明                                                         | 备注               |
| ------ | ------------------------------------------------------------ | ------------------ |
| 功能   | 创建设备文件                                                 |                    |
| 参数   | `class` 类结构体指针 `parent` 设备指针 `devt` 设备号 `drvdata` 驱动数据 `fmt` 文件名字 | `fmt` 描述不太准确 |
| 返回值 | 无                                                           |                    |

#### 4.void device_destroy(struct class *class, dev_t devt)

| 名字   | 说明                               | 备注 |
| ------ | ---------------------------------- | ---- |
| 功能   | 删除设备文件                       |      |
| 参数   | `class` 类结构体指针 `devt` 设备号 |      |
| 返回值 | 无                                 |      |

## 时钟设置函数

#### 1.struct clk *clk_get(struct device *dev, const char *id);

| 名字   | 说明                            | 备注 |
| ------ | ------------------------------- | ---- |
| 功能   | 获取时钟                        |      |
| 参数   | `dev` 设备 `id` 时钟名字        |      |
| 返回值 | 成功：`时钟结构体` 失败：`NULL` |      |

#### 2.int clk_enable(struct clk *clk);

| 名字   | 说明                  | 备注 |
| ------ | --------------------- | ---- |
| 功能   | 使能时钟              |      |
| 参数   | `clk` 时钟结构体      |      |
| 返回值 | 成功：`0` 失败：`非0` |      |

#### 3.void clk_disable(struct clk *clk);

| 名字   | 说明             | 备注 |
| ------ | ---------------- | ---- |
| 功能   | 关闭时钟         |      |
| 参数   | `clk` 时钟结构体 |      |
| 返回值 | 无               |      |

#### 4.void clk_put(struct clk *clk);

| 名字   | 说明             | 备注 |
| ------ | ---------------- | ---- |
| 功能   |                  |      |
| 参数   | `clk` 时钟结构体 |      |
| 返回值 | 无               |      |

#### 5.unsigned long clk_get_rate(struct clk *clk);

| 名字   | 说明             | 备注 |
| ------ | ---------------- | ---- |
| 功能   | 获取时钟频率     |      |
| 参数   | `clk` 时钟结构体 |      |
| 返回值 | 时钟频率         |      |

### linux中断

产生中断异常之后：

- 计算`中断线`[^中断线] 
- 执行 asm_do_IRQ 函数
- 找到struct irq_desc 

#### 函数

**typedef irqreturn_t (*irq_handler_t)(int irq, void * dev);**

| 名字   | 说明                                          | 备注 |
| ------ | --------------------------------------------- | ---- |
| 功能   | 中断处理函数                                  |      |
| 参数   | ``irq` 中断线`dev` 设备结构体，或者是私有数据 |      |
| 返回值 | 成功：`IRQ_HANDLED` 失败：`IRQ_NONE`          |      |

**int __must_check request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,const char *name, void *dev);**

| 名字   | 说明                                                         | 备注 |
| ------ | ------------------------------------------------------------ | ---- |
| 功能   | 注册中断                                                     |      |
| 参数   | `irq` 中断线 `handler` 中断处理函数 `flags` 响应中断的模式 `name` 中断的名字 `dev` 设备结构体，或者是私有数据 |      |
| 返回值 | 成功：`0` 失败：`!0`                                         |      |

**void free_irq(unsigned int irq, void *dev_id)**

| 名字   | 说明                                    | 备注                       |
| ------ | --------------------------------------- | -------------------------- |
| 功能   | 释放注册的中断线                        |                            |
| 参数   | irq 中断线，dev_id 注册时传入的私有数据 | 必须和注册时传入的数据一致 |
| 返回值 | 无                                      |                            |

**#define in_interrupt()**

| 名字   | 说明                           | 备注 |
| ------ | ------------------------------ | ---- |
| 功能   | 判断程序执行的环境             |      |
| 参数   | 无                             |      |
| 返回值 | `!0` 中断上下文 `0` 进程上下文 |      |

#### 通过函数计算中断线

需要包含的头文件是

```c
#include <linux/gpio.h>
#include <mach/platform.h>
```

**int gpio_to_irq(unsigned gpio);**

| 名字   | 说明                       | 备注 |
| ------ | -------------------------- | ---- |
| 功能   | 根据GPIO号转换成GPIO中断线 |      |
| 参数   | `gpio` GPIO 号             |      |
| 返回值 | 中断线                     |      |

注意：

查找GPIO的方法

> /*  gpio group pad start num. */
> enum {
>     PAD_GPIO_A      = (0 * 32),
>     PAD_GPIO_B      = (1 * 32),
>     PAD_GPIO_C      = (2 * 32),
>     PAD_GPIO_D      = (3 * 32),
>     PAD_GPIO_E      = (4 * 32),
>     PAD_GPIO_ALV    = (5 * 32),
> };
>
> 例子：
>
> 查找GPIOA28
>
> ```c
> gpio_a_28=PAD_GPIO_A+28;
> ```
>
> 查找GPIOA28中断线
>
> ```c
> gpio_a_28_line=gpio_to_irq(PAD_GPIO_A+28);
> ```

#### 直接计算中断线

> #define IRQ_GPIO_A_START                (IRQ_GPIO_START + PAD_GPIO_A)
> #define IRQ_GPIO_B_START                (IRQ_GPIO_START + PAD_GPIO_B)
> #define IRQ_GPIO_C_START                (IRQ_GPIO_START + PAD_GPIO_C)
> #define IRQ_GPIO_D_START                (IRQ_GPIO_START + PAD_GPIO_D)
> #define IRQ_GPIO_E_START                (IRQ_GPIO_START + PAD_GPIO_E)
>
> 例子：
>
> 查找GPIOA28中断线
>
> ```c
> gpio_a_28_line=IRQ_GPIO_A_START+28;
> ```

## 内核中延时的方法

1. 需要包含 `#include<linux/delay.h>`

2. 睡眠延时函数

   - void ssleep(unsigned int seconds);

     > 秒 级延时

   - void msleep(unsigned int msecs);

     > 毫秒级延时

3. 死循环延时

   - void udelay(n)

     > 微秒 级延时

   - void ndelay(unsigned long x)

     > 纳 秒 级延时

   - mdelay(n)

     > 毫秒级延时

4. 长延时

   jiffies

   初始值为 -5×min[^min] 

   #define time_after(a,b)

5. 获取内核时间

     void do_gettimeofday(struct timeval *tv);

     void getnstimeofday(struct timespec *ts);

## 中断下半部

1. 软中断

   - %99 是永不到的

   - 系统支持的软中断

     ```c
     enum
     {
        HI_SOFTIRQ=0,
        TIMER_SOFTIRQ,
        NET_TX_SOFTIRQ,
        NET_RX_SOFTIRQ,
        BLOCK_SOFTIRQ,
        BLOCK_IOPOLL_SOFTIRQ,
        TASKLET_SOFTIRQ,
        SCHED_SOFTIRQ,
        HRTIMER_SOFTIRQ,
        RCU_SOFTIRQ,    /* Preferable RCU should always be the last softirq */
        NR_SOFTIRQS
     };
     ```

   - #define in_interrupt()

     > 返回真在终端上下文 返回假在进程上下文

     判断当前处在的环境

2. tasklet_struct

   - tasklet 时基于软中断的，它处于中断上下文

   - 不能在tasklet 下半部睡眠

   - tasklet 结构体

     ```c
     struct tasklet_struct
     {
             struct tasklet_struct *next;
             unsigned long state;
             atomic_t count;
             void (*func)(unsigned long);
             unsigned long data;
     };
     ```

   - 函数

     **void tasklet_init(struct tasklet_struct \*t,void (\*func)(unsigned long), unsigned long data);**

     | 名字   | 说明                                                         | 备注 |
     | ------ | ------------------------------------------------------------ | ---- |
     | 功能   | 初始化 `struct tasklet_struct` 结构体                        |      |
     | 参数   | `t` `struct tasklet_struct`结构体指针 `func` 执行下半部时使用的函数指针 `data` 执行中断下半部时需要的数据 |      |
     | 返回值 | 无                                                           |      |

     **static inline void tasklet_schedule(struct tasklet_struct *t)**

     | 名字   | 说明                                  | 备注 |
     | ------ | ------------------------------------- | ---- |
     | 功能   | 调度 `tasklet`                        |      |
     | 参数   | `t` `struct tasklet_struct`结构体指针 |      |
     | 返回值 | 无                                    |      |

     **static inline void tasklet_hi_schedule(struct tasklet_struct *t)**

     | 名字   | 说明                                  | 备注 |
     | ------ | ------------------------------------- | ---- |
     | 功能   | 高优先级调度 `tasklet`                |      |
     | 参数   | `t` `struct tasklet_struct`结构体指针 |      |
     | 返回值 | 无                                    |      |

3. work_struct

   - 处于进程上下文，可以睡眠

   - 结构体

     ```c
     typedef void (*work_func_t)(struct work_struct *work);
     struct work_struct {
             atomic_long_t data;
             struct list_head entry;
             work_func_t func;
     #ifdef CONFIG_LOCKDEP
             struct lockdep_map lockdep_map;
     #endif
     };
     ```

   - 函数

     **#define INIT_WORK(_work, _func)**

     | 名字   | 说明                                                         | 备注 |
     | ------ | ------------------------------------------------------------ | ---- |
     | 功能   | 初始化`struct work_struct` 结构体                            |      |
     | 参数   | _`work` `struct work_struct` 的指针 `func` 执行下半部时使用的函数指针 |      |
     | 返回值 | 无                                                           |      |

     **int schedule_work(struct work_struct *work);**

     | 名字   | 说明                               | 备注 |
     | ------ | ---------------------------------- | ---- |
     | 功能   | 调度 work                          |      |
     | 参数   | `work` `struct work_struct` 的指针 |      |
     | 返回值 | 无                                 |      |

     **void flush_scheduled_work(void);**

     | 名字   | 说明             | 备注 |
     | ------ | ---------------- | ---- |
     | 功能   | 刷新内核工作队列 |      |
     | 参数   | 无               |      |
     | 返回值 | 无               |      |

4. delayed_work

   调度之后，延时n之后在执行函数

   - 结构体

     ```c
     struct delayed_work {
             struct work_struct work;
             struct timer_list timer;
     };
     ```

   - 函数

     **#define INIT_DELAYED_WORK(_work, _func)**

     | 名字   | 说明                                                         | 备注 |
     | ------ | ------------------------------------------------------------ | ---- |
     | 功能   | 初始化`struct work_struct` 结构体                            |      |
     | 参数   | _`work` `struct work_struct` 的指针 `func` 执行下半部时使用的函数指针 |      |
     | 返回值 | 无                                                           |      |

     **int schedule_delayed_work(struct delayed_work *work, unsigned long delay)**

     | 名字   | 说明                                                    | 备注     |
     | ------ | ------------------------------------------------------- | -------- |
     | 功能   | 调度 work 延时 `delay` 时长执行                         |          |
     | 参数   | `work` `struct delayed_work` 的指针 delay 延时时长 n*HZ | HZ 是 1s |
     | 返回值 | 无                                                      |          |

     **bool flush_delayed_work(struct delayed_work *dwork);**

     | 名字   | 说明                                | 备注 |
     | ------ | ----------------------------------- | ---- |
     | 功能   | 刷新 delayed_work                   |      |
     | 参数   | `work` `struct delayed_work` 的指针 |      |
     | 返回值 |                                     |      |

      **bool flush_delayed_work_sync(struct delayed_work *work);**

     | 名字   | 说明                                | 备注 |
     | ------ | ----------------------------------- | ---- |
     | 功能   | 刷新 delayed_work                   |      |
     | 参数   | `work` `struct delayed_work` 的指针 |      |
     | 返回值 |                                     |      |

     **bool cancel_delayed_work_sync(struct delayed_work *dwork);**

     | 名字   | 说明                                | 备注 |
     | ------ | ----------------------------------- | ---- |
     | 功能   | 取消 delayed_work 执行              |      |
     | 参数   | `work` `struct delayed_work` 的指针 |      |
     | 返回值 |                                     |      |

## kernel 并发机制

### 抢占

1.`抢占模式`，内核代码可以被其他代码打断，单CPU情况下加锁相当于关闭`抢占模式`，多个CPU需要加锁

2.`非抢占模式`，内核代码不可以被打断，单CPU情况下可以不加锁，多个CPU需要加锁

临界区的概念[^临界区] ，消费电子一般使用抢占模式，服务器一般使用非抢占模式。



**#define preempt_disable()**

| 名字   | 说明         | 备注                           |
| ------ | ------------ | ------------------------------ |
| 功能   | 关闭内核抢占 | 内核是抢占模式下，否则没有意义 |
| 参数   | 无           |                                |
| 返回值 | 无           |                                |

**#define preempt_enable()**

| 名字   | 说明         | 备注                           |
| ------ | ------------ | ------------------------------ |
| 功能   | 打开内核抢占 | 内核是抢占模式下，否则没有意义 |
| 参数   | 无           |                                |
| 返回值 | 无           |                                |

### 自旋锁

头文件 `include/linux/spinlock.h`

自选锁在`抢占模式`单核CPU上获得锁就是关闭内核抢占，在单核CPU`非抢占模式`函数为空，获得锁之后不能去睡眠。

#### 函数

**#define spin_lock_init(_lock)** 

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 初始化自旋锁            |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 无                      |      |

**static inline void spin_lock(spinlock_t *lock)**

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 获得锁                  |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 无                      |      |

**static inline void spin_unlock(spinlock_t *lock)**

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 释放锁                  |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 无                      |      |

**static inline void spin_lock_bh(spinlock_t *lock)**

| 名字   | 说明                     | 备注 |
| ------ | ------------------------ | ---- |
| 功能   | 获得锁，并禁止中断下半部 |      |
| 参数   | **spinlock_t** 类型指针  |      |
| 返回值 | 无                       |      |

**static inline void spin_unlock_bh(spinlock_t *lock)**

| 名字   | 说明                       | 备注 |
| ------ | -------------------------- | ---- |
| 功能   | 释放锁，取消禁止中断下半部 |      |
| 参数   | **spinlock_t** 类型指针    |      |
| 返回值 | 无                         |      |

**static inline void spin_lock_irq(spinlock_t *lock)**

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 获得锁，并关闭中断      |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 无                      |      |

**static inline void spin_unlock_irq(spinlock_t *lock)**

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 释放锁，打开中断        |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 无                      |      |

**#define spin_lock_irqsave(lock, flags)** 

| 名字   | 说明                                                | 备注 |
| ------ | --------------------------------------------------- | ---- |
| 功能   | 获得锁，并保存中断状态                              |      |
| 参数   | `lock` **spinlock_t** 类型指针 `falgs` 保存中断状态 |      |
| 返回值 | 无                                                  |      |

**static inline void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)**

| 名字   | 说明                                            | 备注 |
| ------ | ----------------------------------------------- | ---- |
| 功能   | 释放锁，恢复中断状态                            |      |
| 参数   | `lock` **spinlock_t** 类型指针 `falgs` 中断状态 |      |
| 返回值 | 无                                              |      |

**static inline int spin_trylock(spinlock_t *lock)**

| 名字   | 说明                    | 备注 |
| ------ | ----------------------- | ---- |
| 功能   | 尝试获得锁              |      |
| 参数   | **spinlock_t** 类型指针 |      |
| 返回值 | 成功`0` 失败`!0`        |      |

### 信号量

头文件 `include/linux/semaphore.h`

如果没有获得信号量，不会死等而会睡眠。当释放信号量时会唤醒睡眠的进程[^进程上下文切换] 

#### 结构体

```c
struct semaphore {
        raw_spinlock_t          lock;
        unsigned int            count;
        struct list_head        wait_list;
};
```

| 名字      | 说明                                                         | 备注                                             |
| --------- | ------------------------------------------------------------ | ------------------------------------------------ |
| lock      | 基于 `raw_spinlock_t` 实现                                   |                                                  |
| count     | 当`count`有值时(!0)可以获得信号量,当`count`没有值时(0)需要等待(睡眠) | 当`count=1`时为互斥信号量，`count>1`为计数信号量 |
| wait_list | 等待队列                                                     |                                                  |

**static inline void sema_init(struct semaphore *sem, int val)**

| 名字   | 说明                               | 备注 |
| ------ | ---------------------------------- | ---- |
| 功能   | 初始化信号量                       |      |
| 参数   | `sem` 信号量指针 `val` count初始值 |      |
| 返回值 | 无                                 |      |

**void down(struct semaphore *sem);**

| 名字   | 说明                   | 备注 |
| ------ | ---------------------- | ---- |
| 功能   | 获得信号量，不可被中断 |      |
| 参数   | `sem` 信号量指针       |      |
| 返回值 |                        |      |

**int \__must_check down_interruptible(struct semaphore *sem);**

| 名字   | 说明                   | 备注                                 |
| ------ | ---------------------- | ------------------------------------ |
| 功能   | 获取信号量，可以被打断 |                                      |
| 参数   | `sem` 信号量指针       |                                      |
| 返回值 | 成功 `0` 失败 `!0`     | 如果被信号打断返回`!0` 返回 `-EINTR` |

**int \__must_check down_killable(struct semaphore *sem);**

| 名字   | 说明                           | 备注                                 |
| ------ | ------------------------------ | ------------------------------------ |
| 功能   | 获取信号量，可以被致命信号打断 |                                      |
| 参数   | `sem` 信号量指针               |                                      |
| 返回值 | 成功 `0` 失败 `!0`             | 如果被信号打断返回`!0` 返回 `-EINTR` |

**int \__must_check down_trylock(struct semaphore *sem);**

| 名字   | 说明               | 备注 |
| ------ | ------------------ | ---- |
| 功能   | 尝试获取信号量     |      |
| 参数   | `sem` 信号量指针   |      |
| 返回值 | 成功 `0` 失败 `!0` |      |

**int \__must_check down_timeout(struct semaphore *sem, long jiffies);**

| 名字   | 说明                                                     | 备注                                 |
| ------ | -------------------------------------------------------- | ------------------------------------ |
| 功能   | 获取信号量，等待`jiffies` 时间，如果没有获得信号量则返回 |                                      |
| 参数   | `sem` 信号量指针 `jiffies` 时间                          | 获取信号量`jiffies` +延时            |
| 返回值 | 成功 `0` 失败 `!0`                                       | 如果被信号打断返回`!0` 返回 `-ETIME` |

**void up(struct semaphore *sem);**

| 名字   | 说明             | 备注 |
| ------ | ---------------- | ---- |
| 功能   | 释放信号量       |      |
| 参数   | `sem` 信号量指针 |      |
| 返回值 |                  |      |

### 互斥量

头文件 `include/linux/mutex.h`

#### 结构体

```c
struct mutex {
        /* 1: unlocked, 0: locked, negative: locked, possible waiters */
        atomic_t                count;
        spinlock_t              wait_lock;
        struct list_head        wait_list;
#if defined(CONFIG_DEBUG_MUTEXES) || defined(CONFIG_SMP)
        struct task_struct      *owner;
#endif
#ifdef CONFIG_DEBUG_MUTEXES
        const char              *name;
        void                    *magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
        struct lockdep_map      dep_map;
#endif
};
```

#### 函数

**#define mutex_init(mutex)**

| 名字   | 说明                | 备注 |
| ------ | ------------------- | ---- |
| 功能   | 初始化互斥量        |      |
| 参数   | `struct mutex` 指针 |      |
| 返回值 |                     |      |

**static inline void mutex\_destroy(struct mutex *lock)**

| 名字   | 说明                | 备注           |
| ------ | ------------------- | -------------- |
| 功能   | 销毁互斥量          | 没有做任何操作 |
| 参数   | `struct mutex` 指针 |                |
| 返回值 |                     |                |

**void mutex_lock(struct mutex *lock);**

| 名字   | 说明                                         | 备注 |
| ------ | -------------------------------------------- | ---- |
| 功能   | 获取信号量，没有获取到睡眠，不可以被信号打断 |      |
| 参数   | `struct mutex` 指针                          |      |
| 返回值 |                                              |      |

**int \__must_check mutex_lock_interruptible(struct mutex *lock);**

| 名字   | 说明                                       | 备注 |
| ------ | ------------------------------------------ | ---- |
| 功能   | 获取信号量，没有获取到睡眠，可以被信号打断 |      |
| 参数   | `struct mutex` 指针                        |      |
| 返回值 | 获得`0` 失败 `!0`                          |      |

**int \__must_check mutex_lock_killable(struct mutex *lock);**

| 名字   | 说明                                           | 备注 |
| ------ | ---------------------------------------------- | ---- |
| 功能   | 获取信号量，没有获取到睡眠，可以被致命信号打断 |      |
| 参数   | `struct mutex` 指针                            |      |
| 返回值 | 获得`0` 失败 `!0`                              |      |

**int mutex_trylock(struct mutex *lock);**

| 名字   | 说明                | 备注 |
| ------ | ------------------- | ---- |
| 功能   | 尝试获得互斥量      |      |
| 参数   | `struct mutex` 指针 |      |
| 返回值 | 获得 `1` 失败 `0`   |      |

**void mutex_unlock(struct mutex *lock);**

| 名字   | 说明                | 备注 |
| ------ | ------------------- | ---- |
| 功能   | 释放互斥量          |      |
| 参数   | `struct mutex` 指针 |      |
| 返回值 |                     |      |

### 读写锁

头文件 `include/linux/rwock.h`，本质上为自旋锁

读加锁，可以多个读者加锁

写加锁，如果需要加写锁，则不能存在写锁和读锁

#### 类型

```c
typedef struct {
        arch_rwlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
        unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
        unsigned int magic, owner_cpu;
        void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
        struct lockdep_map dep_map;
#endif
} rwlock_t;
```

#### 函数

**# define rwlock_init(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**void read_lock(rwlock_t *lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**void read_unlock(rwlock_t *lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_lock_irq(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_unlock_irq(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_lock_bh(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_unlock_bh(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_lock_irqsave(lock, flags)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define read_unlock_irqrestore(lock, flags)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**int read_trylock(rwlock_t *lock);**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**void write_lock(rwlock_t *lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**void write_unlock(rwlock_t *lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_lock_irq(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_unlock_irq(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_lock_bh(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_unlock_bh(lock)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_lock_irqsave(lock, flags)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**#define write_unlock_irqrestore(lock, flags)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

**int write_trylock(rwlock_t *lock);**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

### 读写信号量

头文件 `include/linux/rwsem.h`

#### 结构体

```c
struct rw_semaphore {
        long                    count;
        raw_spinlock_t          wait_lock;
        struct list_head        wait_list;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
        struct lockdep_map      dep_map;
#endif
};
```

#### 函数

- rwsem init 

  **#define init_rwsem(sem)**

- lock for reading
  **void down_read(struct rw_semaphore *sem);**

- trylock for reading -- returns 1 if successful, 0 if contention
  **int down_read_trylock(struct rw_semaphore *sem);**

- lock for writing
  **void down_write(struct rw_semaphore *sem);**

- trylock for writing -- returns 1 if successful, 0 if contention
  **int down_write_trylock(struct rw_semaphore *sem);**

- release a read lock
  **void up_read(struct rw_semaphore *sem);**

- release a write lock
  **void up_write(struct rw_semaphore *sem);**

### 读写锁(写者优先)

- 多个写者不能共存

- 多个读者可以共存

- 在有读者是写者可以写，在有写者时其它写着不可以写

- 读者每次读是都需要检测在读的过程中写者是否来过

  头文件 `include/linux/seqlock.h`

#### 使用方法

```c
do {
  	seq = read_seqbegin(&foo);
  	...
  } while (read_seqretry(&foo, seq));
  
```

#### 结构体

```c
  typedef struct {
          unsigned sequence;
          spinlock_t lock;
  } seqlock_t;
```

|          |                                                              |                                                              |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| sequence | 当为`偶数`时读者可以读，写者可以写，当为`奇数` 时写者不能写，读者不能读 | 读者 读取结束时应该判断`sequence` 是否改变如果改变需要重新读，写者加锁时 `sequence++` 写者解锁时 `sequence++` |
| lock     | 保护 `sequence`                                              |                                                              |

#### 函数

  **#define seqlock_init(x)** 

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

  **static inline void write_seqlock(seqlock_t *sl)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

  **static inline void write_sequnlock(seqlock_t *sl)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

  **static inline int write_tryseqlock(seqlock_t *sl)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

  **static __always_inline unsigned read_seqbegin(const seqlock_t *sl)**

| 名字   | 说明 | 备注                 |
| ------ | ---- | -------------------- |
| 功能   |      |                      |
| 参数   |      | cpu_relax(); cpu休息 |
| 返回值 |      |                      |

  **static __always_inline int read_seqretry(const seqlock_t *sl, unsigned start)**

| 名字   | 说明 | 备注 |
| ------ | ---- | ---- |
| 功能   |      |      |
| 参数   |      |      |
| 返回值 |      |      |

### 原子量

1. 头文件 `arch/arm/include/asm/atomic.h`

2. 数据类型

   ```c
   typedef struct {
           int counter;
   } atomic_t;
   ```

3. 操作函数

   #define atomic_inc(v)           atomic_add(1, v)
   #define atomic_dec(v)           atomic_sub(1, v)

   #define atomic_inc_and_test(v)  (atomic_add_return(1, v) == 0)
   #define atomic_dec_and_test(v)  (atomic_sub_return(1, v) == 0)
   #define atomic_inc_return(v)    (atomic_add_return(1, v))
   #define atomic_dec_return(v)    (atomic_sub_return(1, v))
   #define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)

   #define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)

### 阻塞与非阻塞访问

- 同步 等待数据来到
- 阻塞 如果没有数据一直等，(睡眠)
- 异步 安排任务去工作，工作完成时通过信号、线程通知用户层
- 非阻塞 如果没有数据，出错返回

#### 等待队列(非阻塞访问)

##### 结构体

```c
struct __wait_queue_head {
        spinlock_t lock;
        struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;
```

##### 函数

**#define init_waitqueue_head(q)**

| 名字   | 功能                         | 备注 |
| ------ | ---------------------------- | ---- |
| 功能   | 初始化等待队列               |      |
| 参数   | `q` `wait_queue_head_t` 指针 |      |
| 返回值 |                              |      |

**#define wait_event(wq, condition)**

| 名字   | 功能                                                         | 备注                       |
| ------ | ------------------------------------------------------------ | -------------------------- |
| 功能   | 进入等待                                                     |                            |
| 参数   | `wq` `wait_queue_head_t` 等待队列指针 `condition` 不睡眠条件 | 为`true` 退出 `false` 等待 |
| 返回值 |                                                              |                            |

**#define wait_event_timeout(wq, condition, timeout)**

**#define wait_event_interruptible(wq, condition)**

**#define wait_event_killable(wq, condition)** 

**#define wait_event_interruptible_timeout(wq, condition, timeout)**

**#define wait_event_interruptible_exclusive(wq, condition)**

|      |                      |      |
| ---- | -------------------- | ---- |
|      | 睡眠的时候带互斥标志 |      |
|      |                      |      |
|      |                      | 下   |

**#define wake_up(x)**  

| 名字   | 功能                         | 备注 |
| ------ | ---------------------------- | ---- |
| 功能   | 唤醒队列                     |      |
| 参数   | `x` `wait_queue_head_t` 指针 |      |
| 返回值 |                              |      |

**#define wake_up_nr(x, nr)**

**#define wake_up_all(x)**

**#define wake_up_interruptible(x)**

**#define wake_up_interruptible_nr(x, nr)** 

**#define wake_up_interruptible_all(x)**

```c
#define lock_wait_evnet(wq,cond,lock)						\
{															\
	int ret=0;												\
	DEFINE_WAIT(__wait);									\
	unsigned long flags=0;									\
	spin_lock_irqsave(lock,flags);							\
	while(!cond){											\
   	 	prepare_to_wait(&wq, &__wait, TASK_INTERRUPTIBLE);	\
        if(signal_pending(current)){						\
          finish_wait(&wq, &__wait);						\
          ret=-ERESTARTSYS;									\
          break;											\
        }													\
        spin_unlock_irqrestore(lock,flags);					\
        schedule();											\
        spin_lock_irqsave(lock,flags);						\
        finish_wait(&wq, &__wait);							\
	}														\
 	spin_unlock_irqrestore(lock,flags);						\
 	ret;													\
}
```



#### 高级IO

1.把可能改变进程状态的等待队列放入到table

2.判断设备是否可读或可写

3.返回响应标志

##### 函数

**static inline void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p);**

| 名字   | 功能                                                         | 备注 |
| ------ | ------------------------------------------------------------ | ---- |
| 功能   | 把可能改变进程状态的`wait_address`放入到`p`                  |      |
| 参数   | `filp` `struct file` 指针 `wait_address` `wait_queue_head_t` 指针 `poll_table` 指针 |      |
| 返回值 |                                                              |      |



## 补充内容

### container_of(ptr, type, member)

例

```c
struct test_t{
  int a;
  int b;
  int c;
} test;
```

假如 test.c 地址为 0x11c 计算 test的地址

`(struct test_t *)(0))` 这个结构体中成员`c`地址为 0x08

&test等于(0x11c-0x08);

即

type 为数据类型

ptr  为type中成员的地址

member 为指针指向的成员在该数据类型中的成员名字
$$
type首地址=ptr-(\&(((type *)0)->member))
$$
