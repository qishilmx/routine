/**
 * @Author: fjk
 * @Date:   2018-10-18T14:18:04+08:00
 * @Email:  sunnyfjk@gmai.com
 * @Filename: main.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T11:51:50+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct user_led_con_t {
  uint8_t power;
  uint8_t num;
  uint8_t status;
};

int main(void) {
  int fd = 0;
  int ret = 0;
  int i = 0;
  struct user_led_con_t con = {0};
  fd = open("/dev/mydev", O_RDWR);
  if (fd < 0) {
    ret = fd;
    perror("open");
    goto open_myled_err;
  }
  for (i = 0; i < 15; i++) {
    if ((i % 4) == 0)
      con.power = !con.power;
    con.num = i % 4;
    ret = write(fd, &con, sizeof(con));
    if (ret < 0) {
      perror("write");
    }
    sleep(1);
  }

  ret = read(fd, &con, sizeof(con));
  if (ret < 0) {
    perror("read");
  }

  printf("%#x\n", con.status);
  close(fd);
  return 0;
open_myled_err:
  return ret;
}
