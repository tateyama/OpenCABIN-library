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

// master only plugin

// TODO: make, edit and make !!
// TODO: mop_callback_register() sample !!
// TODO: send message from mop !!
// TODO: grapp messaging system

#include <avlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include <defaultvars.h>
#include "msg.h"
#include "pid.h"

typedef void (*CALLBACKDATA)(struct avbuf *av);
#define DEFINE_CALLBACKDATA
#include <callbackll.h>

struct mop {
  void *handle;
  void *(*minitfunc)(int ac, char **av);
  void (*mrepeatfunc)(void *);
  //void (*mthreadfunc)(void *);
  //void (*mjoystick)(int no, int type, int number, int value, struct ocl_jsstat *);
  void (*mjoystickfunc)(int no, int type, int number, int value);
  void (*mcleanupfunc)(void *);
  pthread_t thr;
  pthread_mutex_t stop_control;
  int id;
  int ac;
  char **av;
  void *callbacklist;
  struct msgqueue msgq;
  int msgexistp;
  void *procdata;
};

struct mopbuf {
  int maxnum;
  struct mop **buf;
} mopbuftop;

static int mop_current = 0;

static pthread_key_t ptk_mop;

void
mopsys_init(int num)
{
  if (num < 16) num = 16;
  mopbuftop.maxnum = num;
  if (NULL == (mopbuftop.buf = (struct mop **)calloc(1, sizeof(struct mop *) * num))) {
    perror("calloc()");
    exit(1);
  }
  pthread_key_create(&ptk_mop, 0);
}

static struct mop *
mop_new(void)
{
  int i;
  struct mop *mopptr;
  
  if (NULL == (mopptr = (struct mop *)calloc(1, sizeof(struct mop)))) {
    perror("malloc");
    exit(1);
  }
}

void
mopbuf_realloc()
{
  // TODO: reallocation
  fprintf(stderr, "ERROR: mopbuf full\n");
  exit(1);
}

struct mop *
mop_delete(int id)
{
  struct mop *mopptr;
  mopptr = mopbuftop.buf[id];
  mopbuftop.buf[id] = 0;
  return mopptr;
}

inline static struct mop *
getmop(int mopid)
{
  return mopbuftop.buf[mopid];
}

static int
mop_newid(struct mop *mopptr)
{
  int i;

  if (mop_current == mopbuftop.maxnum) {
    for (i = 0; i < mop_current; ++i) {
      if (0 == mopbuftop.buf[i]) {
	return i;
      }
    }
    mopbuf_realloc();
  }
  mopbuftop.buf[mop_current] = mopptr;
  return mop_current++;
}

void
mop_exec_minit(struct mop *mop)
{
}

void
mop_execinit(struct mop *mopptr)
{
}

static struct mop *
mop_getself(void)
{
  return (struct mop *)pthread_getspecific(ptk_mop);
}

void mop_msg_process(struct mop *mopptr);

static void
mop_checkintrs_internal(struct mop *mopptr)
{
  pthread_testcancel();
  if (mopptr->msgexistp) {
    switch (mopptr->msgexistp) {
    case 1:
      pthread_mutex_lock(&mopptr->stop_control);
      break;
    case 2:
      if (mopptr->mcleanupfunc) {
	(*mopptr->mcleanupfunc)(mopptr->procdata);
      }
      pthread_exit(0);
      break;
    case 3:
      mop_msg_process(mopptr);
      break;
    }
    mopptr->msgexistp = 0;
  }
}

// Application API: signal check (allow signal interrupt)
void
mop_checkintrs(void)
{
  struct mop *mopptr;
  mopptr = mop_getself();
  mop_checkintrs_internal(mopptr);
}

/*
// Application API
void
mop_msgcheck(void)
{
#if 0
    if (mopptr->msgs) {
    
  }
#endif
}

static void
mop_process_msgs(struct mop *mopptr)
{
  
}

void
mop_msg_cbreg(int ind, int symid, void *(*handler)(void *))
{
  struct mop *mopptr =  mopbuftop.buf[ind];
  mop_sethandler(mopptr, symid, handler);
}

void *
mop_callhandler_direct(int ind, int ac, void **av)
{
  void *retval;
  struct mop *mopptr =  mopbuftop.buf[ind];

  return retval;
}

void
mop_callhandler_str(int ind, char *name, int ac, char **av)
{
  
}
*/

