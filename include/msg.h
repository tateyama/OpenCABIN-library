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

struct msgqueue {
  struct msgptr *head;
  struct msgptr *tail;
  pthread_mutex_t mutex;
};

struct msgptr {
  int type;
  struct avbuf *avbuf;
  struct msgptr *next;
};

struct mop;

void msgptr_init(int num);
void msgq_init(struct msgqueue *ptr);
void msgq_lock(struct msgqueue *ptr);
void msgq_unlock(struct msgqueue *ptr);
//void msgq_push(struct msgqueue *queue, struct msgptr *msg);
struct msgptr *msgq_shift(struct msgqueue *queue);
//void mop_msgq_internal(struct mop *moppptr, int ac, unsigned char **av);
void msgq_push(struct msgqueue *queue, int ac, unsigned char **av);
