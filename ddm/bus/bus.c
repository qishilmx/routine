/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:27+08:00
 * @Email:  qlcx@tom.com
 * @Filename: bus.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:47:35+08:00
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
int bus_match(struct device *e, struct device_driver *r) {
  if (strcmp(e->platform_data, r->name))
    return 0;
  return 1;
}

struct bus_type bus = {
    .name = "bus", .match = bus_match,
};
EXPORT_SYMBOL(bus);
/*----------------------------结束添加----------------------------*/
static __init int bus_init(void) {
  /*开始添加*/
  bus_register(&bus);
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
  /*继续添加*/
  /*结束添加*/
}

static __exit void bus_exit(void) {
  /*开始添加*/
  bus_unregister(&bus);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(bus_init);
module_exit(bus_exit);
