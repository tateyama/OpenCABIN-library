/* OpenCABIN Library

   Copyright (C) 2006,2007 KGT Inc.

    This file is part of the OpenCABIN Library developed
    under CnC project sponsored by National Institute of
    Information and Communications Technology (NICT).

    The OpenCABIN Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    The OpenCABIN Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define LISTENQ 6

int
tcpbind(int port)
{
  int fd, on = 1;
  struct sockaddr_in addr;

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (0 > (fd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("tcpbind(): ");
    return -1;
  }

  if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
    perror("Warning: cannot setsockopt(SO_REUSEADDR)");
  }

  if (-1 == bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
    perror("tcpbind(): ");
    exit(1);
  }

  if (-1 == listen(fd, LISTENQ)) {
    perror("listen()");
    exit(1);
  }

  return fd;
}

int
tcpaccept(int fd)
{
  struct sockaddr saddr;
  socklen_t len = sizeof(struct sockaddr);
  return accept(fd, &saddr, &len);
}

int
udpbind(int port)
{
}
