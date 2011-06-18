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
// lossy.c
// lossy communication 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <tatemcast.h>

#define MAX_LOSSY_CNCT 25

#include <lossy.h>
#include <ocl.h>
// send master

static char *g_mcdestaddr = "224.0.1.20";

#ifdef OCL_MULTICAST_NETDEV
static char *g_mcdevaddr = OCL_MULTICAST_NETDEV;
#else
static char *g_mcdevaddr = 0;
#endif

void
lc_setDstaddr(char *dst)
{
  g_mcdestaddr = dst;
}

void
lc_setDevaddr(char *dev)
{
  g_mcdevaddr = dev;
}

void *
lcm_init(struct lcm *retval, short port)
{
  if (!retval) {
    if (NULL == (retval = (struct lcm *)malloc(sizeof(struct lcm)))) {
      fprintf(stderr, "ERROR: in lcm_init(), malloc() returns %d (%s)\n", retval, strerror(errno));
      exit(1);
    }
  }
  retval->fd = mcsender_init(g_mcdestaddr, port, 1, g_mcdevaddr);
  retval->port = port;
  mc_dstNew(&(retval->dst), g_mcdestaddr, port);
  return retval;
}

int
lcm_send(struct lcm *lcmptr, char *buf, int size)
{
  int retval = mcwrite(lcmptr->fd, &lcmptr->dst, buf, size);

  if (retval <= 0) {
    fprintf(stderr, "ERROR: lcm_send() returns %d (%s)\n", retval, strerror(errno));
  }
  return retval;
}

// receive client

struct lcc *
lcc_init(struct lcc *lccptr, short port, void *(*f)(void *))
{
  if (!lccptr) {
    if (NULL == (lccptr = (struct lcc *)malloc(sizeof(struct lcc)))) {
      fprintf(stderr, "ERROR: in lcc_init(), malloc() returns %d (%s)\n", lccptr, strerror(errno));
    }
  }
  lccptr->fd = mcrecver_init(g_mcdestaddr, port, g_mcdevaddr);
  pthread_create(&lccptr->thr, 0, f, lccptr);
  return lccptr;
}
/*
void
lc_thread(void *v)
{
  struct lcc *lccptr = (struct lcc *)v;
  while (0 < mcread(lccptr->fd, lccptr->buf, lccptr->bufsize)) {
    
  }
}
*/
