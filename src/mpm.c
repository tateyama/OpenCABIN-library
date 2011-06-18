/* OpenCABIN Library

   Copyright (C) 2006,2007,2008 KGT Inc.

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
/* master plugin mechanism */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "caberror.h"
#include <pthread.h>

void mctrl_idle(void);

#define APPNUM 32

struct app {
  char (*dlhandle);
  int id;
  int initialized; // <-- DANGER: proc!
  char *name;
  void *(*minit)(int ac, char **av);
  //  void *(*rinit)(int ac, char **av);
  //void (*ginit)(void *);
  //void (*draw)(void *);
  void (*midle)(void *);
  void (*mcleanup)(void *);
  void (*mjoystick)(int no, int type, int number, int value, int time);
};

static struct app apparray[APPNUM];

#define PROCNUM 1024
struct process {
  struct app *app;
  int procid;
  char initialized;
  int ac;
  char **av;
  void *mdata;
  void *rdata;
};

static struct process procarray[PROCNUM];

int
mpm_proc_new(struct app *aptr, int procid, int ac, char **av)
{
  int i;
  for (i = 0; i < PROCNUM; ++i) {
    if (0 == procarray[i].app) {
      goto newslotfound;
    }
  }
  cab_error(1, "mpm_proc_new: no process slot. increase PROCNUM (current %d)\n",
	    PROCNUM);
  return -1;
 newslotfound:
  procarray[i].app = aptr;
  procarray[i].procid = procid;
  procarray[i].ac = ac;
  procarray[i].av = av;
  procarray[i].initialized = 0;

  svdb_newPObject(procid);

  return i;
}

#include <sys/uio.h>

void
mpm_avdeepcopy(struct iovec *retval, int ac, char **av)
{
  int size, size0, i, s;
  char **avbuf, *bufptr, *buf;

  // av deep copy
  size0 = (ac + 1) * sizeof(char *);
  size = size0;
  for (i = 0; i < ac; ++i) {
    size += strlen(av[i]) + 1;
  }

  if (NULL == (buf = (char *)malloc(size))) {
    cab_error(1, "cmd_runapp: cannot malloc()\n");
    exit(1);
  }

  bufptr = buf + size0;
  //avbuf = (char **)retval;
  avbuf = (char **)buf;

  for (i = 0; i < ac; ++i) {
    avbuf[i] = bufptr;
    s = strlen(av[i]) + 1;
    memcpy(bufptr, av[i], s);
    bufptr += s;
  }

  avbuf[ac] = 0;

  retval->iov_len = size;
  retval->iov_base = buf;
}

static struct app *
app_get(int appid)
{
  int i;

  for (i = 0; i < APPNUM; ++i) {
    if (appid == apparray[i].id) {
      return &apparray[i];
    }
  }
  return 0;
} 

int
mpm_runapp(struct iovec *retval, int appid, int ac, char **av)
{
  struct app *appptr;
  int procid;

  if (0 == (appptr = app_get(appid))) {
    cab_error(1, "mpm_runapp(): cannot find appID(%d)\n", appid);
    return -1;
  }
  
  mpm_avdeepcopy(retval, ac, av);

  procid = newprocid();

  if (0 > mpm_proc_new(appptr, procid, ac, (char **)retval->iov_base)) {
    return -1;
  }
  
  return procid;
}

static struct app *app_new(struct app *aptr, char *name);

int
mpm_app_new2(char *name, int appid)
{
  int i;

  for (i = 0; i < APPNUM; ++i) {
    if (0 == apparray[i].dlhandle) {
      break;
    }
  }
  // DANGEROUS
  printf("mpm_app_new2 %d %s\n", i, name);

  apparray[i].id = appid;

  if (0 == app_new(&apparray[i], name)) {
    return 0;
  } else {
    return 1;
  }
}

static
struct app *
app_new(struct app *aptr, char *name)
{
  char *handle, *fname;

  void (*f)();
  if (NULL == (handle = dlopen(name, RTLD_LAZY))) {
    //cab_error(1, "dlopen(%s): %s\n", name, dlerror());
    cab_error(1, "%s\n", dlerror());
    return NULL;
  }

  fname = "minit";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }

  aptr->minit = (void *(*)(int ac, char **av))f;
#if 0
  fname = "ginit";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->ginit = f;

  fname = "draw";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->draw = f;
#endif
  fname = "midle";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->midle = (void (*)(void *))f;

  fname = "mcleanup";
  if (NULL == (f = dlsym(handle, fname))) {
    cab_error(1, "dlsym(%s, %s): %s\n", name, fname, dlerror());
    dlclose(handle);
    return NULL;
  }
  aptr->mcleanup = (void (*)(void *))f;

  //
  aptr->mjoystick = (void (*)(int no, int type, int number, int value, int time))dlsym(handle, "mjoystick");

  aptr->name = strdup(name);
  aptr->dlhandle = handle;
  return aptr;
}


