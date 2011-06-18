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
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int
tcpconnect(char *host0)
{
  int fd, port;
  char *host = strdup(host0), *pstr;
  struct in_addr inaddr;
  struct hostent *hentptr;
  struct sockaddr_in addr;

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  if (NULL == (pstr = strchr(host, ':'))) {
    port = 80;
  } else {
    *pstr = 0;
    port = atoi(pstr + 1);
  }
  addr.sin_port = htons(port);

  if (1 == inet_aton(host, &inaddr)) {
    addr.sin_addr = inaddr;
    //memcpy(&addr.sin_addr, &inaddr, sizeof(struct in_addr));
  } else {
    if (NULL == (hentptr = gethostbyname(host))) {
      fprintf(stderr, "hostname \"%s\" cannot be resolved\n", host);
      return -1;
    }
    memcpy(&addr.sin_addr, hentptr->h_addr, hentptr->h_length);
  }

  if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "cannot create socket\n");
    return -1;
  }

  if (-1 == connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
    fprintf(stderr, "cannot connect to %s:%d\n", host, port);
    return -1;
  }
  free(host);
  return fd;
}
#if 0
int
main(int ac, char **av)
{
  int fd, retval;
  char buf[BUFSIZ];
  if (-1 == (fd = tcpconnect("yt:28980"))) {
    exit(1);
  }
  //fd = tcpconnect("133.11.54.47:28980");
  printf("connected\n");
  while (0 < (retval = read(fd, buf, BUFSIZ))) {
    buf[retval] = 0;
    printf("\"%s\"\n", buf);
  }
  return 0;
}
#endif