void
mop_msg_send(int mopid, int ac, unsigned char **av)
{
  struct mop *mopptr;
  int symid;

  if (ac <= 0) {
    return;
  }
  if (0 == (mopptr = getmop(mopid))) {
    fprintf(stderr, "no such mop (id: %d)\n", pid_get(PID_TYPE_MOP, mopid));
    return;
  }
  msgq_push(&mopptr->msgq, ac, av);
  mopptr->msgexistp = 3;
}

//==================================================

/*
void
mop_msghookreg(int ind, int symid, void *(*handler)(void *))
{
}

void
mop_tsin()
{
}
*/

void
mop_stop(int ind)
{
  struct mop *mopptr = mopbuftop.buf[ind];
  pthread_mutex_lock(&mopptr->stop_control);
  mopptr->msgexistp = 1;
}

void
mop_cont(int ind)
{
  struct mop *mopptr = mopbuftop.buf[ind];
  mopptr->msgexistp = 0;
  pthread_mutex_unlock(&mopptr->stop_control);
}

//==================================================
// API: void mop_callback_register(int symid, void *(*f)(void *));
// API: void mop_callback_register_name(char *name, void *(*f)(void *));

void
mop_callback_register(int symid, CALLBACKDATA f)
{
  struct mop *mopptr;
  mopptr = mop_getself();
  if ((symid == sym_stop) || (symid == sym_cont) || (symid == sym_intr) || (symid == sym_kill)) {
    fprintf(stderr, "cannot register callback\n");
    return;
  };
  callback_insert(&mopptr->callbacklist, symid, f);
}

void
mop_callback_register_name(char *name, CALLBACKDATA f)
{
  int msgid = snm_intern(name);
  mop_callback_register(msgid, f);
}

//==================================================

static void
mop_msgq_init(struct mop *moppptr)
{
}

void
mop_msg_process(struct mop *mopptr)
{
  struct msgptr *ptr;
  CALLBACKDATA cb;
  msgq_lock(&mopptr->msgq);
  while (0 != (ptr = msgq_shift(&mopptr->msgq))) {
    //ptr->type;
    //ptr->avbuf;

    // user callback
    if (0 == callback_search(&mopptr->callbacklist, ptr->type, &cb)) {
      (*cb)(ptr->avbuf);
      continue;
    }

    // SIGSTOP
    if (ptr->type == sym_stop) {
      pthread_mutex_lock(&mopptr->stop_control);
      continue;
    }
#if 0
    if (ptr->type == sym_cont) {
      continue;
    }
#endif

    // SIGINTR
    if (ptr->type == sym_intr) {
      if (mopptr->mcleanupfunc) {
	(*mopptr->mcleanupfunc)(mopptr->procdata);
      }
      mopptr->msgexistp = 1;
      return;
    }

    // SIGKILL
    if (ptr->type == sym_kill) {
      //pthread_cancel(mopptr->thr);
      pthread_exit(0);
    }

    // callback not found
    // TODO: ?
  }
  msgq_unlock(&mopptr->msgq);
}

// ==================================================

// metamorphic application

struct mmapp {
  void *handle;
  void *(*minitfunc)(int ac, char **av);
  //void (*mthreadfunc)(void *);
  void (*mrepeatfunc)(void *);
  void (*midlefunc)(void *);
  void (*mjoystickfunc)(int no, int type, int number, int value);
  void (*mcleanupfunc)(void *);
  void *(*rinitfunc)(int ac, char **av);
  void (*gdrawfunc)(void *);
  void (*ginitfunc)(void *);
  void (*gidlefunc)(void *);
  void (*rcleanupfunc)(void *);
};

struct mmapp *
mmapp_load(char *path, struct mmapp *mmappptr)
{
  void *handle;

  if (NULL == (handle = dlopen(path, RTLD_LAZY))) {
    cab_error(1, "%s\n", dlerror());
    return NULL;
  }

  mmappptr->handle = handle;
  mmappptr->minitfunc = (void *(*)(int ac, char **av))dlsym(handle, "minit");
  //mmappptr->mthreadfunc = (void (*)(void *))dlsym(handle, "mthread");
  mmappptr->mrepeatfunc = (void (*)(void *))dlsym(handle, "mrepeat");
  mmappptr->mjoystickfunc = (void (*)(int no, int type, int number, int value))dlsym(handle, "mjoystick");
  mmappptr->midlefunc = (void (*)(void *))dlsym(handle, "midle");
  mmappptr->mcleanupfunc = (void (*)(void *))dlsym(handle, "mcleanup");
  mmappptr->rinitfunc = (void *(*)(int ac, char **av))dlsym(handle, "rinit");
  mmappptr->ginitfunc = (void (*)(void *))dlsym(handle, "ginit");
  mmappptr->gidlefunc = (void (*)(void *))dlsym(handle, "gidle");
  mmappptr->gdrawfunc = (void (*)(void *))dlsym(handle, "gdraw");
  mmappptr->rcleanupfunc = (void (*)(void *))dlsym(handle, "rcleanup");

  return mmappptr;
}

