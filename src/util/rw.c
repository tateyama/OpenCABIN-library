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
#include "myerror.h"

char *
mallocm(int size)
{
  char *retval;
  if (0 == (retval = malloc(size))) {
    cab_perror(1, "malloc(): ");
    return 0;
  }
  return retval;
}

/* force write */

int
writeforce(int fd, char *buf, int size)
{
  int retval, org = size;
  while (size > 0) {
    switch (retval = write(fd, buf, size)) {
    case 0:
      cab_error(1, "writeforce(): returns 0 (order %d bytes -> written: %d bytes)\n", org, org - size);
      return retval;
      break;
    case -1:
      cab_perror(1, "writeforce(order %d bytes -> written: %d bytes): ", org, org - size);
      return retval;
      break;
    default:
      buf += retval;
      size -= retval;
      break;
    }
  }
  return 1;
}

/* force read */
int
readforce(int fd, char *buf, int size)
{
  int retval, org = size;
  while (size > 0) {
    switch (retval = read(fd, buf, size)) {
    case 0:
      cab_error(1, "readforce(): returns 0 (order %d bytes -> read: %d bytes)\n", org, org - size);
      return 0;
      break;
    case -1:
      cab_perror(1, "readforce(order %d bytes -> read: %d bytes): ", org, org - size);
      return -1;
      break;
    default:
      buf += retval;
      size -= retval;
      break;
    }
  }
  return 1;
}

/* read with message */
int
readm(int fd, char *buf, int size)
{
  int retval, org = size;
  switch (retval = read(fd, buf, size)) {
  case 0:
      cab_error(1, "readm(): returns 0 (order %d bytes -> read: %d bytes)\n", size, size - retval);
      break;
  case -1:
    cab_perror(1, "readm(order %d bytes -> read: %d bytes): ", size, size - retval);
    break;
  }
  return retval;
}
