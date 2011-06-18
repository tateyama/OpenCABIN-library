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
// plugin manager master 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cabprot.h>
#include <cluster_base.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// ==================================================
// LOADAPP Command

static int
loadapp_cb(int rdr_id, void *val)
{
  int retval;
  int rdr_fd = renderer_getfd(rdr_id);
  char *path = (char *)val;

  if (0 >= readforce(rdr_fd, (char *)&retval, sizeof(retval))) {
    cab_error(1, "write fail. remove renderer: %s\n", renderer_getname(rdr_id));
    remove_renderer(rdr_id);
  }

  if (retval != 0) {
    //appid = -1;
    fprintf(stderr, "cannot run %s on %s\n", path, renderer_getname(rdr_id));
  }
  return 0;
}

#define CMTSTRNUM 256

/* return appid */
int
cmd_loadapp(char *path, char *dir)
{
  int len1 = strlen(path), len2 = strlen(dir);
  char *cmdbuf;
  char cmtstr[CMTSTRNUM];
  int fd, i, appid = 0, retval;
  
  if (0 == (cmdbuf = (char *)mallocm(len1 + len2 + 2))) {
    cab_error(1, "while cmd_loadapp\n");
    exit(1);
  }
  strcpy(cmdbuf, path);
  strcpy(cmdbuf + len1 + 1, dir);
  sendcmd2renderers(RENDCMD_LOADAPP, cmdbuf, len1 + len2 + 2);

  if (appid != -1) {
    appid = newappid();
  }
  retval = mpm_app_new2(path, appid);

#if 1
  snprintf(cmtstr, CMTSTRNUM, "loadapp(%s)", path);
  cmtstr[CMTSTRNUM - 1] = 0;
  if (0 != renderers_foreach(loadapp_cb, (void *)path, cmtstr)) {
    cab_error(1, "fail to load %s\n", path);
    return 0;
  }
#else
  for (i = 0; i < renderer_count; ++i) {
    fd = rarray[i].fd;
    if (0 >= readforce(fd, (char *)&retval, sizeof(retval))) {
      cab_error(1, "write fail. remove renderer: %s\n", rarray[i].name);
      remove_renderer(i);
    }
    if (retval != 0) {
      appid = -1;
      fprintf(stderr, "cannot run %s on %s\n", path, rarray[i].name);
    }
  }
#endif
  sendint2renderers(appid);
  return appid;
}

// ==================================================
// RUNAPP Command

/* return procid */
int
cmd_runapp(int appid, int ac, char **av)
{
  struct iovec retval;
  int procid;
  int avsize = sizeof(char *) * (ac + 1), size;

  if (0 > (procid = mpm_runapp(&retval, appid, ac, av))) {
    cab_error(1, "cannot runapp (appid = %d)\n", appid);
    exit(1);
  }

  sendint2renderers(RENDCMD_RUNAPP);
  sendint2renderers(appid);
  sendint2renderers(procid);
  sendint2renderers(ac);

  size = retval.iov_len - avsize;
  sendint2renderers(size);

  send2renderers(retval.iov_base + avsize, size);

  return procid;
}

// ==================================================
// DISPOFF Command

/* return procid */
void
cmd_dispoff(int procid)
{
  sendint2renderers(RENDCMD_DISPOFF);
  sendint2renderers(procid);
}

// ==================================================
// DISPON command

void
cmd_dispon(int procid)
{
  sendint2renderers(RENDCMD_DISPON);
  sendint2renderers(procid);
}

// ==================================================
// EXIT command

void
cmd_exit(int procid)
{
  sendint2renderers(RENDCMD_EXIT);
  sendint2renderers(procid);
}

// ==================================================
// ABORT command

void
cmd_abort(procid)
{
  sendint2renderers(RENDCMD_ABORT);
  sendint2renderers(procid);
}
