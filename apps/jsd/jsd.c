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
#include <ocl.h>
#include <string.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <stdlib.h>
//#include "jsd.h"

static int fd;

struct data {
  int fd;
  struct ocl_jsstat stat;
};

void *
minit(int ac, char **av)
{
  char *devfn = "/dev/input/js0";
  //char *data = "test";
  struct data *data;

  printf("ac: %d\n", ac);
  if (ac > 1) {
    printf("av[0] = %s\n", av[0]);
  }

  if (-1 == (fd = open(devfn, O_RDONLY))) {
    perror(devfn);
    exit(1);
  }

  //printf("nind:minit()done\n");

  if (NULL == (data = calloc(1, sizeof(struct data)))) {
    perror("malloc()");
    exit(1);
  }

  return data;
  //printf("jsd data %x\n", data);
  //return (void *)data;
}

#if 0
void
midle(void *mprocdata)
{
}
#endif

void
mrepeat(void *mprocdata)
{
  int retval;
  struct js_event ev;
  char *test = (char *)mprocdata;

	//printf("nind:mrepeat()done\n");

  if (sizeof(struct js_event) == (retval = read(fd, &ev, sizeof(struct js_event)))) {
    if (ev.type & JS_EVENT_INIT) {
      return;
    }
    //printf("%x %d %2d %3d %d\n", test, ev.type, ev.number, ev.value, ev.time);
  }

  oclJS_changeStatus(0, ev.type, ev.number, ev.value, ev.time);

  //sensors_write(ind, (NUM *)p);
}

void
mcleanup(void *mprocdata)
{
  //fob_exit();
  fprintf(stderr, "jsd: mcleanup()\n");
}
