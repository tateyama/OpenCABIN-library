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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pid.h>

struct pid {
  int type, id;
};

unsigned int pid_get(int type, int id);

static struct pid *table;
static int table_max = BUFSIZ;
static int table_cur = 0;

void
pid_init(int num)
{
  if (num > table_max) {
    table_max = num;
  }
  if (NULL == (table = calloc(num, sizeof(struct pid)))) {
    perror("malloc()");
    exit(1);
  }
}

static void
table_realloc()
{
  int new_max = table_max * 2;

  if (NULL == (table = realloc((void *)table, new_max * sizeof(struct pid)))) {
    perror("realloc()");
    exit(1);
  }
  memset((void *)(&table[table_max]), 0, sizeof(struct pid) * (new_max - table_max));
  table_max = new_max;
}

unsigned int
pid_get(int type, int id)
{
  unsigned int i;
  for (i = 0; i < table_cur; ++i) {
    if ((table[i].id == id) && (table[i].type == type)) {
      return i;
    }
  }
  // no such type & id.
  return 0;
}

int
pid_gettypeid(int pid, int *idptr)
{
  if (idptr) {
    *idptr = table[pid].id;
  }
  return table[pid].type;
}

unsigned int
pid_new(int type, int local_id)
{
  int i;
#if 1
  if (table_cur >= table_max) {
    for (i = 0; i < table_cur; ++i) {
      if (table[i].id == 0) {
	table[i].id = local_id;
	table[i].type = type;
	return i;
      }
    }
    // no room
    table_realloc();
  }
  table[table_cur].id = local_id;
  table[table_cur].type = type;
  return table_cur++;
#else
  for (i = 0; i < table_cur; ++i) {
    if (table[i].id == 0) {
      table[i].id = local_id;
      table[i].type = type;
      return i;
    }
  }
  if (table_cur >= table_max) {
    table_realloc();
  }
  table[table_cur].id = local_id;
  table[table_cur].type = type;
  return table_cur++;
#endif
}

void
pid_delete(int type, int id)
{
  unsigned int pid;

  pid = pid_get(type, id);
  table[pid].type = 0;
  table[pid].id = 0;
}

void
pid_sendsignal(int i, int sig)
{
  
}

int
pid_num(void)
{
  int count = 0, i;

  for (i = 0; i < table_cur; ++i) {
    if (table[i].id == 0) continue; // skip;
    //printf("pid = %d, type = %d; local_id = %d\n", i, table[i].type, table[i].id);
    ++count;
  }
  return count;
}

int
oclint_pid_foreach(int (*cbfunc)(int pid, int type_id, int local_id))
{
  int i;

  for (i = 0; i < table_cur; ++i) {
    if (table[i].id == 0) continue; // skip;
    (*cbfunc)(i, table[i].type, table[i].id);
    //printf("pid = %d, type = %d; local_id = %d\n", i, table[i].type, table[i].id);
  }
}

void
pid_printlist()
{
}
