/**
 * @Author: qlc
 * @Date:   2018-10-22T17:43:58+08:00
 * @Email:  qlcx@tom.com
 * @Filename: interrupt.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T10:48:23+08:00
 * @License: GPL
 */
#include "interrupt.h"
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <mach/platform.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/

IRQ_ST irq_key;

irqreturn_t key_irq_handler(int irq, void *dev) {

  IRQ_ST *d = dev;
  PERR("irq=%d\n", irq);
  tasklet_schedule(&d->task);
  tasklet_hi_schedule(&d->hi_task);

  if (in_interrupt()) {
    PERR("in_interrupt\n");
  } else {
    PERR("no_interrupt\n");
  }
  /*执行成功*/
  return IRQ_HANDLED;
  /*
   执行失败返回
   return IRQ_NONE
   */
}

void key_task_func(unsigned long data) {

  if (in_interrupt()) {
    PERR("in_interrupt\n");
  } else {
    PERR("no_interrupt\n");
  }
  PERR("data=%p\n", (void *)data);
}

void key_hi_task_func(unsigned long data) {

  if (in_interrupt()) {
    PERR("in_interrupt\n");
  } else {
    PERR("no_interrupt\n");
  }
  PERR("data=%p\n", (void *)data);
}

/*----------------------------结束添加----------------------------*/
static __init int interrupt_init(void) {
  /*开始添加*/
  int i = 0, ret = 0;

  irq_key.irq_num[0] = gpio_to_irq(PAD_GPIO_A + 28);
  irq_key.irq_name[0] = "key_1";
  irq_key.irq_num[1] = gpio_to_irq(PAD_GPIO_B + 30);
  irq_key.irq_name[1] = "key_2";
  irq_key.irq_num[2] = IRQ_GPIO_B_START + 31;
  irq_key.irq_name[2] = "key_3";
  irq_key.irq_num[3] = IRQ_GPIO_B_START + 9;
  irq_key.irq_name[3] = "key_4";

  tasklet_init(&irq_key.task, key_task_func, (unsigned long)&irq_key);
  tasklet_init(&irq_key.hi_task, key_hi_task_func, (unsigned long)&irq_key);

  for (i = 0; i < SIZE; i++) {
    ret = request_irq(irq_key.irq_num[i], key_irq_handler,
                      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                      irq_key.irq_name[i], &irq_key);
    if (ret)
      goto request_irq_err;
  }

  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
request_irq_err:
  while (i--) {
    free_irq(irq_key.irq_num[i], &irq_key);
  }
  return ret;
  /*结束添加*/
}

static __exit void interrupt_exit(void) {
  /*开始添加*/
  int i = SIZE;
  while (i--) {
    free_irq(irq_key.irq_num[i], &irq_key);
  }
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(interrupt_init);
module_exit(interrupt_exit);
