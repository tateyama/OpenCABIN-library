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
#include "avlib.h"

struct strbuf *strbufnew2(int size);

// ==================================================
int
ceilbase2(int num)
{
  int i = 8;
  while (i < num) {
    i <<= 1;
  }
  return i;
}

// ==================================================
// strbufspool

void
sbspool_init(struct sbspool *spool)
{
  //sbarray_init(spool);
  int i;

  memset(&(spool->av[0]), 0, sizeof(char *) * spool->max);
}

struct sbspool *
sbspool_new(int maxnum)
{
  struct sbspool *retval;

  if (maxnum < 8) maxnum = 8;
  maxnum = ceilbase2(maxnum);
  if (NULL == (retval = (struct sbspool *)malloc(sizeof(struct sbspool)
						 + maxnum * sizeof(struct strbuf *)))) {
    perror("malloc()");
    myexit(1);
  }

  retval->max = maxnum;
  sbspool_init(retval);
  return retval;
}

struct strbuf *
sbspool_get(struct sbspool *spool, int size)
{ 
  int  i;
  int j = -1;
  struct strbuf *retval;

  for (i = 0; i < spool->max; ++i) {
    if (0 == spool->av[i]) continue;
    if (spool->av[i]->max > size) {
      if ((-1 == j) || (spool->av[j]->max > spool->av[i]->max)) {
	j = i;
      }
    }
  }
  if (-1 != j) {
    retval = spool->av[j];
    spool->av[j] = 0;
    return retval;
  }
  return strbufnew2(size);
}

void
sbspool_realloc(struct sbspool **abpptr, int newmaxnum)
{
  struct sbspool *newptr;
  int i;
  
  newptr = (struct sbspool *)realloc(*abpptr, sizeof(struct sbspool) + newmaxnum * sizeof(struct strbuf *));
  
  if (NULL == newptr) {
    perror("realloc()");
    myexit(1);
  }

  for (i = newptr->max; i < newmaxnum; ++i) {
    newptr->av[i] = 0;
  }

  newptr->max = newmaxnum;

  *abpptr = newptr;
}



void
sbspool_put(struct sbspool **spptr, struct strbuf *sb)
{
  int i;
  struct sbspool *spool = *spptr;

  for (i = 0; i < spool->max; ++i) {
    if (0 == spool->av[i]) {
      spool->av[i] = sb;
      return;
    }
  }
  // realloc sbarray
  i = spool->max;
  sbspool_realloc(spptr, i + 1);
  spool = *spptr;
  spool->av[i] = sb;
  //strbufinit(sb);
}

//==================================================
// strbuf

void
strbufinit(struct strbuf *sbptr)
{
  int i;
  sbptr->used = 0;

  for (i = 0; i < sbptr->max; ++i) {
    sbptr->buf[i] = 0;
  }
}

struct strbuf *
strbufnew(int size)
{
  struct strbuf *retval;

  if (NULL == (retval = (struct strbuf *)malloc(sizeof(struct strbuf) + size))) {
    perror("malloc()");
    myexit(1);
  }

  retval->max = size;
  strbufinit(retval);
  return retval;
}

struct strbuf *
strbufnew2(int size)
{
  return strbufnew(ceilbase2(size));
}

struct strbuf *
strbufstrcpy(struct sbspool **spptr, struct strbuf *dst, char *str)
{
  struct strbuf *freesb;
  int len = strlen(str) + 1;

  if (dst == 0) {
    dst = sbspool_get(*spptr, len);
  } else {
    if (dst->max < len) {
      freesb = dst;
      dst = sbspool_get(*spptr, len);
      sbspool_put(spptr, freesb);
    }
  }
  memcpy(dst->buf, str, len);
  dst->used = len;
  return dst;
}

// ==================================================

void
avbuf_init(struct avbuf *abptr)
{
  int i;

  abptr->used = 0;

  for (i = 0; i < abptr->max; ++i) {
    abptr->av[i] = 0;
  }
}

struct avbuf *
avbuf_new(int maxnum)
{
  struct avbuf *retval;

  if (0 == maxnum) maxnum = 8;

  if (NULL == (retval = (struct avbuf *)malloc(sizeof(struct avbuf)
					       + maxnum * sizeof(char *)))) {
    perror("malloc()");
    myexit(1);
  }

  retval->max = maxnum;
  avbuf_init(retval);
  return retval;
}

void
avbuf_realloc(struct avbuf **abpptr, int newmaxnum)
{
  struct avbuf *newptr;
  int i;
  
  newptr = realloc(*abpptr, sizeof(struct avbuf) + newmaxnum * sizeof(char *));

  if (NULL == newptr) {
    perror("realloc()");
    myexit(1);
  }

  for (i = newptr->max; i < newmaxnum; ++i) {
    newptr->av[i] = 0;
  }

  newptr->max = newmaxnum;

  *abpptr = newptr;
}