void
mctrl_scheduler()
{
}

void mpm_pull_mjoysticks(void);
void setPOIDm(int procid);

void
mpm_idle()
{
  int i;
  struct process *procptr;
  for (i = 0; i < PROCNUM; ++i) {
    if (procarray[i].app && procarray[i].initialized) {
      setPOIDm(procarray[i].procid);
      procptr = &procarray[i];
      (*procptr->app->midle)(procptr->mdata);
    }
  }
  mpm_pull_mjoysticks();

}

static struct process *
getproc(int procid)
{
  int i;
  struct process *procptr;
  for (i = 0; i < PROCNUM; ++i) {
    if (procarray[i].procid == procid) {
      return &procarray[i];
    }
  }
  return 0;
}

static int cur_procid = -1;

int
getPOIDm(void)
{
  return cur_procid;
}

void
setPOIDm(int procid)
{
  cur_procid = procid;
}

// if the process exists, return 1. else 0.
int
mpm_proc_existp(int procid)
{
#if 0
  struct process *procptr = getproc(procid);
  if (0 == procptr) {
    return 0;
  };
  return 1;
#else
  return (0 == getproc(procid)) ? 0 : 1;
#endif
}

int
mpm_getargv(int procid, int *ac, char ***av)
{
  struct process *procptr = getproc(procid);
  if (0 == procptr) {
    return -1;
  };
  *ac = procptr->ac;
  *av = procptr->av;
  return 0;
}

void
mpm_procinit1(int procid)
{
  struct process *procptr = getproc(procid);
  setPOIDm(procid);
  void *(*f)(int, char **);
  //fprintf(stderr, "pass1: ");printsyment();
  //fprintf(stderr, "pass1-: ");printsyment();
#if 1
  procptr->mdata = (*procptr->app->minit)(procptr->ac, procptr->av);
#else
  f = procptr->app->minit;
  procptr->mdata = (*f)(0, 0);
#endif
  procptr->initialized = 1;
}

// buggy?
void
mpm_procinit()
{
  int i;
  struct process *procptr;

  for (i = 0; i < PROCNUM; ++i) {
    if (procarray[i].app && (procarray[i].initialized == 0)) {
      procptr = &procarray[i];
      //printf("app_minit: %s\n", procarray[i].app->name);
      //printsyment();
      //fprintf(stderr, "i: %d\n", i);
      //fprintf(stderr, "proc: %x\n", procptr);
      //fprintf(stderr, "app: %x\n", procptr->app);
      procptr->mdata = (*procptr->app->minit)(procptr->ac, procptr->av);
      //printsyment();
#if 0
      printf("app_ginit: %s\n", procarray[i].app->name);
      (*procarray[i].app->ginit)();
      printf("app_init_done: %s\n", procarray[i].app->name);
#endif
      procptr->initialized = 1;
    }
  }
}

void
mpm_call_mjoystick(int no, int type, int number, int value, int time)
{
  int i;

  for (i = 0; i < PROCNUM; ++i) {
    if ((procarray[i].app)&&((procarray[i].app)->mjoystick)) {
      (*((procarray[i].app)->mjoystick))(no, type, number, value, time);
      //return;
    }
  }
}

struct mjoystickdata {
  int no, type, number, value, time;
  struct mjoystickdata *next;
};

static struct mjoystickdata g_mjd_queue;
pthread_mutex_t g_mjd_mutex = PTHREAD_MUTEX_INITIALIZER;
int g_mjd_count = 0;

void
mpm_push_mjoystick(int no, int type, int number, int value, int time)
{
  struct mjoystickdata *ptr, *ptr2;
  pthread_mutex_lock(&g_mjd_mutex);
  if (NULL == (ptr = malloc(sizeof(struct mjoystickdata)))) {
    perror("malloc()");
    exit(1);
  }
  ptr->no = no;
  ptr->type = type;
  ptr->number = number;
  ptr->value = value;
  ptr->time = time;

  for (ptr2 = &g_mjd_queue; ptr2->next; ptr2 = ptr2->next) {};

  ptr2->next = ptr;
  ptr->next = 0;

  pthread_mutex_unlock(&g_mjd_mutex);
}

void
mpm_pull_mjoysticks(void)
{
  struct mjoystickdata *ptr, *ptr2;

  pthread_mutex_lock(&g_mjd_mutex);

  ptr = g_mjd_queue.next;
  g_mjd_queue.next = 0;
  while (ptr) {
    mpm_call_mjoystick(ptr->no, ptr->type, ptr->number, ptr->value, ptr->time);
    ptr2 = ptr->next;
    free(ptr);
    ptr = ptr2;
  }

  pthread_mutex_unlock(&g_mjd_mutex);
}

