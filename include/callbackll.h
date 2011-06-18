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

#ifndef DEFINE_CALLBACKDATA
typedef void *CALLBACKDATA;
//typedef int CALLBACKDATA;
#define DEFINE_CALLBACKDATA
#endif

struct callback {
  struct callback *next; // next must be at top position in this struct.
  int key;
  CALLBACKDATA data;
};

void callback_sysinit(void);
int callback_delete(void **list, int key, CALLBACKDATA data);
void callback_insert(void **list, int key, CALLBACKDATA data);
void callback_foreach(void **list, void (*f)(int key, CALLBACKDATA d, void *val), void *val);

// return condition: found (0) or not (1).
int callback_search(void **list, int key, CALLBACKDATA *data);
