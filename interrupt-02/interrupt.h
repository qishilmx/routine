/**
 * @Author: qlc
 * @Date:   2018-10-23T10:28:14+08:00
 * @Email:  qlcx@tom.com
 * @Filename: interrupt.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T10:32:34+08:00
 * @License: GPL
 */
#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_
#include <linux/interrupt.h>
#define SIZE 4

typedef struct {
  int irq_num[SIZE];
  char *irq_name[SIZE];
  struct tasklet_struct task;
  struct tasklet_struct hi_task;
} IRQ_ST;

#endif