struct mop *
mmapp2mop(struct mmapp *mmapptr, struct mop *mopptr)
{
  mopptr->handle = mmapptr->handle;
  mopptr->minitfunc = mmapptr->minitfunc;
  mopptr->mrepeatfunc = mmapptr->mrepeatfunc;
  //mopptr->midlefunc = mmapptr->midlefunc;
  mopptr->mcleanupfunc = mmapptr->mcleanupfunc;
}

void *
mop_ithr_mainloop(struct mop *mopptr, void *procdata)
{
  while (1) {
    pthread_testcancel(); // SIGKILL
    //(*mopptr->mrepeatfunc)(mopptr);
    (*mopptr->mrepeatfunc)(procdata);
    mop_checkintrs_internal(mopptr);
  }
  return NULL;
}

void *
mop_threadproc(void *v)
{
  struct mop *mopptr = (struct mop *)v;
  void *procdata = 0;

  pthread_setspecific(ptk_mop, mopptr);

  if (mopptr->minitfunc) {
    procdata = (*mopptr->minitfunc)(mopptr->ac, mopptr->av);
  }
  mopptr->procdata = procdata;
  if (mopptr->mrepeatfunc) {
    mop_ithr_mainloop(mopptr, procdata);
  }
  if (mopptr->mcleanupfunc) {
    (*mopptr->mcleanupfunc)(procdata);
  }
  return 0;
}

void
mop_run_ithr(struct mop *mopptr, int ac, char **av)
{
  mopptr->ac = ac;
  mopptr->av = av;
  pthread_mutex_init(&mopptr->stop_control, 0);
  pthread_create(&mopptr->thr, 0, mop_threadproc, mopptr);
}

/*
#if 0 // duplicate


void
mop_ithr_newthread(struct mop *mopptr)
{
  pthread_create(&mopptr->thr, 0, mop_ithr_mainloop, (void *)mopptr);
}
#endif
*/

void
mop_run_cmd(struct mop *mopptr, int ac, char **av)
{
  
}

unsigned int
mmapp_run(int ac, char **av)
{
  unsigned int pid;
  struct mmapp mmapp;

  if (NULL == mmapp_load(av[0], &mmapp)) {
    fprintf(stderr, "\n");
    return 0;
  }

  if (mmapp.rinitfunc || mmapp.ginitfunc || mmapp.gdrawfunc ||
      mmapp.gidlefunc) {  //  I do not check mmapp.rcleanupfunc
    // graphics application

    int appid;
    int procid;
    unsigned int pid;
    
    if (0 == (appid = cmd_loadapp(av[0], "."))) {
      fprintf(stderr, "failed to load %s\n", av[1]);
      return 0;
    }
    procid = cmd_runapp(appid, ac, av);
    printf("proc: %d invoked.\n", procid);
    fflush(stdout);
    //printsyment();
    pid = pid_new(PID_TYPE_APP, procid);
    mpm_procinit1(procid);
    fprintf(stderr, "pid %d (grapp) running.\n", pid);
    return pid;
  }

  //if (mmapp.mthreadfunc) {
  if (mmapp.mrepeatfunc) {
    // master only daemon (independent thread)
    int mopid;
    struct mop *mopptr;

    mopptr = mop_new();
    mopid = mop_newid(mopptr);
    mmapp2mop(&mmapp, mopptr);
    pid = pid_new(PID_TYPE_MOP, mopid);
    printf("pid %d (mop) running.\n", pid);
    mop_run_ithr(mopptr, ac, av);
    return pid;
  } else if (mmapp.midlefunc) {
    // master only daemon (shared thread)
    return;
  }
  // master only command
  // only minit, midle, mcleanup
  return;
}
