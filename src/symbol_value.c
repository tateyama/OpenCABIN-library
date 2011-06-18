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
#include <string.h>
#include <pthread.h>
#include "cabprot.h"

// symbol_value.c

// symbol value  db

// * poid: plugin object ID は本来，隠されるもの．
// * plugin entity ?
// * Mymalloc();

int getPOID(void);

struct symval {
  int symid, val;
};

#define MAXTYPENUM 2

//==================================================
// symbol value buffer

struct svbuf {
  int maxsize;
  int cur;
  struct symval array[0];
};

static
struct svbuf *
svbuf_new(int n)
{
  int size;
  struct svbuf *retval;

  if (n < 16) n = 512;

  size = sizeof(struct svbuf) + sizeof(struct symval) * n;

  if (NULL == (retval = (struct svbuf *) malloc(size))) {
    perror("malloc()");
    exit(1);
  }
  memset(retval, 0, size);
  retval->maxsize = n;
  return retval;
}

static
struct symval *
svbuf_get(struct svbuf *ptr, int symid)
{
  int n = ptr->cur, i;

  for (i = 0; i < n; ++i) {
    if (symid  == (ptr->array[i]).symid) {
      return &(ptr->array[i]);
    }
  }
  return 0;
}


static
int
svbuf_getval(struct svbuf *ptr, int symid)
{
  struct symval *svent = svbuf_get(ptr, symid);

  return svent ? svent->val : -1;
}

static
void
svbuf_register(struct svbuf **pptr, int symid, int val)
{
  struct svbuf *ptr = *pptr;
  struct symval *svptr;

  // overflow, then realloc()
  if (ptr->cur >= ptr->maxsize) {
    int newnum = ptr->maxsize * 2;
    int newsize = sizeof(struct svbuf) + sizeof(struct symval) * newnum;

    if (NULL == (ptr = (struct svbuf *)realloc(ptr, newsize))) {
      perror("realloc()");
      exit(1);
    }
    *pptr = ptr;
  }

  svptr = &(ptr->array[ptr->cur]);
  svptr->symid = symid;
  svptr->val = val;
  ++(ptr->cur);
}

#if 1
static
int
svbuf_set(struct svbuf **pptr, int symid, int val)
{
  struct symval *svent = svbuf_get(*pptr, symid);

  if (svent) {
    svent->val = val;
  } else {
    svbuf_register(pptr, symid, val);
  }
  return 0;
}
#else
static
int
svbuf_set(struct svbuf **pptr, int symid, int val)
{
  // nocheck
  
}
#endif
//==================================================
// The symval db in a plugin object.

static struct svbuf **povalarray[MAXTYPENUM];

#if 0
static int typenum_cur = 0;
static int typenum_max = 0;
#endif

static int ponum_cur = 0;
static int ponum_max = 256;

void
svdb_init(void)
{
  int size = sizeof(struct svbuf *) * ponum_max;
  int t;

  for (t = 0; t < MAXTYPENUM; ++t) {
    if (NULL == (povalarray[t] = (struct svbuf **)malloc(size))) {
      perror("malloc()");
      exit(1);
    }
    memset(povalarray[t], 0, size);
  }
}

// call this function when new plugin object create.
void
svdb_newPObject_typed(int typeid, int poid)
{
  struct svbuf **bufptr = povalarray[typeid];
  int newponummax, i;

  // ensure the povalarray has enough space
  if (ponum_max < poid) {
    int size;
    newponummax = (poid + 255) & ~255;
    size = sizeof(struct symval *) * newponummax;
    if (NULL == (povalarray[typeid] = (struct svbuf **)realloc(povalarray[typeid], size))) {
      perror("realloc()");
      exit(1);
    }
    for (i = ponum_max; i < newponummax; ++i) {
      povalarray[typeid][i] = (struct svbuf *)0;
    }
    ponum_max = newponummax;
  }


  // a poess has symvals (svbuf)
  if (povalarray[typeid][poid] == 0) {
    povalarray[typeid][poid] = svbuf_new(0);
  }
}

void
svdb_newPObject(int poid)
{
  int t;

  for (t = 0; t < MAXTYPENUM; ++t) {
    svdb_newPObject_typed(t, poid);
  }
}


#if 0
void
svdb_newtype(int typeid)
{
}
#endif


// client api
int
svdb_getval(int typeid, int poid, int symid)
{
  return svbuf_getval(povalarray[typeid][poid], symid);
}

// master & client background
int
svdb_set(int typeid, int poid, int symid, int val)
{
  return svbuf_set(&(povalarray[typeid][poid]), symid, val);
}

//==================================================

void
valm_init(void)
{
  svdb_init();
  svdb_newPObject(0); // for global (inter-pobj) variable
}

static int
valm_set0(int poid, int type, int symid, int val)
{
  int buf[5] = {RENDCMD_SETSYMVAL, type, poid, symid, val};
  int retval = svdb_set(type, poid, symid, val);
  send2renderers((char *)buf, sizeof(int) * 5);
  return retval;
}

// master api
int
valm_set(int type, int symid, int val)
{
  int poid = getPOIDm();
  return valm_set0(poid, type, symid, val);
}

// master api: global variable value
int
valgm_set(int type, int symid, int val)
{
  return valm_set0(0, type, symid, val);
}

static int
valm_getval0(int poid, int typeid, int symid)
{
  //return svdb_getval(typeid, symid, symid);
  return svdb_getval(typeid, poid, symid);
}

int
valm_getval(int typeid, int symid)
{
  return valm_getval0(getPOIDm(), typeid, symid);
}

int
valgm_getval(int typeid, int symid)
{
  return valm_getval0(0, typeid, symid);
}

//==================================================

static pthread_cond_t valc_cond;
static pthread_mutex_t valc_mutex;

static int
valc_getval0(int poid, int typeid, int symid)
{
  pthread_mutex_lock(&valc_mutex);
  int retval = svdb_getval(typeid, poid, symid);
  while (-1 == retval) {
    pthread_cond_wait(&valc_cond, &valc_mutex);
    retval = svdb_getval(typeid, poid, symid);
  }
  pthread_mutex_unlock(&valc_mutex);
  return retval;
}

// client api
int
valc_getval(int typeid, int symid)
{
  int poid = getPOID();
  return valc_getval0(poid, typeid, symid);
}

// client api: global (inter-pluginObjects) variable
int
valgc_getval(int typeid, int symid)
{
  return valc_getval0(0, typeid, symid);
}

int
valc_valp(int typeid, int symid)
{
  int poid = getPOID();
  int retval = svdb_getval(typeid, poid, symid);
  //retval = svbuf_getval(povalarray[typeid][poid], symid);
  return retval != -1;
}

// client background api
void
rcmd_recvsymval(int fd)
{
  int retval[4];
  if (0 >= readforce(fd, (char *)retval, sizeof(int) * 4)) {
    cab_error(1, "cannot get symval command from master\n");
    exit(1);
  }
  pthread_mutex_lock(&valc_mutex);
  svdb_set(retval[0], retval[1], retval[2], retval[3]);
  pthread_cond_broadcast(&valc_cond);
  pthread_mutex_unlock(&valc_mutex);
  usleep(0);
}

void
valc_init(void)
{
  svdb_init();
  svdb_newPObject(0); // for global (inter-pobj) variable
  pthread_cond_init(&valc_cond, 0);
  pthread_mutex_init(&valc_mutex, 0);
  rbase_regrcmd(RENDCMD_SETSYMVAL, rcmd_recvsymval);
}
