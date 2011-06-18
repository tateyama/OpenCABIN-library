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
// vdml.c
// Dynamic Medium loSsy shared Value
// VVVVEEERRRRYYY SSSLLLOOOWWWEERRR but safer implementation

// vdml

#define MAXVDMLSIZE (4096 - sizeof(int))
#include <stdio.h>
#include <stdlib.h>
#include <cabprot.h>
#include <string.h>
#include <pthread.h>
#include <sys/errno.h>
#include "vdml.h"
#include <tatemcast.h>

struct vdml_packet {
  int index;
  unsigned char buf[0];
};


struct vdmlbuf {
  int size;
  int current;
  struct vdml **buf;
};

static struct vdml *vdml_new(int idx, int size);

static struct lcm g_lcm;
static struct vdmlbuf vdmlbuftop;
static short g_port = 28979;
static struct vdml_packet *g_recvbuf;
static int g_recvbuf_size;

static void
vdmlbuf_init(int num)
{
  if (NULL == (vdmlbuftop.buf = (struct vdml **)calloc(1, sizeof(struct vdml *) * num))) {
    fprintf(stderr, "vdmlbuf_init(): ");
    perror("malloc()");
    exit(1);
  }
  vdmlbuftop.size = num;
  vdmlbuftop.current = 0;
}

//==================================================


static void
vdml_init(int num)
{
  if (num <= 0) num = 4096;
  vdmlbuf_init(num);
}

void
vdmlm_init(int num, short port)
{
  if (port) {
    g_port = port;
  }

  vdml_init(num);
  lcm_init(&g_lcm, g_port);
}

// lossless send (master)
void
vdmlm_sendll(int idx)
{
  struct vdml *ptr = vdmlm_getobj(idx);
  int header[3] = {RENDCMD_VDML, 0, 0};
  header[1] = ptr->id;
  header[2] = ptr->size;
  send2renderers((char *)header, sizeof(header));
  send2renderers((char *)ptr->buf, ptr->size); // send via TCP at first (init?)
};

void
vdmlm_updatell(int idx)
{
  struct vdml *ptr = vdmlm_getobj(idx);
  int header[2] = {RENDCMD_VDMLUPDATE, 0};
  header[1] = ptr->id;
  send2renderers((char *)header, sizeof(header));
  send2renderers((char *)ptr->buf, ptr->size); // send via TCP at first (init?)
};

// lossy send (master)
void
vdmlm_sendls(int idx)
{
  struct vdml *ptr = vdmlm_getobj(idx);
  
  lcm_send(&g_lcm, (unsigned char *)ptr + sizeof(int), ptr->size + sizeof(int));
}


static void vdmlc_readloop(void *);
static void vdmlc_recvbuf_init(int);
static void vdmlc_init_thr(void);
static struct lcc g_lcc;

static pthread_cond_t ccond;
static pthread_mutex_t cmutex;

// master function: void vdmlm_sendll(int idx)
void
rcmd_vdmlc_new_callback(int fd)
{
  // with
  int data[2]; // id, size
  struct vdml *vdmlptr;
  if (0 >= readforce(fd, (char *)data, sizeof(data))) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }

  pthread_mutex_lock(&cmutex);
  vdmlptr = vdmlbuftop.buf[data[0]];
  if (!vdmlptr) {
    vdmlptr = vdml_new(data[0], data[1]);
  }
  if (0 >= readforce(fd, (char *)vdmlptr->buf, vdmlptr->size)) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }
  pthread_cond_broadcast(&ccond);
  pthread_mutex_unlock(&cmutex);
}

void
rcmd_vdmlc_contents_callback(int fd)
{
  int id;
  struct vdml *vdmlptr;

  if (0 >= readforce(fd, (char *)&id, sizeof(id))) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }

  //printf("vdmlc_contents_callback: %d\n", id);
  pthread_mutex_lock(&cmutex);
  vdmlptr = vdmlbuftop.buf[id]; // DANGEROUS !!
  if (0 >= readforce(fd, (char *)vdmlptr->buf, vdmlptr->size)) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }
  pthread_cond_broadcast(&ccond);
  pthread_mutex_unlock(&cmutex);
}

void
vdmlc_init(int num, short port)
{
  if (port) {
    g_port = port;
  }
  vdml_init(num);
  vdmlc_recvbuf_init(BUFSIZ);
  lcc_init(&g_lcc, g_port, vdmlc_readloop); // the vdml contents receiving thread
  vdmlc_init_thr();
  rbase_regrcmd(RENDCMD_VDML, rcmd_vdmlc_new_callback); // <- new vdml callback function
  rbase_regrcmd(RENDCMD_VDMLUPDATE, rcmd_vdmlc_contents_callback); // <- lossless vdml contents callback function
}

