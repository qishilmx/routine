/**
 * @Author: qlc
 * @Date:   2018-10-18T14:47:55+08:00
 * @Email:  qlcx@tom.com
 * @Filename: char_led.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-18T15:52:17+08:00
 * @License: GPL
 */
/*
GPIO_B26
GPIO_C11
GPIO_C7
GPIO_C12
 */

#ifndef __X6818_LED_H_
#define __X6818_LED_H_

#define LED_GPIO_BASE_ADDR 0xC0010000

#define GPIOOUT_B 0xB000
#define GPIOOUTENB_B 0xB004
#define GPIOPULLENB_B 0xB060
#define GPIOPULLENB_DISABLE_DEFAULT_B 0xB064

#define GPIOOUT_C 0xC000
#define GPIOOUTENB_C 0xC004
#define GPIOPULLENB_C 0xC060
#define GPIOPULLENB_DISABLE_DEFAULT_C 0xC064

struct char_led_t {
  void __iomem *v_led;
  struct cdev cdev_led;
  struct file_operations ops_led;
  dev_t num_led;
  struct class *class_led;
  struct device *device_led;
};

#endif
