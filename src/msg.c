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
#include <pthread.h>
#include <avlib.h>
#include "msg.h"

static struct msgptr *msg_freetop;
static struct msgptr *msg_freecur;
static pthread_mutex_t msg_mutex;
static int msg_num = 1024;

void
msgptr_init(int num)
{
  int i;

  if (num > msg_num) {
    msg_num = num;
  }

  if (NULL == (msg_freetop = (struct msgptr *)malloc(msg_num * sizeof(struct msgptr)))) {
    perror("malloc()");
    exit(1);
  }

  for (i = 0; i < (msg_num - 1); ++i) {
    msg_freetop[i].next = &msg_freetop[i + 1];
  }
  msg_freetop[msg_num - 1].next = 0;
  msg_freecur = msg_freetop;
  pthread_mutex_init(&msg_mutex, 0);
}

static struct msgptr *
msgptr_new(void)
{
  struct msgptr *retval = msg_freecur;
  int i;

  if (0 == retval) {
    struct msgptr *newtop;
    int newnum = msg_num * 2;
    if (NULL == (newtop = realloc((void *)msg_freetop, newnum * sizeof(struct msgptr)))) {
      perror("realloc()");
      exit(1);
    }
    msg_freetop = newtop;
    for (i = msg_num; i < (newnum - 1); ++i) {
      msg_freetop[i].next = &msg_freetop[i + 1];
    }
    msg_freetop[newnum - 1].next = 0;
    msg_freecur = &msg_freetop[msg_num];
    msg_num = newnum;
  }
  pthread_mutex_lock(&msg_mutex);
  msg_freecur = retval->next;
  retval->next = 0;
  pthread_mutex_unlock(&msg_mutex);
  return retval;
}

void
msgptr_free(struct msgptr *ptr)
{
  pthread_mutex_lock(&msg_mutex);
  ptr->next = msg_freecur;
  msg_freecur = ptr;
  pthread_mutex_unlock(&msg_mutex);
}

// ==================================================
// msgq
// --------------------------------------------------
void
msgq_init(struct msgqueue *ptr)
{
  ptr->head = 0;
  ptr->tail = 0;
  pthread_mutex_init(&ptr->mutex, 0);
}

void
msgq_lock(struct msgqueue *ptr)
{
  pthread_mutex_lock(&ptr->mutex);
}

void
msgq_unlock(struct msgqueue *ptr)
{
  pthread_mutex_unlock(&ptr->mutex);
}

static void
msgq_push_internal(struct msgqueue *queue, struct msgptr *msg)
{
  if ((msg == 0) || (queue == 0)) {
    // ERROR
    fprintf(stderr, "msgptr_push_internal(): FATAL ERROR; the %s points zero\n", msg ? "message" : "mqueue");
    exit(1); // too strict?
    return;
  }
  msg->next = 0;
  if (queue->head == 0) {
    queue->head = msg;
  } else {
    queue->tail->next = msg;
  }
  queue->tail = msg;
}

struct msgptr *
msgq_shift(struct msgqueue *queue)
{
  struct msgptr *retval;
  if (queue == 0) {
    // ERROR
    fprintf(stderr, "msgptr_shift(): FATAL ERROR; the queue points zero\n");
    exit(1); // too strict?
    return;
  }
  //if (0 == *queue) return 0;
  retval = queue->head;
  queue->head = retval->next;
  if (retval->next == 0) {
    queue->tail = 0;
  }
  return retval;
}

void
msgq_push(struct msgqueue *queue, int ac, unsigned char **av)
{
  struct avbuf *abptr = avdeepcopy2(ac, (char **)av);
  struct msgptr *msg;
  int symid;

  symid = snm_intern(av[0]);
  msg = msgptr_new();
  msg->avbuf = abptr;
  msg->type = symid;
  msgq_lock(queue);
  msgq_push_internal(queue, msg);
  msgq_unlock(queue);
}
