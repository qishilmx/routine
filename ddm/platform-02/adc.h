/**
 * @Author: qlc
 * @Date:   2018-10-25T19:38:35+08:00
 * @Email:  qlcx@tom.com
 * @Filename: adc.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-25T19:45:58+08:00
 * @License: GPL
 */
#ifndef __ADC_H_
#define __ADC_H_

#include <asm/io.h>
#include <linux/proc_fs.h>

void __iomem *adc_init(unsigned int addr, unsigned int size, int flag);
void adc_exit(void __iomem *vir);

void adc_start(void __iomem *vir);

int adc_get_status(void __iomem *vir);
uint16_t adc_get_data(void __iomem *vir);

void adc_interrupt_open(void __iomem *vir);
void adc_interrupt_close(void __iomem *vir);

#endif
