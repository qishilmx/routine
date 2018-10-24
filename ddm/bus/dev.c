/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:07+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dev.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T16:13:39+08:00
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
extern struct bus_type bus;
void dev_release(struct device *dev) {}
/*----------------------------结束定义----------------------------*/
/*----------------------------开始添加----------------------------*/
struct device dev = {
    .init_name = "dev_led",
    .release = dev_release,
    .bus = &bus,
    .platform_data = "dev_led",
};
/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  /*暂停添加*/
  PERR("INIT\n");
  return device_register(&dev);
  ;
  /*继续添加*/
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  device_unregister(&dev);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
