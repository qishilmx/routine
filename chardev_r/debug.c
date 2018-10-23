/**
 * @Author: qlc
 * @Date:   2018-10-19T13:31:55+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T22:45:28+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void) {
  CHAR_DEV_SET value_S = {0};
  CHAR_DEV_GET value_G = {0};
  int fd = 0;
  int ret = 0;
  fd = open("/dev/chardev_r", O_RDWR);
  if (fd < 0) {
    return fd;
  }
  value_S.set_size = 1;
  value_S.set_clear = 10;
  ret = ioctl(fd, SET_VALUE, &value_S);
  printf("ret=%d\n", ret);

  ret = ioctl(fd, GET_VALUE, &value_G);
  printf("%d:%d\n", value_G.show_num, value_G.show_size);
  printf("ret=%d\n", ret);

  close(fd);
  return 0;
}
