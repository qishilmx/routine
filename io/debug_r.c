/**
 * @Author: qlc
 * @Date:   2018-10-23T15:56:33+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-24T19:21:50+08:00
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

char buf[100] = {0};
int main(int argc, char const *argv[]) {

  int fd = 0, efd = 0, ret = 0;
  struct epoll_event event = {0};
  fd = open("/dev/dev_wait", O_RDONLY);
  if (fd < 0) {
    perror("open");
    return fd;
  }
  efd = epoll_create(1);
  if (efd < 0) {
    perror("epoll_create");
    goto epoll_create_err;
  }
  event.events |= EPOLLIN;
  event.data.fd = fd;
  ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
  if (ret < 0) {
    perror("epoll_ctl EPOLL_CTL_ADD");
    goto EPOLL_CTL_ADD_err;
  }
  while ((ret = epoll_wait(efd, &event, 1, 100)) >= 0) {
    if (ret == 0)
      continue;
    if (ret == 1) {
      read(event.data.fd, buf, 100);
      printf("buf=%s\n", buf);
    }
  }
  close(efd);
  close(fd);
  return 0;
EPOLL_CTL_ADD_err:
  close(efd);
epoll_create_err:
  close(fd);
  return 0;
}
