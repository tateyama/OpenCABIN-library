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

// linked list pattern
// if you want the ll to be callback-queue deletable, you must offer sacrifice a top element for speed optimization.

#include <callbackll.h>

static void callback_free(struct callback *ptr);
static struct callback *callback_new(void);

#define CALLBACKALLOCUNIT 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct callback *callbackfree = 0;
static int callbacknum = 0;
static void callback_delete_struct(struct callback *prev);

// return prev callback
static struct callback *
callback_search_internal(struct callback *elist, int key)
{
  struct callback *callbackptr;

  for (; elist->next; elist = elist->next) {
    callbackptr = elist->next;
    if (key == callbackptr->key) {
      return elist;
    }
  }

  return (struct callback *)0; // not found
}

// return condition: found (0) or not (1).
int
callback_search(void **list, int key, CALLBACKDATA *data)
{
  struct callback *elist = (struct callback *)list;
  struct callback *prev = callback_search_internal(elist, key);
  if (0 == prev) {
    return 1; // not found
  }
  *data = prev->next->data;
  return 0;
}

void
callback_insert(void **list, int key, CALLBACKDATA data)
{
  struct callback *callbackptr = callback_new();
  struct callback *elist = (struct callback *)list;

  callbackptr->key = key;
  callbackptr->data = data;
  callbackptr->next = elist->next;
  elist->next = callbackptr;
}

int
callback_delete(void **list, int key, CALLBACKDATA data)
{
  struct callback *elist = (struct callback *)list;
  struct callback *prev = callback_search_internal(elist, key), *target;

  if (0 == prev) {
    return 1; // not found
  }
#if 0
  target = prev->next;
  prev->next = target->next;
  callback_free(target);
#else
  callback_delete_struct(prev);
#endif
  return 0;
}

void
callback_foreach(void **list, void (*f)(int key, CALLBACKDATA d, void *val), void *val)
{
  struct callback *prev = (struct callback *)list, *eptr;

  if (0 == prev) {
    return;
  }
  for (; prev->next; prev = prev->next) {
    eptr = prev->next;
    (*f)(eptr->key, eptr->data, val);
  }
};

#ifdef STANDALONE

void
test(int key, CALLBACKDATA d, void *val)
{
  printf("%d %s %s\n", key, (char *)d, (char *)val);
}

int
main(int ac, char **av)
{
  void *list;
  char *data;

  callback_sysinit();
  list = 0;
  callback_insert(&list, 10, "testdata");
  callback_insert(&list, 20, "abc");
  if (1 == callback_search(&list, 10, (CALLBACKDATA) &data)) {
    fprintf(stderr, "not found\n");
    return 1;
  }
  callback_foreach(&list, test, "phase_1");

  if (1 == callback_delete(&list, 10, 0)) {
    fprintf(stderr, "10 not found\n");
    return 1;
  }

  callback_foreach(&list, test, "phase_2");

  if (1 == callback_delete(&list, 20, 0)) {
    fprintf(stderr, "20 not found\n");
    return 1;
  }

  callback_foreach(&list, test, "phase_3");
  fprintf(stderr, "success\n");
  return 0;
}
#endif

//==================================================
// memory management

struct callbackbuf {
  struct callbackbuf *next;
  struct callback buf[0];
};

static struct callbackbuf *callbackbuftop;

static void
callbackbuf_alloc(int num)
{
  int i, size;
  struct callbackbuf *target;
  struct callback *eptr;

  // at least 16 callbacks.
  if (num < 16) {
    num = 16;
  }

  // allocate
  size = sizeof (struct callbackbuf) + sizeof(struct callback) * num;
  if (NULL == (target = (struct callbackbuf *)malloc(size))) {
    perror("malloc()");
    exit(1);
  }

  callbacknum += num;

  memset((void *)target, 0, size);

  // link all new callbacks
  eptr = &target->buf[0];
  for (i = 0; i < (num - 1); ++i) {
    eptr[i].next = &eptr[i + 1];
  }

  // register to the callbackfree.
  if (0 == callbackfree) {
    callbackfree = &eptr[0];
    return;
  }

  // append to the callbackfree tail.
  for (eptr = callbackfree; eptr->next; eptr = eptr->next) {};
  eptr->next = &eptr[0];
  return;
}

static void
callbackbuf_init(void)
{
  callbackbuf_alloc(CALLBACKALLOCUNIT);
}

static int
callbackbuf_expand()
{
  callbackbuf_alloc(callbacknum);
}

static void
callback_delete_struct(struct callback *prev)
{
  struct callback *tmp;

  if (0 == prev) {
    fprintf(stderr, "FATAL ERROR; callback_delete(): Illegal deletion, pattern 0 (cancelled).\n");
    //exit(1);
    return;
  }

  tmp = prev->next;

  if (0 == tmp) {
    fprintf(stderr, "FATAL ERROR; callback_delete(): Illegal deletion, pattern 1 (cancelled).\n");
    //exit(1);
    return;
  }

  prev->next = tmp->next;
  callback_free(tmp);
}

// exported API

void
callback_sysinit(void)
{
  callbackbuf_init();
}

static void
callback_free(struct callback *ptr)
{
  ptr->next = callbackfree;
  callbackfree = ptr;
}

static struct callback *
callback_new(void)
{
  struct callback *retval;
  if (0 == callbackfree) {
    callbackbuf_expand();
  }
  retval = callbackfree;
  callbackfree = retval->next;
  retval->next = 0;
  return retval;
}
