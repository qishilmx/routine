/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:07+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dev.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T20:18:19+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*

struct platform_device {
        const char      * name;
        int             id;
        struct device   dev;
        u32             num_resources;
        struct resource * resource;

        const struct platform_device_id *id_entry;

        struct mfd_cell *mfd_cell;

        struct pdev_archdata    archdata;
};
 */

struct resource led_resource[] = {
        [0]{
            .start = 0xC0010000,
            .end = 0xC0010000 + SZ_64K - 1,
            .name = "led",
            .flags = IORESOURCE_MEM,
        },
};

void led_release(struct device *d) {}

struct platform_device led_dev = {
    .name = "led",
    .id = -1,
    .dev =
        {
            .init_name = "led", .release = led_release,
        },
    .num_resources = ARRAY_SIZE(led_resource),
    .resource = led_resource,
};

/*----------------------------结束添加----------------------------*/
static __init int dev_init(void) {
  /*开始添加*/
  platform_device_register(&led_dev);
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
  /*继续添加*/
  /*结束添加*/
}

static __exit void dev_exit(void) {
  /*开始添加*/
  platform_device_unregister(&led_dev);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(dev_init);
module_exit(dev_exit);
