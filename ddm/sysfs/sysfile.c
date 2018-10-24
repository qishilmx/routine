/**
 * @Author: qlc
 * @Date:   2018-10-24T14:16:05+08:00
 * @Email:  qlcx@tom.com
 * @Filename: sysfile.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T14:56:51+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始定义----------------------------*/
struct kset *kset;
struct kobject led1, led2;
/*----------------------------结束定义----------------------------*/
/*----------------------------开始添加----------------------------*/
ssize_t led_show(struct kobject *k, struct attribute *a, char *c) {
  return sprintf(c, "%s--%s\n", k->name, a->name);
}
ssize_t led_store(struct kobject *k, struct attribute *a, char const *c,
                  size_t s) {
  PERR("%s--%s\n", k->name, a->name);
  return s;
}

void led_release(struct kobject *k) { ; }
struct sysfs_ops led_sysfs_ops = {
    .show = led_show, .store = led_store,
};
struct attribute led_on = {
    .name = "on", .mode = 0644,
};
struct attribute led_off = {
    .name = "off", .mode = 0644,
};
struct attribute *led_att[] = {
    &led_on, &led_off, NULL,
};
struct kobj_type led = {
    .release = led_release,
    .sysfs_ops = &led_sysfs_ops,
    .default_attrs = led_att,
};
/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  int ret = 0;
  kset = kset_create_and_add("led", NULL, NULL);
  ret = kobject_init_and_add(&led1, &led, &kset->kobj, "led1");
  ret = kobject_init_and_add(&led2, &led, &kset->kobj, "led2");
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
  /*继续添加*/
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  kobject_del(&led1);
  kobject_del(&led2);
  kset_unregister(kset);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
