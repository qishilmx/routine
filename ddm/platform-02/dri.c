/**
 * @Author: qlc
 * @Date:   2018-10-24T14:59:19+08:00
 * @Email:  qlcx@tom.com
 * @Filename: dri.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-25T20:17:58+08:00
 * @License: GPL
 */
#include "adc.h"
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/sched.h>
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
  int irq;
  uint16_t data;
  wait_queue_head_t read;
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
  if (size != sizeof(uint16_t))
    return -EINVAL;
  if (c->data == 0 && (file->f_flags & O_NONBLOCK))
    return -EAGAIN;
  adc_start(c->vir);
  wait_event_interruptible(c->read, (c->data >= 0));
  if (copy_to_user(buffer, &c->data, size))
    return -EFAULT;
  c->data = -1;
  return size;
}
ssize_t dev_write(struct file *file, const char __user *buffer, size_t size,
                  loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  return size;
}
long dev_unlocked_ioctl(struct file *file, unsigned int cmd,
                        unsigned long data) {
  return 0;
}

unsigned int dev_poll(struct file *file, struct poll_table_struct *tab) {
  CHAR_DEV_R *c = file->private_data;
  unsigned int mode = 0;
  poll_wait(file, &c->read, tab);
  if (c->data)
    mode |= POLLIN;
  return mode;
}

irqreturn_t adc_irq_handler(int irq, void *data) {

  CHAR_DEV_R *c = data;
  adc_interrupt_close(c->vir);
  c->data = adc_get_data(c->vir);
  wake_up(&c->read);
  return IRQ_HANDLED;
}

int adc_probe(struct platform_device *device) {
  CHAR_DEV_R *adc = NULL;
  struct resource *adc_dev_res = NULL;
  adc_dev_res = platform_get_resource(device, IORESOURCE_MEM, 0);
  if (IS_ERR_OR_NULL(adc_dev_res))
    return -EBUSY;
  adc = kmalloc(sizeof(*adc), GFP_KERNEL);
  if (IS_ERR_OR_NULL(adc))
    goto kmalloc_err;

  init_waitqueue_head(&adc->read);
  adc->vir = adc_init(adc_dev_res->start,
                      adc_dev_res->end - adc_dev_res->start + 1, 0);
  if (IS_ERR_OR_NULL(adc->vir))
    goto adc_init_err;

  if (request_irq(adc_dev_res->start, adc_irq_handler, 0, "adc_irq", adc))
    goto request_irq_err;
  adc->irq = adc_dev_res->start;

  if (alloc_chrdev_region(&adc->num, 0, 1, "adc_dev"))
    goto alloc_chrdev_region;
  /*初始化函数造作集*/
  adc->ops.open = dev_open;
  adc->ops.release = dev_release;
  adc->ops.read = dev_read;
  adc->ops.write = dev_write;
  adc->ops.unlocked_ioctl = dev_unlocked_ioctl;
  adc->ops.poll = dev_poll;

  cdev_init(&adc->cdev, &adc->ops);
  if (cdev_add(&adc->cdev, adc->num, 1))
    goto cdev_add_err;
  adc->class = class_create(THIS_MODULE, "adc_dev");
  if (IS_ERR_OR_NULL(adc->class))
    goto class_create_err;
  adc->device = device_create(adc->class, NULL, adc->num, NULL, device->name);
  if (IS_ERR_OR_NULL(adc->device))
    goto device_create_err;
  platform_set_drvdata(device, adc);
  return 0;
device_create_err:
  class_destroy(adc->class);
class_create_err:
  cdev_del(&adc->cdev);
cdev_add_err:
  unregister_chrdev_region(adc->num, 1);
alloc_chrdev_region:
  free_irq(adc_dev_res->start, adc);
request_irq_err:
  adc_exit(adc->vir);
adc_init_err:
  kfree(adc);
kmalloc_err:
  return -ENOMEM;
}

int adc_remove(struct platform_device *device) {
  CHAR_DEV_R *adc = platform_get_drvdata(device);
  device_destroy(adc->class, adc->num);
  class_destroy(adc->class);
  cdev_del(&adc->cdev);
  unregister_chrdev_region(adc->num, 1);
  adc_exit(adc->vir);
  kfree(adc);
  return 0;
}

struct platform_device_id adc_id_table[] = {
    {"adc_dev", 0}, {"adc_dev_1", 1}, {},
};

struct platform_driver adc_dev = {
    .probe = adc_probe,
    .remove = adc_remove,
    .driver =
        {
            .name = "adc_dev",
        },
    .id_table = adc_id_table,
};

/*----------------------------结束添加----------------------------*/
static __init int dri_init(void) {
  /*开始添加*/
  /*暂停添加*/
  PERR("INIT\n");
  return platform_driver_register(&adc_dev);
  /*继续添加*/
  /*结束添加*/
}

static __exit void dri_exit(void) {
  /*开始添加*/
  platform_driver_unregister(&adc_dev);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(dri_init);
module_exit(dri_exit);
