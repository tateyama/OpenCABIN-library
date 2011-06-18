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
// rbase.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/param.h>
#include <unistd.h>
#include <signal.h>

static int mainfd = 0;

void plugin_init(void);

int
rbase_init(short port)
{
  mainfd = tcpbind(port);
  // wait connection

  plugin_init(); // pmrenderer.c
  vdmlc_init(0, 0); // vdml.c
  SHvar_cinit(); // shared_variable.c
  return mainfd;
}

#define MAXRCMDS 256
static void (*rcmdarray[MAXRCMDS])(int);
static maxrcmd = 0;

void
rbase_regrcmd(int cmdid, void (*func)())
{
  if (rcmdarray[cmdid]) {
    fprintf(stderr, "rbase_regrcmd error: cmdid collision\n");
    exit(1);
  }
  if (MAXRCMDS <= cmdid) {
    fprintf(stderr, "rbase_regrcmd() error: cmdid (%d) exceeds MAXRCMDS\n", cmdid);
    exit(1);
  }
  rcmdarray[cmdid] = func;
}

static int sg_cmdfd;
void rbase_cmdinput(int fd);

void
renderer_input(int val)
{
  rbase_cmdinput(val);
}

void
rbase_cmdinput(int fd)
{
  int i;
  int rcmd;

  if (0 >= readforce(fd, (char *)&rcmd, sizeof(rcmd))) {
    cab_error(1, "cannot get rendering command from master\n");
    exit(1);
  }
  
  if (rcmd >= MAXRCMDS) {
    cab_error(1, "rbase_readcmdloop(): rcmd id (%d) exceeds MAXRCMDS\n", rcmd);
    return;
  }
  
  if (rcmdarray[rcmd]) {
    rcmdarray[rcmd](fd);
    return;
  }
}

void
rbase_readcmdloop()
{
  while (1) {
    rbase_cmdinput(sg_cmdfd);
  }
}

static
pthread_t cmdrecvthr;

void *
rbase_cmdrecvthrproc(void *val)
{
  rbase_readcmdloop();
}

static void
newthread_cmdrecvthr(void)
{
  pthread_create(&cmdrecvthr, 0, rbase_cmdrecvthrproc, 0);
}

static
pthread_t mainthr;

void *rbase_gthrproc(void *val);

#if 0
void *
rbase_gthrproc(void *val)
{
  
}
#endif


int
rbase_accept_master(int fd, int val)
{
  return fdproc_accept(fd);
}

int
rbase_main0(void)
{
  char *name = "test";
  int bfd;
  short port = 8980;

  bfd = rbase_init(port);
  //fdproc_init();
  //fdproc_register(bfd, rbase_accept_master, 0);
  //fdproc_mainloop();
  //bfd = tcpbind(8980);
  rg_init();
  while (1) {
    //rg_init();
    sg_cmdfd = rbase_accept_master(bfd, 0);
    newthread_cmdrecvthr();
    cinit_defaultvars(); //defaultvars.c
    sensors_initC(); // sensors.c
    rg_mainloop(name);
    close(sg_cmdfd);
  }
  return 0;
}

short g_port = 28980;

int
rbase_main(void)
{
  char *name = "test";
  int bfd;
  if (g_port <= 0) {
    g_port = 28980;
  }

  bfd = rbase_init(g_port);
  //bfd = tcpbind(8980);
  //fprintf(stderr, "connected\n");
  rg_init();
  fprintf(stderr, "initialized.\n");
  sg_cmdfd = rbase_accept_master(bfd, 0);
  fprintf(stderr, "connected.\n");
  newthread_cmdrecvthr();
  sensors_initC(); // sensors.c
  rg_mainloop(name);
  close(sg_cmdfd);
  //execl();
  return 0;
}

static void
sighand_reboot(int sig)
{
}

void
rbase_closeStdin()
{
  close(0);
}

void
rbase_daemon(char **av)
{
  char cwdbuf[MAXPATHLEN];

  signal(SIGINT, sighand_reboot);
  getcwd(cwdbuf, MAXPATHLEN);

  while (1) {
    rbase_main(); // <- ?
    
    chdir(cwdbuf);
    execv(av[0], av);
  }
}
