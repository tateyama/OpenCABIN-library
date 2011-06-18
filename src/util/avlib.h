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
#include <string.h>
#include <stdlib.h>

struct strbuf {
  int max;
  int used;
  char buf[0];
};

struct sbarray {
  int max;
  int used;
  struct strbuf *av[0];
};

struct sbspool {
  int max;
  //pthread_mutex_t mutex;
  struct strbuf *av[0];
};

struct avbuf {
  int max;
  int used;
  char *av[0];
};

extern int ceilbase2(int num);
struct strbuf *strbufnew(int size);

struct avbuf *avbuf_new(int maxnum);
void avbuf_appendStr(struct avbuf **abpptr, char *str);
void avbuf_clear(struct avbuf *abptr);
void split2avbuf(char *str, struct avbuf **retval);

void avbuf_init(struct avbuf *abptr);
void avbuf_realloc(struct avbuf **abpptr, int newmaxnum);


struct sbspool *sbspool_new(int maxnum);
void sbspool_realloc(struct sbspool **abpptr, int newmaxnum);

struct strbuf *strbufstrcpy(struct sbspool **spptr, struct strbuf *dst, char *str);
struct strbuf *strbufstrcat(struct strbuf *dst, char *str);

struct avbuf *avdeepcopy2(int acin, char **avin);
struct avbuf *avdeepcopy(struct avbuf *avbufptr);

#include "myexit.h"
