/**
 * @Author: qlc
 * @Date:   2018-10-19T11:35:08+08:00
 * @Email:  qlcx@tom.com
 * @Filename: chardev_r.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T14:43:49+08:00
 * @License: GPL
 */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*定义字符设备相关结构体*/
typedef struct {
  dev_t chardev_r_num;
  struct cdev chardev_r_dev;
  struct file_operations chardev_r_ops;
  struct class *chardev_r_class;
  struct device *chardev_r_device;
} CHAR_DEV_R;

/*定义ioctl相关结构体*/
typedef struct {
  unsigned int set_size;
  int set_clear;
} CHAR_DEV_SET;
typedef struct {
  int show_num;
  int show_size;
} CHAR_DEV_GET;
#define SET_VALUE _IOW('L', 1, CHAR_DEV_SET *)
#define GET_VALUE _IOR('L', 2, CHAR_DEV_GET *)
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
  return 0;
}
ssize_t chardev_r_write(struct file *file, const char __user *buffer,
                        size_t size, loff_t *pos) {
  return 0;
}
long chardev_r_unlocked_ioctl(struct file *file, unsigned int cmd,
                              unsigned long data) {
  int ret = 0;
  // CHAR_DEV_R *c = file->private_data; /*暂时未使用*/
  CHAR_DEV_SET value_S;
  CHAR_DEV_GET value_G;

  switch (cmd) {
  case SET_VALUE:
    ret = copy_from_user(&value_S, (void *)data, sizeof(value_S));
    PERR("value_S.set_size=%d,value_S.set_clear=%d\n", value_S.set_size,
         value_S.set_clear);
    break;
  case GET_VALUE:
    value_G.show_num = 100;
    value_G.show_size = 512;
    ret = copy_to_user((void *)data, &value_G, sizeof(value_G));
    PERR("value_G.show_num=%d,value_G.show_size=%d\n", value_G.show_num,
         value_G.show_size);
    break;
  }

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
                                        cdr->chardev_r_num, NULL, "chardev_r");
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
static __init int chardev_r_init(void) {
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

static __exit void chardev_r_exit(void) {
  /*开始添加*/
  chardev_r_destroy(chardev_r);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(chardev_r_init);
module_exit(chardev_r_exit);
