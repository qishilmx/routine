/**
 * @Author: qlc
 * @Date:   2018-10-23T11:08:04+08:00
 * @Email:  qlcx@tom.com
 * @Filename: chardev_lock.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:13:01+08:00
 * @License: GPL
 */
#include "led.h"
#include <linux/init.h>
#include <linux/module.h>
/**
 * @Author: qlc
 * @Date:   2018-10-20T10:54:04+08:00
 * @Email:  qlcx@tom.com
 * @Filename: chardev.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:13:01+08:00
 * @License: GPL
 */
#include "led.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*定义字符设备相关结构体*/
typedef struct {
  void __iomem *led_value;
  dev_t chardev_r_num;
  struct cdev chardev_r_dev;
  struct file_operations chardev_r_ops;
  struct class *chardev_r_class;
  struct device *chardev_r_device;
  spinlock_t lock;
  int flag;
} CHAR_DEV_R;

/*定义相应操作函数*/
int chardev_r_open(struct inode *inode, struct file *file) {
  /*保存不会被内核改变的数据*/
  CHAR_DEV_R *c = container_of(inode->i_cdev, CHAR_DEV_R, chardev_r_dev);
  int ret = 0;

  spin_lock(&c->lock);
  if (c->flag == 1) {
    ret = -EBUSY;
    goto device_busy;
  }
  c->flag = 1;
  spin_unlock(&c->lock);
  file->private_data = c;
  return 0;
device_busy:
  spin_unlock(&c->lock);
  return ret;
}
int chardev_r_release(struct inode *inode, struct file *file) {
  int ret = 0;
  CHAR_DEV_R *c = file->private_data;
  spin_lock(&c->lock);
  if (c->flag == 0) {
    ret = -ENODEV;
    goto device_nodev;
  }
  c->flag = 0;
  spin_unlock(&c->lock);
  return 0;
device_nodev:
  spin_unlock(&c->lock);
  return ret;
}
ssize_t chardev_r_read(struct file *file, char __user *buffer, size_t size,
                       loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  LED_STATUS s;
  if (size != sizeof(s))
    return -EINVAL;

  led_get_status(c->led_value, &s.status);

  if (copy_to_user(buffer, &s, size)) {
    return -EFAULT;
  }

  return size;
}
ssize_t chardev_r_write(struct file *file, const char __user *buffer,
                        size_t size, loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  LED_STATUS s;
  if (size != sizeof(s))
    return -EINVAL;

  if (copy_from_user(&s, buffer, size)) {
    return -EFAULT;
  }

  PERR("con.power=%d\tcon.num=%d\n", s.on_off, s.led_num);
  if (s.on_off) {
    led_on(c->led_value, s.led_num);
  } else {
    led_off(c->led_value, s.led_num);
  }
  return size;
}
long chardev_r_unlocked_ioctl(struct file *file, unsigned int cmd,
                              unsigned long data) {
  return 0;
}

/*定义字符设备结构体指针*/
CHAR_DEV_R *chardev_r = NULL;

CHAR_DEV_R *chardev_r_create(void) {
  int ret = 0;
  CHAR_DEV_R *cdr;
  cdr = kzalloc(sizeof(*cdr), GFP_KERNEL);
  if (IS_ERR_OR_NULL(cdr))
    goto kzalloc_err;
  /*初始化锁*/
  spin_lock_init(&cdr->lock);
  cdr->led_value = led_init(); /*需要修改*/
  if (IS_ERR_OR_NULL(cdr->led_value))
    goto led_init_err;
  /*      申请设备号并注册    */
  ret = alloc_chrdev_region(&cdr->chardev_r_num, 0, 1, "chardev_r");
  if (ret) {
    PERR("alloc chardev_r chrdev region err\n");
    goto alloc_chardev_r_chrdev_region_err;
  }
  /*初始化设备*/
  cdev_init(&cdr->chardev_r_dev, &cdr->chardev_r_ops);

  /*初始化相应操作函数*/
  cdr->chardev_r_ops.open = chardev_r_open;
  cdr->chardev_r_ops.release = chardev_r_release;
  cdr->chardev_r_ops.read = chardev_r_read;
  cdr->chardev_r_ops.write = chardev_r_write;
  cdr->chardev_r_ops.unlocked_ioctl = chardev_r_unlocked_ioctl;

  /*添加设备*/
  ret = cdev_add(&cdr->chardev_r_dev, cdr->chardev_r_num, 1);
  if (ret) {
    PERR("cdev_add_chardev_r_device_err\n");
    goto cdev_add_chardev_r_device_err;
  }

  cdr->chardev_r_class = class_create(THIS_MODULE, "chardev_r");
  if (IS_ERR_OR_NULL(cdr->chardev_r_class)) {
    PERR("chardev_r_class_create_err\n");
    ret = -ENOMEM;
    goto chardev_r_class_create_err;
  }
  cdr->chardev_r_device = device_create(cdr->chardev_r_class, NULL,
                                        cdr->chardev_r_num, NULL, "mydev");
  if (IS_ERR_OR_NULL(cdr->chardev_r_device)) {
    PERR("chardev_r_device_create_err\n");
    ret = -ENOMEM;
    goto chardev_r_device_create_err;
  }

  return cdr;
chardev_r_device_create_err:
  class_destroy(cdr->chardev_r_class);
chardev_r_class_create_err:
  cdev_del(&cdr->chardev_r_dev);
cdev_add_chardev_r_device_err:
  unregister_chrdev_region(cdr->chardev_r_num, 1);
alloc_chardev_r_chrdev_region_err:
  led_exit(cdr->led_value);
led_init_err: /*需要修改*/
  kfree(cdr);
kzalloc_err:
  return NULL;
}

void chardev_r_destroy(CHAR_DEV_R *cdr) {
  device_destroy(cdr->chardev_r_class, cdr->chardev_r_num);
  class_destroy(cdr->chardev_r_class);
  cdev_del(&cdr->chardev_r_dev);
  unregister_chrdev_region(cdr->chardev_r_num, 1);
  led_exit(cdr->led_value); /*需要修改*/
  kfree(cdr);
}

/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  chardev_r = chardev_r_create();
  if (IS_ERR_OR_NULL(chardev_r))
    goto chardev_r_create_err;
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
chardev_r_create_err:
  return -ENOMEM;
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  chardev_r_destroy(chardev_r);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
