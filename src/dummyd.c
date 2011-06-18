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
#include <ocl.h>

typedef float POS[6];

externSHVAR(float *, eyepos);
//staticSHVAR(float [], eyepos);

void *
minit(int ac, char **av)
{
  int usec = 700000;
  extern int sio_fd;
  unsigned char err;
  extern int testint;

  fprintf(stderr, "dummyd: minit()\n");

  return 0;
}

#if 0
void
midle(void *mprocdata)
{
}
#endif

void
mrepeat(void *mprocdata)
{
  eyepos[0] = eyepos[0] + 0.1f;
  SHvarVdml_sendls(&_SHVARID(eyepos)); // SHVARUPDATEVDML(eyepos);
  //fprintf(stderr, "dummyd: mrepeat\n");
  sleep(1);
}

void
mcleanup(void *mprocdata)
{
  fob_exit();
  fprintf(stderr, "fobd: mcleanup()\n");
}