static struct vdml *
vdml_new(int idx, int size)
{
  struct vdml *vdmlptr;

  if (NULL == (vdmlptr = (struct vdml *)malloc(size + sizeof(struct vdml)))) {
    perror("vdml_new(): malloc()");
    exit(1);
  }

  vdmlptr->size = size;
  vdmlptr->id = idx;

  memset(vdmlptr->buf, 0, size);

  vdmlbuftop.buf[idx] = vdmlptr;

  return vdmlptr;
}

//==================================================

static void
vdmlc_recvbuf_init(int size)
{
  if (0 >= size) {
    size = BUFSIZ;
  }

  g_recvbuf_size = sizeof(struct vdml_packet) + size;

  if (NULL == (g_recvbuf = (struct vdml_packet *)malloc(g_recvbuf_size))) {
    fprintf(stderr, "ERROR: in vdmlc_recvbuf_init(), malloc() returns %d (%s)\n", g_recvbuf, strerror(errno));
    exit(1);
  }
}

// client network reading thread
static void
vdmlc_readloop(void *val)
{
  int retval, idx;
  struct vdml *vdmlptr;
  
  while (0 < (retval = mcread(g_lcc.fd, (unsigned char *)g_recvbuf, g_recvbuf_size))) {
    idx = g_recvbuf->index;
    //printf("vdmlc_readloop(): id: %d\n", idx);
    
    vdmlptr = vdmlbuftop.buf[idx];
    if (vdmlptr == 0) {
      fprintf(stderr, "vdmlc_read error: no such index %d (%d bytes packet)\n", idx, retval);
      continue;
    }
    if ((retval - sizeof(struct vdml_packet)) != vdmlptr->size) {
      fprintf(stderr, "vdmlc_read error: size mismatch %d mustbe %d (id: %d, idx: %d)\n", retval - sizeof(struct vdml_packet),
	      vdmlptr->size, vdmlptr->id, idx);
      continue;
    }
    memcpy(vdmlptr->buf, g_recvbuf->buf, vdmlptr->size);
    //if (idx == 3) printf("vdmlptr: %x\n", vdmlptr->buf[0]);
    // TODO: kick application or set an updated flag
  }
  fprintf(stderr, "vdml_read() returns %d\n", retval);
  exit(1);
}

//
static void
vdmlc_init_thr(void)
{
  pthread_mutex_init(&cmutex, 0);
  pthread_cond_init(&ccond, 0);
}

unsigned char *
vdmlc_getbuf(int idx)
{
  struct vdml *ptr;
  pthread_mutex_lock(&cmutex);
  while (0 == (ptr = vdmlbuftop.buf[idx])) {
    pthread_cond_wait(&ccond, &cmutex);
    ptr = vdmlbuftop.buf[idx];
  }
  pthread_mutex_unlock(&cmutex);
  return ptr->buf;
}

struct vdml *
vdmlm_getobj(int idx)
{
  return vdmlbuftop.buf[idx];
}

unsigned char *
vdmlm_getbuf(int idx)
{
  struct vdml *ptr = vdmlbuftop.buf[idx];
  if (!ptr) return 0;
  return ptr->buf;
}


// ==================================================
// master API

static int g_vdml_id = 0;

// return: value-id

// double buffer?
int
vdmlm_id_new(void)
{
  return ++g_vdml_id;
}


#if 0
static struct vdml *
vdml_malloc(int size)
{
  int rsize = ((size + 3) / 4) * 4 + sizeof(struct vdml_);

}
#endif

int
vdmlm_new(int size, struct vdml **pptr)
{
  int id;
  struct vdml *vdmlptr;

  if (MAXVDMLSIZE < size) {
    fprintf(stderr, "vdmlm_new(%d): size must be less than %d", size, MAXVDMLSIZE);
    return -1;
  }

  if (0 > (id = vdmlm_id_new())) {
    return -1;
  }

  vdmlptr = vdml_new(id, size);

  if (pptr) {
    *pptr = vdmlptr; // return buffer ?
  };
  return id;
}

int
vdmlm_put(int id)
{
  
}

void
vdmlm_flush(int id)
{
  
}

// ==================================================
// master & client internal API

int
vdml_alloc(int id, int size)
{
}

char *
vdml_getbuf(int id)
{
  
}

// ==================================================
// client API

void
vdmlc_update(void)
{
}

void
vdml_wait(int id)
{
}

void
vdml_registercb(int id, void (*f)())
{
  
}

void
rcmd_vdml_newbuf(int fd)
{
#if 0
  pthread_mutex_lock(&valc_mutex);
  svdb_set(retval[0], retval[1], retval[2], retval[3]);
  pthread_cond_broadcast(&valc_cond);
  pthread_mutex_unlock(&valc_mutex);
#endif
}
