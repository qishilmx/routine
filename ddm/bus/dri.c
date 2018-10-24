/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:19+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dri.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T17:13:16+08:00
 * @License: GPL
 */
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始定义----------------------------*/
/*----------------------------结束定义----------------------------*/
/*----------------------------开始添加----------------------------*/
int mydevice_driverprobe(struct device *dev) {
  PERR("dev->platform_data=%s\n", (char *)(dev->platform_data));
  return 0;
}
int mydevice_driveremove(struct device *dev) {
  PERR("dev->platform_data=%s\n", (char *)(dev->platform_data));
  return 0;
}
extern struct bus_type bus;
struct device_driver dri = {
    .name = "dri_led",
    .bus = &bus,
    .probe = mydevice_driverprobe,
    .remove = mydevice_driveremove,
};
/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  /*暂停添加*/
  PERR("INIT\n");
  return driver_register(&dri);
  /*继续添加*/
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  driver_unregister(&dri);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
