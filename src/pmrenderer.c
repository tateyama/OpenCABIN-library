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
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "rw.h"

//#include "oclgvars.h"

#if 0
// for struct iovec
#include <sys/types.h>
#include <sys/uio.h>
#endif

#include "cabprot.h"
//#include "mctrl.h"
//#include "rendmem.h"

#include "dlfcn.h"

#define MAXAPPS 32

int g_newprocp = 0;

struct app {
  char *dlhandle;
  int appid;
  char *name;
  char *path;
  char *dir; // <- WRONG!!
  int refcount;
  void (*ginit)(void *);
  void (*gidle)(void *);
  void *(*rinit)(int ac, char **av);
  void (*draw)(void *);
};

static struct app apps[MAXAPPS];

#define PROCNUM 1024
struct process {
  struct app *app;
  int procid;
  char initialized;
  char display;
  char exit;
  char quit;
  int ac;
  char **av;
  struct process *next;
  void *data;
};

int g_maxmemcount = 0;
#define MEMNUM 1024

static struct process procs[PROCNUM];

static struct process liveprocs;

//==================================================

int curappnum = 0;

void
appinit(void)
{
  int i;
  for (i = 0; i < MAXAPPS; ++i) {
    apps[i].appid = -1;
  }
}


static struct app *
getapp(int appid)
{
  int i;
  for (i = 0; i < MAXAPPS; ++i) {
    if (apps[i].appid == appid) {
      return &apps[i];
    };
  }
  //cab_error(1, "renderer: getapp(): appid = %d query failed\n", appid);
  // nosuch appid
  return 0;
}

static
struct app *
newapp(void)
{
  int i;
  for (i = 0; i < MAXAPPS; ++i) {
    if (apps[i].appid == -1) {
      return &apps[i];
    };
  }
  cab_error(1, "renderer appslot overflow (MAXAPPS = %d)\n", MAXAPPS);
  return 0;
}

static
struct app*
findapp_handle(void *handle)
{
  int i;
  for (i = 0; i < MAXAPPS; ++i) {
    if (apps[i].dlhandle == handle) {
      return &apps[i];
    };
  }
  // no such handle
  return 0;
}

static
struct app *
rapp_new(struct app *aptr, char *name)
{
  char *handle, *fname;

  void (*f)();
  if (NULL == (handle = dlopen(name, RTLD_LAZY))) {
    //cab_error(1, "dlopen(%s): %s\n", name, dlerror());
    cab_error(1, "%s\n", dlerror());
    return NULL;
  }

  fname = "ginit";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->ginit = f;

  fname = "rinit";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }

  aptr->rinit = (void *(*)(int ac, char **av))f;

  fname = "gidle";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->gidle = (void (*)(void *))f;

  fname = "gdraw";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->draw = (void (*)(void *))f;


  //aptr->name = strdup(name);
  aptr->dlhandle = handle;

  return aptr;
}

int
oclint_appid_existp(int appid)
{
  return (0 == getapp(appid)) ? 0 : 1;
}

//==================================================
static struct process *
getproc(int id)
{
  struct process *tmp = liveprocs.next;

  while (tmp) {
    if (tmp->procid == id) {
      return tmp;
    }
    tmp = tmp->next;
  }
  // no such process
  return 0;
}

void (*oclint_app_drawfunc(int id))(void *)
{
  struct app *appptr = getapp(id);
  return appptr->draw;
}

static
struct process *
newproc(void)
{
  int i;

  for (i = 0; i < PROCNUM; ++i) {
    if (0 == procs[i].app) {
      procs[i].next = liveprocs.next;
      liveprocs.next = &procs[i];
      return &procs[i];
    }
  }
  
  return 0;
}

// for main thread
static int current_poid_forMainThread = 0;

static void
setPOID_forMainThread(int poid)
{
  current_poid_forMainThread = poid;
}

int
getPOID()
{
  return current_poid_forMainThread;
}

// manage procs as "list"?

int
oclint_procs_foreach(int (*cbfunc)(int app_id, int proc_id, void *procdata, void *val),
	     void *val, char *cmtstr)
{
  int i;
  int r;
  int retval;
  struct process *tmp = liveprocs.next;

  while (tmp) {
    setPOID_forMainThread(tmp->procid);
    if (tmp->initialized) {
      if (0 != (r = (*cbfunc)(tmp->app->appid, tmp->procid, tmp->data, val))) {
	cab_error(1, "apps_foreach(): %s\n", cmtstr);
	return r;
      }
    }
    tmp = tmp->next;
  }
  return 0;
}

int
oclint_proc_getDisplay(int procid)
{
  struct process *ptr = getproc(procid);
  if (0 == ptr) return -1;
  return ptr->display;
}
//==================================================
void
rdo_loadapp(int appid, char *path, char *dir)
{
  struct app *appptr = getapp(appid);

  if (0 == appptr) {
    if (0 == (appptr = newapp())) {
      cab_error(1, "cannot rdo_loadapp\n");
      exit(1);
    }
    appptr->appid = appid;
    appptr->path = strdup(path);
    appptr->name = appptr->path;
    appptr->dir = strdup(dir);
    rapp_new(appptr, path);
  }

  fprintf(stderr, "loadapp success %d %s %s\n", appid, path, dir);
}

