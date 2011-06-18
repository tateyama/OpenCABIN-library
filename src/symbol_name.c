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
// symbol_name.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cabprot.h"

#define STANDALONE

static char **syment = 0;
static int syment_size = 1024;
static int syment_cur = 0;

static char *symentbuf = 0;
static int symentbuf_size = 1024;
static int symentbuf_cur = 0;

//==================================================
// master implementation

//--------------------------------------------------

static void
symentbuf_expand()
{
#if 0
  symentbuf_size *= 2;
#if 0
  // strbuf cannot move
  if (NULL == (symentbuf = realloc(symentbuf_size))) {
    perror("malloc()");
    exit(1);
  }
#else
  if (NULL == (symentbuf = malloc(symentbuf_size))) {
    perror("malloc()");
    exit(1);
  }
  symentbuf_cur = 0;
#endif
#else
  int new_symentbuf_size = 2 * symentbuf_size;
  if (NULL == (symentbuf = realloc(symentbuf, symentbuf_size))) {
    perror("malloc()");
    exit(1);
  }
  memset(symentbuf + symentbuf_size, 0, new_symentbuf_size - symentbuf_size);
  symentbuf_size = new_symentbuf_size;
#endif
}

static void
symentbuf_init(int n)
{
  if (n > 0) {
    symentbuf_size = n;
  }

  if (NULL == (symentbuf = malloc(symentbuf_size))) {
    perror("malloc()");
    exit(1);
  }
  memset(symentbuf, 0, symentbuf_size);
}

static char *
symentbuf_register(char *str)
{
  int len = strlen(str) + 1;
  char *dst;

  if ((symentbuf_cur + len) > symentbuf_size) {
    symentbuf_expand(); // <- [TODO] symentbuf_expand(len);
  }
  dst = symentbuf + symentbuf_cur;
  memcpy(dst, str, len);
  symentbuf_cur += len;
  return dst;
}

//--------------------------------------------------

static void
syment_expand(void)
{
  int new_syment_size = 2 * syment_size;
  int size = new_syment_size * sizeof(char *);

  if (NULL == (syment = (char **)realloc(syment, size))) {
    perror("malloc()");
    exit(1);
  }

  memset(((char *)syment) + syment_size * sizeof(char *), 0, (new_syment_size - syment_size) * sizeof(char *));
  syment_size = new_syment_size;
}

static void
syment_expand1(int id)
{
  int new_syment_size = syment_size;
  while (id >= new_syment_size) {
    new_syment_size *= 2;
  }
  if (NULL == (syment = realloc(syment, sizeof(char *) * new_syment_size))) {
    perror("malloc()");
    exit(1);
  }
  memset(((char *)syment) + syment_size * sizeof(char *), 0, (new_syment_size - syment_size) * sizeof(char *));
  syment_size = new_syment_size;
}

static void
syment_init(int n)
{
  int size = sizeof(char *) * syment_size;
  //fprintf(stderr, "syment_init()\n");
  if (n > 0) {
    syment_size = n;
  }
  if (NULL == (syment = malloc(size))) {
    perror("malloc()");
    exit(1);
  }
  memset(syment, 0, size);
}

static int
syment_query(char *str)
{
  int i;
  char **ptr = syment;
  for (i = 0; i < syment_cur; ++i) {
    if (!strcmp(str, *ptr)) {
      return i + 1;
    }
    ++ptr;
  }
  return 0;
}

static int
syment_register(char *str)
{
  char *buf;

  //fprintf(stderr, "syment_register: len = %d\n", strlen(str));

  buf = symentbuf_register(str);

  if (syment_cur >= syment_size) {
    syment_expand();
  }

  while (syment[syment_cur] != 0) {
    ++syment_cur;
    if (syment_cur >= syment_size) {
      syment_expand();
    }
  }
  syment[syment_cur] = buf;
  ++syment_cur;
  return syment_cur; 
}

