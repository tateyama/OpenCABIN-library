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

void
send2renderers(char *msg, int size)
{
  fprintf(stderr, "FATAL ERROR: renderer cannot call send2renderers()\n");
  exit(1);
}

int
getPOIDm(void)
{
  fprintf(stderr, "FATAL ERROR: renderer cannot call getPOIDm()\n");
  exit(1);
}

//mpm_call_mjoystick() {};
void mpm_push_mjoystick(int no, int type, int number, int value, int time) {};


void
oclint_renderers_num(void)
{
  fprintf(stderr, "FATAL ERROR: renderer cannot call getPOIDm()\n");
  exit(1);
}
