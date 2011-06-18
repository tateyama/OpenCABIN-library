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
#include <oclconf.h>
#include <sio.h>

int
main(int ac, char **av)
{
  char *devfn = OCL_FOB_DEVICE;
  int fd = sio_init(devfn, OCL_FOB_SPEED);
  int retval;
  char buf;

  if (-1 == fd) {
    perror(devfn);
    exit(1);
  }

  buf = 0x3f; // STREAM STOP (Fob manual p.103)

  switch (retval = write(fd, &buf, 1)) {
  case 1:
    // OK
    break;
  case 0:
  case -1:
    fprintf(stderr, "cannot write stop command\n");
    perror(devfn);
    break;
  }

  return 0;
}