static int
syment_register2(int id, char *str)
{
  char *buf;
  int id1 = id - 1;

  if (id1 >= syment_size) {
    syment_expand1(id1);
  } else {
    if (syment[id1] != 0) {
      cab_error(1, "symname_register(id = %d, string = %s): id = %d was %s\n", id, str, id, syment[id1]);
      exit(1);
    }
  }

  buf = symentbuf_register(str);
  syment[id1] = buf;
  syment_cur = (syment_cur > id) ? syment_cur : id;
  return id; 
}

static char *
syment_get(int id)
{
  return syment[id - 1];
}

// client implementation

//==================================================
//  symbol name api 
//--------------------------------------------------

// symbol name master api
int
snm_init(void)
{
  symentbuf_init(0);
  syment_init(0);
}

#if 0
void
printsyment(void)
{
  fprintf(stderr, "syment = %x\n", syment);
}
#endif

int
snm_intern(char *str)
{
  int id;

  id = syment_query(str);

  //fprintf(stderr, "snm_intern(%s), syment = %x \n", str, syment);
  if (0 == id) {
    int msg[3] = { RENDCMD_SYMNAM, id, 0};
    int len = strlen(str);
    id = syment_register(str);
    //broadcast_to_client(str, id);
    msg[1] = id;
    msg[2] = len;
    send2renderers((char *)msg, sizeof(int) * 3);
    send2renderers(str, len);
  }
  return id;
}

int snc_init(void);
int snc_query(char *str);
void rcmd_newsymnam(int fd);

// symbol name client api

static pthread_cond_t newsym_cond;
static pthread_mutex_t newsym_mutex;

int
snc_init()
{
  symentbuf_init(0);
  syment_init(0);
  pthread_cond_init(&newsym_cond, 0);
  pthread_mutex_init(&newsym_mutex, 0);
  rbase_regrcmd(RENDCMD_SYMNAM, rcmd_newsymnam);
}

// called by callback
void
snc_newsymnam(int id, char *str)
{
  pthread_mutex_lock(&newsym_mutex);
  syment_register2(id, str);
  //fprintf(stderr, "snc_newsym: arrived\n");
  // wakeup waiting clients();
  pthread_cond_broadcast(&newsym_cond);
  pthread_mutex_unlock(&newsym_mutex);
  //fprintf(stderr, "snc_newsymnam: unlocked\n");
  usleep(0);
  //fprintf(stderr, "snc_newsymnam: end\n");
}

int
snc_query(char *str)
{
  int id;
  //fprintf(stderr, "snc_query: try to get lock\n");
  pthread_mutex_lock(&newsym_mutex);
  //fprintf(stderr, "snc_query: got lock\n");
  if (0 == (id = syment_query(str))) {
    do {
      // sleep_until_newsymbol;
      // I can stop this thread, even if other applications stop!
      // Otherwise I cannot get correct rendering result.
      // fprintf(stderr, "snc_query: symwaiting\n");
      pthread_cond_wait(&newsym_cond, &newsym_mutex);
      //fprintf(stderr, "snc_query: come\n");
      id = syment_query(str);
    } while (0 == id);
  }
  pthread_mutex_unlock(&newsym_mutex);
  return id;
}

void
rcmd_newsymnam(int fd)
{
  int id, len;
  char *str;

  if (0 >= readforce(fd, (char *)&id, sizeof(id))) {
    cab_error(1, "cannot get symnam command from master\n");
    exit(1);
  }

  if (0 >= readforce(fd, (char *)&len, sizeof(len))) {
    cab_error(1, "cannot get symnam command from master\n");
    exit(1);
  }

  if (NULL == (str = alloca(len + 1))) {
    cab_error(1, "fail to alloca() while proceeding symnam command from master\n");
    exit(1);
  }

  if (0 >= readforce(fd, (char *)str, len)) {
    cab_error(1, "cannot get symnam command from master\n");
    exit(1);
  }
  str[len] = 0;
  fprintf(stderr, "newsymname: %s\n", str);

  snc_newsymnam(id, str);
}