void
avbuf_appendStr(struct avbuf **abpptr, char *str)
{
  struct avbuf *abptr = *abpptr;

  if (abptr->max <= abptr->used) {
    avbuf_realloc(abpptr, ceilbase2(abptr->max + 1));
  }
  abptr = *abpptr;
  abptr->av[abptr->used] = str;
  abptr->used += 1;
  
}

void
avbuf_clear(struct avbuf *abptr)
{
  int i;

  for (i = 0; i < abptr->max; ++i) {
    abptr->av[i] = 0;
  }
  abptr->used = 0;
}

// ==================================================


void
sbarray_init(struct sbarray *abptr)
{
  int i;

  abptr->used = 0;

  for (i = 0; i < abptr->max; ++i) {
    abptr->av[i] = 0;
  }
}

struct sbarray *
sbarray_new(int maxnum)
{
  struct sbarray *retval;

  if (NULL == (retval = (struct sbarray *)malloc(sizeof(struct sbarray)
					       + maxnum * sizeof(struct strbuf*)))) {
    perror("malloc()");
    myexit(1);
  }

  retval->max = maxnum;
  sbarray_init(retval);
  return retval;
}

void
sbarray_avdup(char **av, struct sbarray *retav)
{
  int i;

  for (i = 0; *av; ++i) {
  }
}

void
sbarray_copy(char **av)
{
}

void
sbarray_realloc(struct sbarray **abpptr, int newmaxnum)
{
  struct sbarray *newptr;
  int i;
  
  newptr = realloc(*abpptr, sizeof(struct sbarray) + newmaxnum * sizeof(struct strbuf *));

  if (NULL == newptr) {
    perror("realloc()");
    myexit(1);
  }

  for (i = newptr->max; i < newmaxnum; ++i) {
    newptr->av[i] = 0;
  }

  newptr->max = newmaxnum;

  *abpptr = newptr;
}

void
sbarray_appendStr(struct sbspool **spptr, struct sbarray **abpptr, char *str)
{
  struct sbarray *abptr = *abpptr;
  struct strbuf *sbptr;

  if (abptr->max <= abptr->used) {
    sbarray_realloc(abpptr, ceilbase2(abptr->max + 1));
  }
  sbptr = strbufstrcpy(spptr, 0, str);
  abptr = *abpptr;
  abptr->av[abptr->used] = sbptr;
  abptr->used += 1;
  abptr->av[abptr->used] = 0; // ?
}

void
sbarray_clear(struct sbspool **spptr, struct sbarray *abptr)
{
  int i;

  for (i = 0; i < abptr->used; ++i) {
    sbspool_put(spptr, abptr->av[i]);
    abptr->av[i] = 0;
  }
  abptr->used = 0;
}

// split2avbuf() destructs contents of the str.
void
split2avbuf(char *str, struct avbuf **retval)
{
  char *arg = 0;
  int state = 0;

  avbuf_clear(*retval);

  while (1) {
    switch (*str) {
    case ' ': case '\t': case '\r':
      if (0 != state) {
	*str = 0;
	state = 0;
      }
      break;
    case '\n': case 0:
      // end
      *str = 0;
      if (0 != arg) {
	avbuf_appendStr(retval, arg);
      }
      return;
      break;
    default:
      if (0 == state) {
	// new arg, save old arg 
	if (0 != arg) {
	  avbuf_appendStr(retval, arg);
	}
	arg = str;
	state = 1;
      }
      break;
    }
    ++str;
  }
}

struct avbuf *
avdeepcopy2(int acin, char **avin)
{
  int size, size0, i, s;
  char **avout, *bufptr, *buf;
  struct avbuf *retval;

  // av deep copy
  size0 = (acin + 1) * sizeof(char *) + sizeof(struct avbuf);
  size = size0;
  for (i = 0; i < acin; ++i) {
    size += strlen(avin[i]) + 1;
  }

  if (NULL == (retval = (struct avbuf *)malloc(size))) {
    cab_error(1, "cmd_runapp: cannot malloc()\n");
    exit(1);
  }

  bufptr = (char *)&(retval->av[acin + 1]);
  avout = (char **)&(retval->av[0]);

  for (i = 0; i < acin; ++i) {
    avout[i] = bufptr;
    s = strlen(avin[i]) + 1;
    memcpy(bufptr, avin[i], s);
    bufptr += s;
  }

  avout[acin + 1] = 0;
  retval->max = acin;
  retval->used = acin;

  return retval;
}

struct avbuf *
avdeepcopy(struct avbuf *avbufptr)
{
  return avdeepcopy2(avbufptr->used, avbufptr->av);
}

#ifdef STANDALONE
int
main(int ac, char **av)
{
  char buf[BUFSIZ];
  FILE *ifp = stdin;
  struct avbuf *abptr = avbuf_new(32);
  int i;
  
  
  while (NULL != fgets(buf, BUFSIZ, ifp)) {
    split2avbuf(buf, &abptr);
    for (i = 0; i < abptr->used; ++i) {
      printf("\"%s\" ", abptr->av[i]);
    }
    putchar('\n');
  }
  return 0;
}
#endif
