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
// ==============
//  cluster base
// ==============
// for master
//
// features
// 
// * intra-cluster communication (distribution)
// * 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "cabprot.h"
//#include "cabapi.h"
#include "cluster_base.h"

static struct renderer rarray[RENDERES_MAX];

//int renderer_count = 0;
static int renderer_count = 0;
//int next_renderer_id = 1;

void
remove_renderer(int i)
{
  int j;
  int fd = rarray[i].fd;
  close(fd);
  cab_error(1, "remove_renderer(%s): close %d\n", rarray[i].name, fd);
  free(rarray[i].name);
  for (j = i; j < renderer_count - 1; ++j) {
    rarray[j] = rarray[j + 1];
  }
  --renderer_count;
}



void
sendcmd2renderers(int cmd, char *msg, int size)
{
  int fd, i;
  for (i = 0; i < renderer_count; ++i) {
    fd = rarray[i].fd;
    if (0 >= writeforce(fd, (char *)&cmd, sizeof(cmd))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
    if (0 >= writeforce(fd, (char *)&size, sizeof(size))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
    if (0 >= writeforce(fd, msg, size)) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
  }
}

void
send2renderers_int(int val)
{
  int fd, i;
  for (i = 0; i < renderer_count; ++i) {
    fd = rarray[i].fd;
    if (0 >= writeforce(fd, (char *)&val, sizeof(val))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
  }
}

void
sendint2renderers(int value)
{
  int fd, i;
  for (i = 0; i < renderer_count; ++i) {
    fd = rarray[i].fd;
    if (0 >= writeforce(fd, (char *)&value, sizeof(value))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
  }
}

void
send2renderers(char *msg, int size)
{
  int fd, i;
  for (i = 0; i < renderer_count; ++i) {
    fd = rarray[i].fd;
#if 0
    if (0 >= writeforce(fd, (char *)&size, sizeof(size))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
#endif
    if (0 >= writeforce(fd, msg, size)) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
  }
}

int g_curprocid = 0;

//==================================================

int
renderer_getfd(int id)
{
  return rarray[id].fd;
}

char *
renderer_getname(int id)
{
  return rarray[id].name;
}

int
renderers_foreach(int (*cbfunc)(int rdr_id, void *val),
		  void *val, char *cmtstr)
{
  int i;
  int r;
  int retval;

  for (i = 0; i < renderer_count; ++i) {
    if (0 != (r = (*cbfunc)(i, val))) {
      cab_error(1, "renderer_foreach(): %s\n", cmtstr);
      return r;
    }
  }
  return 0;
}

//int oclNumRenderers(void)
int
oclint_renderers_num(void)
{
  return renderer_count;
}

//==================================================
// int process_new_renderer(int newfd)
//     master function

#if 1
int
process_new_renderer(int fd, char *name)
{
  int i;

  for (i = 0; i < RENDERES_MAX; ++i) {
    if (0 == rarray[i].name) {
      rarray[i].name = strdup(name);
      rarray[i].fd = fd;
      ++renderer_count;
      return i;
    }
  }
  cab_error(1, "RENDERES_MAX\n");
  return 0;
}
#else
int
process_new_renderer(int newfd)
{
  int i, retval;

#if 0
  printf("new renderer fd=%d\n", newfd);
  {
    char buf[BUFSIZ];
    int size, retval;
    char *str;

    retval = readforce(newfd, (char *)&size, sizeof(size));
    if (retval <= 0) {
      cab_perror(1, "negotiation after connection failed (1): ");
      close(newfd);
      return 0;
    }
    
    if (0 == (str = (char *)mallocm(size + 1))) {
      cab_error(1, "malloc failed\n");
      close(newfd);
      return 0;
    }

    if (0 >= readforce(newfd, str, size)) {
      cab_perror(1, "negotiation after connection failed (2): ");
      free(str);
      close(newfd);
      return 0;
    }

    str[size] = 0;

    retval = writeforce(newfd, (char *)&next_renderer_id, sizeof(next_renderer_id));
    if (retval <= 0) {
      cab_perror(1, "cannot send renderer(%s) id: ", str);
      free(str);
      close(newfd);
      return 0;
    }

    printf("new renderer %d: %s\n", next_renderer_id, str);
    
    rarray[renderer_count].name = str;
    rarray[renderer_count].rid = next_renderer_id;
    ++next_renderer_id;
  }
#endif
  // new renderer
  rarray[renderer_count].fd = newfd;
  retval = renderer_count;
  ++renderer_count;
  return retval;
}
#endif
//==================================================
void
cb_init()
{
  int i;
  for (i = 0; i < RENDERES_MAX; ++i) {
    rarray[i].fd = -1;
  }
}

#if 1
int g_welcome_new_renderer = 1;

//==================================================
int g_app_count = 0;
int g_process_count = 0;

int
newappid(void)
{
  return ++g_app_count;
}

int
newprocid(void)
{
  return ++g_process_count;
}
#endif
