/**
 * @Author: qlc
 * @Date:   2018-10-23T15:56:33+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-23T17:16:56+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

int main(int argc, char const *argv[]) {
  int fd = 0, efd = 0, ret = 0;
  struct epoll_event e = {'\0'};

  if (argc < 2)
    return -1;

  fd = open("/dev/dev_wait", O_WRONLY);
  if (fd < 0) {
    return fd;
  }
  efd = epoll_create(1);
  if (efd < 0) {
    close(fd);
    return -1;
  }
  e.events |= EPOLLOUT;
  e.data.fd = fd;
  ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &e);
  if (ret < 0) {
    close(efd);
    close(fd);
    return -1;
  }
  while ((ret = epoll_wait(efd, &e, 1, 100)) >= 0) {
    if (ret == 0) {
      continue;
    }
    if (ret == 1) {
      write(e.data.fd, argv[1], strlen(argv[1]) - 1);
    }
  }
  close(efd);
  close(fd);
  return 0;
}
