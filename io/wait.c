/**
 * @Author: qlc
 * @Date:   2018-10-23T12:02:55+08:00
 * @Email:  qlcx@tom.com
 * @Filename: wait.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:20:16+08:00
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
/*定义字符设备相关结构体*/
typedef struct {
  dev_t chardev_r_num;
  struct cdev chardev_r_dev;
  struct file_operations chardev_r_ops;
  struct class *chardev_r_class;
  struct device *chardev_r_device;
  STACK_R *stacks;
  wait_queue_head_t read, write;
  spinlock_t lock;
#define OPEN 1
#define CLOSE 0
  uint8_t flag;
} CHAR_DEV_R;
CHAR_DEV_R *chardev_r = NULL;
/*----------------------------定义变量----------------------------*/
/*----------------------------结束定义----------------------------*/
/*----------------------------开始添加----------------------------*/

/*定义相应操作函数*/
int chardev_r_open(struct inode *inode, struct file *file) {
  /*保存不会被内核改变的数据*/
  CHAR_DEV_R *c = container_of(inode->i_cdev, CHAR_DEV_R, chardev_r_dev);
  file->private_data = c;
  return 0;
}
int chardev_r_release(struct inode *inode, struct file *file) { return 0; }
ssize_t chardev_r_read(struct file *file, char __user *buffer, size_t size,
                       loff_t *pos) {
  int ret = 0;
  CHAR_DEV_R *c = file->private_data;
  spin_lock(&c->lock);
  if ((file->f_flags & O_NONBLOCK) && (c->stacks->S_D_TOP > 0)) {
    spin_unlock(&c->lock);
    return 0;
  } else {
    if (c->stacks->S_D_TOP == 0) {
      DEFINE_WAIT(__wait);
      do {
        prepare_to_wait(&c->read, &__wait, TASK_INTERRUPTIBLE);
        if (c->stacks->S_D_TOP > 0) {
          break;
        }
        if (signal_pending(current)) {
          ret = -ERESTARTSYS;
          break;
        }
        spin_unlock(&c->lock);
        schedule();
        spin_lock(&c->lock);
      } while (c->stacks->S_D_TOP <= 0);
      finish_wait(&c->read, &__wait);
    }
  }
  if (ret) {
    spin_unlock(&c->lock);
    return ret;
  }
  ret = stack_pop(c->stacks, buffer, size);
  spin_unlock(&c->lock);
  wake_up(&c->write);
  return ret;
}
ssize_t chardev_r_write(struct file *file, const char __user *buffer,
                        size_t size, loff_t *pos) {
  int ret = 0;
  CHAR_DEV_R *c = file->private_data;
  spin_lock(&c->lock);
  if ((file->f_flags & O_NONBLOCK) &&
      c->stacks->S_D_TOP == c->stacks->S_D_SIZE) {
    spin_unlock(&c->lock);
    return 0;
  } else {
    if (c->stacks->S_D_TOP == c->stacks->S_D_SIZE) {
      DEFINE_WAIT(__wait);
      do {
        prepare_to_wait(&c->write, &__wait, TASK_INTERRUPTIBLE);
        if (c->stacks->S_D_TOP < c->stacks->S_D_SIZE)
          break;
        if (signal_pending(current)) {
          ret = -ERESTARTSYS;
          break;
        }
        spin_unlock(&c->lock);
        schedule();
        spin_lock(&c->lock);
      } while (c->stacks->S_D_TOP >= c->stacks->S_D_SIZE);
      finish_wait(&c->write, &__wait);
    }
  }
  if (ret) {
    spin_unlock(&c->lock);
    return ret;
  }
  ret = stack_push(c->stacks, buffer, size);
  spin_unlock(&c->lock);
  wake_up(&c->read);
  return ret;
}
long chardev_r_unlocked_ioctl(struct file *file, unsigned int cmd,
                              unsigned long data) {
  return 0;
}

unsigned int chardev_r_poll(struct file *file,
                            struct poll_table_struct *table) {
  int ret = 0;
  CHAR_DEV_R *c = file->private_data;
  poll_wait(file, &c->read, table);
  poll_wait(file, &c->write, table);
  if (c->stacks->S_D_TOP > 0)
    ret |= POLLIN;
  if ((c->stacks->S_D_TOP) < (c->stacks->S_D_SIZE))
    ret |= POLLOUT;
  return ret;
}

CHAR_DEV_R *chardev_r_create(void) {
  int ret = 0;
  CHAR_DEV_R *cdr;
  cdr = kzalloc(sizeof(*cdr), GFP_KERNEL);
  if (IS_ERR_OR_NULL(cdr))
    goto kzalloc_err;
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
  cdr->chardev_r_ops.poll = chardev_r_poll;

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
                                        cdr->chardev_r_num, NULL, "dev_wait");
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
  kfree(cdr);
kzalloc_err:
  return NULL;
}

void chardev_r_destroy(CHAR_DEV_R *cdr) {
  device_destroy(cdr->chardev_r_class, cdr->chardev_r_num);
  class_destroy(cdr->chardev_r_class);
  cdev_del(&cdr->chardev_r_dev);
  unregister_chrdev_region(cdr->chardev_r_num, 1);
  kfree(cdr);
}

/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  chardev_r = chardev_r_create();
  if (IS_ERR_OR_NULL(chardev_r))
    goto chardev_r_create_err;
  chardev_r->stacks = stack_create(100);
  if (IS_ERR_OR_NULL(chardev_r->stacks))
    goto stack_create_err;
  spin_lock_init(&chardev_r->lock);
  init_waitqueue_head(&chardev_r->read);
  init_waitqueue_head(&chardev_r->write);
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
stack_create_err:
  chardev_r_destroy(chardev_r);
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
