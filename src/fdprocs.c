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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>

#include <cabprot.h>

void fdproc_register(int fd, int (*proc)(int i, int v), int val);
void fdproc_deregister(int fd);

//==================================================
// file descriptor based procedures

struct fdproc {
  int fd;
  int active;
  int (*proc)(int fd, int val);
  void *dlhandle;
  int val;
};

#define MAXFDPROC 256

static struct fdproc fdprocs[MAXFDPROC];

static void
init_fdprocs()
{
  int i;
  for (i = 0; i < MAXFDPROC; ++i) {
    fdprocs[i].fd = -1;
  }
}

void
fdproc_register(int fd, int (*proc)(int i, int v), int val)
{
  int i;

  for (i = 0; i < MAXFDPROC; ++i) {
    if (-1 == fdprocs[i].fd) {
      fdprocs[i].fd = fd;
      fdprocs[i].proc = proc;
      fdprocs[i].active = 1;
      fdprocs[i].dlhandle = 0;
      fdprocs[i].val = val;
      return;
    }
  }
  cab_error(1, "increase MAXFDPROC (current = %d)\n", MAXFDPROC);
  exit(1); // DANGEROUS
}

void
fdproc_deregister(int fd)
{
  int i;

  for (i = 0; i < MAXFDPROC; ++i) {
    if (fd == fdprocs[i].fd) {
      fdprocs[i].fd = -1;
      if (fdprocs[i].dlhandle) {
	// what to do?
      }
      fdprocs[i].active = 1;
      return;
    }
  }
  cab_error(1, "increase MAXFDPROC (current = %d)\n", MAXFDPROC);
  exit(1); // DANGEROUS
}


static void
fdproc_setfds(fd_set *fds)
{
  int i;
  for (i = 0; i < MAXFDPROC; ++i) {
    if ((-1 != fdprocs[i].fd) && (1 == fdprocs[i].active)) {
      FD_SET(fdprocs[i].fd, fds);
    }
  }
}

static int maxfd = 0;

void
fdproc_mainloop(void)
{
  fd_set fds;
  int i;
  struct timeval *tvp = 0;

  while (1) {
    FD_ZERO(&fds);
#if 0
    if (g_welcome_new_renderer) {
      FD_SET(bfd, &fds);
    }
#endif
    fdproc_setfds(&fds);

    printf("before select()\n");
    switch (select(maxfd + 1, &fds, NULL, NULL, tvp)) {
    case 0:
      // timeout
      //mctrl_idle();
      break;
    case -1:
      perror("select()");
      exit(1);
      break;
    default:
      for (i = 0; i < MAXFDPROC; i++) {
	if ((fdprocs[i].fd != -1)  && (FD_ISSET(fdprocs[i].fd, &fds))) {
	  fdprocs[i].proc(fdprocs[i].fd, fdprocs[i].val);
	}
      }
      break;
    }
  }
}


static int _fdproc_newclient(int fd, int val);

#if 0
struct serverspec {
  void (*func)(int fd, int val);
  int val;
};

#define SSPECS 256
static struct serverspec {
} specarray[SSPECS];

int specarraynum = 0;
#endif
static int
_fdproc_newclient(int fd, int val)
{
  int newfd, i, retval;
  socklen_t len = sizeof(struct sockaddr);
  struct sockaddr saddr;
  unsigned int type;

  if (-1 == (newfd = accept(fd, &saddr, &len))) {
    cab_perror(0, "accept(): ");
    return;
  }

  if (newfd > maxfd) {
    maxfd = newfd;
  }
  return newfd;
}

int
fdproc_accept(int fd)
{
  int newfd = tcpaccept(fd);
  //if (newfd >= 0) {};
  if (newfd > maxfd) {
    maxfd = newfd;
  }
  return newfd;
}

int
fdproc_server(short port, int val)
{
  int bfd;
  bfd = tcpbind(port);
  fdproc_register(bfd, _fdproc_newclient, val);
}
