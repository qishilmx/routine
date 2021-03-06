/**
 * @Author: qlc
 * @Date:   2018-10-23T12:02:55+08:00
 * @Email:  qlcx@tom.com
 * @Filename: wait.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T17:06:52+08:00
 * @License: GPL
 */
#include "stack_r.h"
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始定义----------------------------*/
/*定义设备结构体*/
typedef struct {
  struct cdev dev;
  dev_t dev_num;

  struct file_operations dev_ops;

  struct class *class;
  struct device *device;

  wait_queue_head_t read, write;
  spinlock_t lock;
  uint8_t flag;
  STACK_R *stacks;
#define START 1
#define END 0
} DEV_ST;
/*----------------------------定义变量----------------------------*/
DEV_ST *dev;
/*----------------------------结束定义----------------------------*/
/*----------------------------开始添加----------------------------*/

int wait_w_open(struct inode *inode, struct file *file) {
  DEV_ST *save = container_of(inode->i_cdev, DEV_ST, dev);
  file->private_data = save;
  return 0;
}
int wait_w_release(struct inode *inode, struct file *file) { return 0; }

ssize_t wait_w_read(struct file *file, char __user *ubuf, size_t size,
                    loff_t *off) {
  DEV_ST *save = file->private_data;
  int ret = 0;
  spin_lock(&save->lock);
  if (file->f_flags & O_NONBLOCK) { //非阻塞读
    if (save->stacks->S_D_TOP == 0) {
      spin_unlock(&save->lock);
      return 0;
    }
  } else { //阻塞读
    if (save->stacks->S_D_TOP == 0) {
      DEFINE_WAIT(__wait);
      do {
        prepare_to_wait(&save->read, &__wait, TASK_INTERRUPTIBLE);
        if (save->stacks->S_D_TOP > 0) {
          break;
        }
        if (signal_pending(current)) {
          ret = -ERESTARTSYS;
          break;
        }
        spin_unlock(&save->lock);
        schedule();
        spin_lock(&save->lock);
      } while (save->stacks->S_D_TOP <= 0);
      finish_wait(&save->read, &__wait);
    }
    if (ret) {
      spin_unlock(&save->lock);
      return ret;
    }
    ret = stack_pop(save->stacks, ubuf, size);
    spin_unlock(&save->lock);
    wake_up(&save->write);
    return ret;
  }

  ret = stack_pop(save->stacks, ubuf, size);
  wake_up(&save->write);
  return ret;
}
ssize_t wait_w_write(struct file *file, const char __user *ubuf, size_t size,
                     loff_t *off) {
  DEV_ST *save = file->private_data;
  int ret = 0;
  spin_lock(&save->lock);
  if (file->f_flags & O_NONBLOCK) {
    if (save->stacks->S_D_TOP == save->stacks->S_D_SIZE) {
      spin_unlock(&save->lock);
      return 0;
    }
  } else {
    if (save->stacks->S_D_TOP == save->stacks->S_D_SIZE) {
      DEFINE_WAIT(__wait);
      do { /*prepare_to_wait 使用了之后一定要使用 finish_wait 函数*/
        prepare_to_wait(&save->write, &__wait, TASK_INTERRUPTIBLE);
        if (save->stacks->S_D_TOP < save->stacks->S_D_SIZE)
          break;
        if (signal_pending(current)) {
          ret = -ERESTARTSYS;
          break;
        }
        spin_unlock(&save->lock);
        schedule();
        spin_lock(&save->lock);
      } while (save->stacks->S_D_TOP >= save->stacks->S_D_SIZE);
      finish_wait(&save->write, &__wait);
    }
  }
  if (ret) {
    spin_unlock(&save->lock);
    return ret;
  }
  ret = stack_push(save->stacks, ubuf, size);
  wake_up(&save->read);
  return ret;
}

long wait_w_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
  return 0;
}

unsigned int poll_w_wait(struct file *file, struct poll_table_struct *table) {
  int ret = 0;
  DEV_ST *save = file->private_data;
  poll_wait(file, &save->read, table);
  poll_wait(file, &save->write, table);
  if (save->stacks->S_D_TOP > 0)
    ret |= POLLIN;
  if (save->stacks->S_D_TOP < save->stacks->S_D_SIZE)
    ret |= POLLOUT;
  return ret;
}

/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  dev = kmalloc(sizeof(*dev), GFP_KERNEL);
  if (IS_ERR_OR_NULL(dev))
    goto dev_kmalloc_err;
  spin_lock_init(&dev->lock);
  init_waitqueue_head(&dev->read);
  init_waitqueue_head(&dev->write);
  dev->flag = END;
  dev->stacks = stack_create(100);
  if (IS_ERR_OR_NULL(dev->stacks))
    goto stack_create_err;
  if (alloc_chrdev_region(&dev->dev_num, 0, 1, "dev_wait"))
    goto alloc_chrdev_region_err;
  dev->dev_ops.open = wait_w_open;
  dev->dev_ops.release = wait_w_release;
  dev->dev_ops.read = wait_w_read;
  dev->dev_ops.write = wait_w_write;
  dev->dev_ops.unlocked_ioctl = wait_w_ioctl;
  dev->dev_ops.poll = poll_w_wait;

  cdev_init(&dev->dev, &dev->dev_ops);
  if (cdev_add(&dev->dev, dev->dev_num, 1))
    goto cdev_add_err;
  dev->class = class_create(THIS_MODULE, "dev_wait");
  if (IS_ERR_OR_NULL(dev->class))
    goto class_create_err;
  dev->device = device_create(dev->class, NULL, dev->dev_num, NULL, "dev_wait");
  if (IS_ERR_OR_NULL(dev->device))
    goto device_create_err;

  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
device_create_err:
  class_destroy(dev->class);
class_create_err:
  cdev_del(&dev->dev);
cdev_add_err:
  unregister_chrdev_region(dev->dev_num, 1);
alloc_chrdev_region_err:
  stack_destroy(dev->stacks);
stack_create_err:
  kfree(dev);
dev_kmalloc_err:
  return -ENOMEM;
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  device_destroy(dev->class, dev->dev_num);
  class_destroy(dev->class);
  cdev_del(&dev->dev);
  unregister_chrdev_region(dev->dev_num, 1);
  stack_destroy(dev->stacks);
  kfree(dev);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