void
rcmd_loadapp(int newfd)
{
  int size, len, retval = 0, appid;
  char *path;
  char *dir;

  if (0 >= readforce(newfd, (char *)&size, sizeof(size))) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }

  if (0 == (path = (char *)mallocm(size))) {
    cab_error(1, "cannot malloc() for loadapp\n");
    return;
  }

  if (0 >= readforce(newfd, path, size)) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }

printf("rcmd_loadapp: size: %d\n", size);
  dir = path + strlen(path) + 1;

  retval = 0;

  if (0 >= writeforce(newfd, (char *)&retval, sizeof(retval))) {
    cab_error(1, "cannot send a 'loadapp' command result to master\n");
    exit(1);
  }

  if (0 >= readforce(newfd, (char *)&appid, sizeof(appid))) {
    cab_error(1, "cannot get 'loadapp' application ID from master\n");
    exit(1);
  }

  if (appid < -1) {
    fprintf(stderr, "loadapp canceled.\n");
    return;
  }

  rdo_loadapp(appid, path, dir);
}

void
rdo_runapp(int appid, int procid, int ac, char **av)
{
  struct app *appptr;
  struct process *procptr;

  if (0 == (appptr = getapp(appid))) {
    cab_error(1, "renderer: cannot runapp: no such application (appid = %d)\n", appid);
    return;
  }
  if (0 == (procptr = newproc())) {
    cab_error(1, "renderer: cannot runapp\n", appid);
    return;
  }
  procptr->procid = procid; // <- dup? (arrayno == procid?)
  procptr->app = appptr;
  procptr->initialized = 0;
  procptr->display = 1;
  procptr->exit = 0;
  procptr->quit = 0;
  procptr->ac = ac;
  procptr->av = av;
  //printf("rdo_runapp\n");
  //procptr->initialized = 1; //DANGEROUS
  svdb_newPObject(procid);
}

void
rcmd_runapp(int newfd)
{
  int procid, ac, size, size0, appid, i, len;
  char **avbuf, *bufptr;

  if (0 >= readforce(newfd, (char *)&appid, sizeof(appid))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  if (0 >= readforce(newfd, (char *)&procid, sizeof(procid))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  if (0 >= readforce(newfd, (char *)&ac, sizeof(ac))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  if (0 >= readforce(newfd, (char *)&size, sizeof(size))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  size0 = sizeof(char *) * (ac + 1);

  if (0 == (avbuf = (char **)mallocm(size + size0))) {
    cab_error(1, "cannot runapp\n");
    exit(1);
  }

  bufptr = ((char *)avbuf) + size0;

  if (0 >= readforce(newfd, bufptr, size)) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  avbuf[0] = bufptr;

  for (i = 1; i < ac; i++) {
    len = strlen(bufptr);
    bufptr += len + 1;
    avbuf[i] = bufptr;
  }

  avbuf[ac] = 0;

  rdo_runapp(appid, procid, ac, avbuf);
  g_newprocp = 1;
}

//==================================================
void
rcmd_dispoff(int newfd)
{
  int procid;
  struct process *pptr;

  if (0 >= readforce(newfd, (char *)&procid, sizeof(procid))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  if (0 == (pptr = getproc(procid))) {
    fprintf(stderr, "rcmd_dispoff: no such pid %d\n", procid);
    return;
  }

  pptr->display = 0;
}

//==================================================
void
rcmd_dispon(int newfd)
{
  int procid;
  struct process *pptr;

  if (0 >= readforce(newfd, (char *)&procid, sizeof(procid))) {
    cab_error(1, "cannot get runapp command from master\n");
    exit(1);
  }

  if (0 == (pptr = getproc(procid))) {
    fprintf(stderr, "rcmd_dispoff: no such pid %d\n", procid);
    return;
  }

  pptr->display = 1;
}
//==================================================

void
rcmd_exit(int newfd)
{
  
}
// remember: do rbase_regrcmd()

//==================================================
void
rcmd_abort(int newfd)
{
  
}
// remember: do rbase_regrcmd()

//==================================================
#if 0
void
oclPostRedisplay(void)
{
  rg_postRedisplay();
}
#endif

// run by mainthread
void
runidle(void)
{
  int i;
  struct process *procptr;

  for (i = 0; i < PROCNUM; ++i) {
    if (0 != procs[i].app) {
      procptr = &procs[i];
      setPOID_forMainThread(procptr->procid);
      if (0 == procptr->initialized) {
	procptr->data = (*procptr->app->rinit)(procptr->ac, procptr->av);
	(*procptr->app->ginit)(procptr->data);
	//printf("%s: ginit\n", procs[i].app->name);
	procptr->initialized = 1;
	rg_postRedisplay();
      }
      (*procptr->app->gidle)(procptr->data);
      //printf("%s: gidle\n", procs[i].app->name);
    }
  }
}

 

void
plugin_init(void)
{
  appinit();
  rbase_regrcmd(RENDCMD_LOADAPP, rcmd_loadapp);
  rbase_regrcmd(RENDCMD_RUNAPP, rcmd_runapp);
  rbase_regrcmd(RENDCMD_DISPOFF, rcmd_dispoff);
  rbase_regrcmd(RENDCMD_DISPON, rcmd_dispon);
  //rbase_regcmd(RENDCMD_KILLPROC,);
  //rbase_regcmd(RENDCMD_NEWMEM, rcmd_memnew);
  //rbase_regcmd(RENDCMD_MEMUPDATE, rcmd_memupdate);
};
