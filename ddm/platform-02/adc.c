/**
 * @Author: qlc
 * @Date:   2018-10-25T19:38:28+08:00
 * @Email:  qlcx@tom.com
 * @Filename: adc.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-25T20:05:05+08:00
 * @License: GPL
 */
#include "adc.h"

#define ADCCON 0x0        //配置寄存器
#define ADCDAT 0x4        //数据寄存器
#define ADCINTENB 0x8     //中断使能寄存器
#define ADCINTCLR 0xc     //中断清除寄存器
#define PRESCALERCON 0x10 //预分频寄存器

void __iomem *adc_init(unsigned int addr, unsigned int size, int flag) {
  void __iomem *v = ioremap(addr, size);
  if (IS_ERR_OR_NULL(v))
    return NULL;
  if (flag) {
    iowrite32(1, v + ADCINTENB);
  }
  iowrite32(8 << 6, v + ADCCON);
  iowrite32(((1 << 15) | 99), v + PRESCALERCON);
  return v;
}
void adc_exit(void __iomem *vir) {
  iowrite32(0, vir + ADCINTENB);
  iowrite32(1, vir + ADCINTCLR);
  iowrite32(0, vir + PRESCALERCON);
  iounmap(vir);
}

void adc_start(void __iomem *vir) {
  unsigned reg = 0;
  reg = ioread32(vir + ADCCON);
  reg &= ~(1 << 2);
  reg |= 1;
  iowrite32(reg, vir + ADCCON);
}

int adc_get_status(void __iomem *vir) {
  unsigned reg = 0;
  reg = ioread32(vir + ADCCON);
  return (reg & 1);
}
uint16_t adc_get_data(void __iomem *vir) {
  unsigned reg = 0;
  reg = ioread32(vir + ADCDAT);
  return (reg & 0xfff);
}

void adc_interrupt_open(void __iomem *vir) { iowrite32(1, vir + ADCINTENB); }
void adc_interrupt_close(void __iomem *vir) { iowrite32(1, vir + ADCINTCLR); }
