/**
 * @Author: qlc
 * @Date:   2018-10-23T15:56:33+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T17:09:23+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  int fd = 0;
  char buf[101] = {'\0'};
  if (argc < 3)
    return -1;
  if (strcmp(argv[1], "O_NONBLOCK")) {
    fd = open("/dev/dev_wait", O_RDWR);
  } else {
    fd = open("/dev/dev_wait", O_RDWR | O_NONBLOCK);
  }
  if (fd < 0) {
    perror("open");
    return fd;
  }

  printf("argv[2]=%s\n", argv[2]);
  if (!strcmp(argv[2], "read")) {
    read(fd, buf, 100);
    printf("buf=%s\n", buf);
  } else {
    if (argc < 4) {
      fprintf(stderr, "%s\n", "argument err");
      close(fd);
      return 0;
    }
    write(fd, argv[3], strlen(argv[3]));
  }

  close(fd);
  return 0;
}
