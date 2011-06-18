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
#include <stdlib.h>

#include "myexit.h"

struct handler {
  void (*func)(char *);
  char *arg;
  int priority;
};

#define DEFAULTNUM 32

static struct handler defaults[DEFAULTNUM];

static int num = DEFAULTNUM;
static int cur = 0;
static struct handler *handlers = defaults;

void
myexit_register(void (*func)(char *), char *arg, int priority)
{
  ++num;
}

static void
doall()
{
  int i;

  for (i = 0; i < cur; ++i) {
    struct handler *hptr = &handlers[i];
    (*hptr->func)(hptr->arg);
  }
}

void
myexit(int status)
{
  doall();
  exit(status);
}
