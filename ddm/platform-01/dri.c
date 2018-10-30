/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:19+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dri.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-25T19:35:54+08:00
 * @License: GPL
 */
#include "led.h"
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*定义字符设备相关结构体*/
typedef struct {
  void __iomem *vir;
  dev_t num;
  struct cdev cdev;
  struct file_operations ops;
  struct class *class;
  struct device *device;
} CHAR_DEV_R;

int dev_open(struct inode *inode, struct file *file) {
  CHAR_DEV_R *c = container_of(inode->i_cdev, CHAR_DEV_R, cdev);
  file->private_data = c;
  return 0;
}
int dev_release(struct inode *inode, struct file *file) { return 0; }

ssize_t dev_read(struct file *file, char __user *buffer, size_t size,
                 loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  LED_STATUS con;
  if (size != sizeof(con))
    return -EINVAL;
  led_get_status(c->vir, &con.status);
  if (copy_to_user(buffer, &con, size))
    return -EINVAL;
  return size;
}
ssize_t dev_write(struct file *file, const char __user *buffer, size_t size,
                  loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  LED_STATUS con;
  if (size != sizeof(con))
    return -EINVAL;
  if (copy_from_user(&con, buffer, size))
    return -EINVAL;
  if (con.on_off) {
    led_on(c->vir, con.led_num);
  } else {
    led_off(c->vir, con.led_num);
  }
  return size;
}
long dev_unlocked_ioctl(struct file *file, unsigned int cmd,
                        unsigned long data) {
  return 0;
}

int led_probe(struct platform_device *device) {
  CHAR_DEV_R *led = NULL;
  struct resource *led_dev_res = NULL;
  led_dev_res = platform_get_resource(device, IORESOURCE_MEM, 0);
  if (IS_ERR_OR_NULL(led_dev_res))
    return -EBUSY;
  led = kmalloc(sizeof(*led), GFP_KERNEL);
  if (IS_ERR_OR_NULL(led))
    goto kmalloc_err;
  led->vir =
      led_init(led_dev_res->start, led_dev_res->end - led_dev_res->start + 1);
  if (IS_ERR_OR_NULL(led->vir))
    goto led_init_err;
  if (alloc_chrdev_region(&led->num, 0, 1, "led_dev"))
    goto alloc_chrdev_region;
  /*初始化函数造作集*/
  led->ops.open = dev_open;
  led->ops.release = dev_release;
  led->ops.read = dev_read;
  led->ops.write = dev_write;
  led->ops.unlocked_ioctl = dev_unlocked_ioctl;

  cdev_init(&led->cdev, &led->ops);
  if (cdev_add(&led->cdev, led->num, 1))
    goto cdev_add_err;
  led->class = class_create(THIS_MODULE, "led_dev");
  if (IS_ERR_OR_NULL(led->class))
    goto class_create_err;
  led->device = device_create(led->class, NULL, led->num, NULL, device->name);
  if (IS_ERR_OR_NULL(led->device))
    goto device_create_err;
  platform_set_drvdata(device, led);
  return 0;
device_create_err:
  class_destroy(led->class);
class_create_err:
  cdev_del(&led->cdev);
cdev_add_err:
  unregister_chrdev_region(led->num, 1);
alloc_chrdev_region:
  led_exit(led->vir);
led_init_err:
  kfree(led);
kmalloc_err:
  return -ENOMEM;
}

int led_remove(struct platform_device *device) {
  CHAR_DEV_R *led = platform_get_drvdata(device);
  device_destroy(led->class, led->num);
  class_destroy(led->class);
  cdev_del(&led->cdev);
  unregister_chrdev_region(led->num, 1);
  led_exit(led->vir);
  kfree(led);
  return 0;
}

struct platform_device_id led_id_table[] = {
    {"led_dev", 0}, {"led_dev_1", 1}, {},
};

struct platform_driver led_dev = {
    .probe = led_probe,
    .remove = led_remove,
    .driver =
        {
            .name = "led_dev",
        },
    .id_table = led_id_table,
};

/*----------------------------结束添加----------------------------*/
static __init int dri_init(void) {
  /*开始添加*/
  /*暂停添加*/
  PERR("INIT\n");
  return platform_driver_register(&led_dev);
  /*继续添加*/
  /*结束添加*/
}

static __exit void dri_exit(void) {
  /*开始添加*/
  platform_driver_unregister(&led_dev);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(dri_init);
module_exit(dri_exit);