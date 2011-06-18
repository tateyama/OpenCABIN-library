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
#include <unistd.h>

extern short g_port;
int rbase_main(void);
void rbase_daemon(char **av);

int
main(int ac, char **av)
{
  int c;
  int daemonp = 0;

  while (EOF != (c = getopt(ac, av, "p:d"))) {
    switch (c) {
    case 'd':
      break;
    case 'p':
      g_port = atoi(optarg);
      break;
    }
  }
  if (daemonp) {
    rbase_daemon(av);
  }
  rbase_main();
  return 0;
}
