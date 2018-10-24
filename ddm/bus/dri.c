/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:19+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dri.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:54:48+08:00
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
/*----------------------------开始添加----------------------------*/
extern struct bus_type bus;

int dri_probe(struct device *d) {
  PERR("dev->platform_data=%s\n", (char *)(d->platform_data));
  return 0;
}

int dri_remove(struct device *d) {
  PERR("dev->platform_data=%s\n", (char *)(d->platform_data));
  return 0;
}

struct device_driver dri = {
    .name = "led", .bus = &bus, .probe = dri_probe, .remove = dri_remove,
};
/*----------------------------结束添加----------------------------*/
static __init int dri_init(void) {
  /*开始添加*/
  driver_register(&dri);
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
  /*继续添加*/
  /*结束添加*/
}

static __exit void dri_exit(void) {
  /*开始添加*/
  driver_unregister(&dri);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(dri_init);
module_exit(dri_exit);
