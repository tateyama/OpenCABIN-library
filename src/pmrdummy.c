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
// pmrdummy.c
// plugin manager renderer dummy 
// for Master

#include <stdio.h>
#include <stdlib.h>
void
rbase_regrcmd(int cmdid, void (*func)())
{
  fprintf(stderr, "FATAL ERROR: master cannot call rbase_regrcmd()\n");
  exit(1);
}

int
getPOID(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call getPOID()\n");
  exit(1);
}

void
runidle(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call runidle()\n");
  exit(1);
}

void
oclint_app_drawfunc(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call oclint_app_drawfunc()\n");
  exit(1);
}

void
oclint_procs_foreach(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call oclint_procs_foreach()\n");
  exit(1);
}

void
oclint_appid_existp(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call oclint_appid_existp()\n");
  exit(1);
}

void
oclint_proc_getDisplay(void)
{
  fprintf(stderr, "FATAL ERROR: master cannot call oclint_proc_getDisplay()\n");
  exit(1);
}
