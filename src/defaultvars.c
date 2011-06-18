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

#include "shared_variable.h"
#include "defaultvars.h"

#if 0
typedef float POS[6];

//SHVAR(float *, eyepos);
SHVARarray(float *, eyepos, 6);

void
minit_eyepos(void)
{
  _SHVARTAG tag;
  int i;
  float *ptr;

  //ptr = (float *)SHvarVdmlg_newM(&tag, "eyepos", sizeof(POS));
  //ptr = (float *)SHvarVdmlg_newM(&(_SHVARID(eyepos)), "eyepos", sizeof(POS));
  GSHVARINITarrayM(eyepos);
  //eyepos = ptr;

  for (i = 0; i < sizeof(POS) / sizeof(float); ++i) {
    *ptr = 0.0f;
  }
    
  //SHvarVdml_sendll(&tag);
  //SHvarVdml_sendll(&_SHVARID(eyepos));
  SHVARUPDATEbufLL(eyepos);
}

void
minit_handpos(void)
{
  _SHVARTAG tag;
  int i;
  float *ptr = (float *)SHvarVdmlg_newM(&tag, "handpos", sizeof(POS));

  for (i = 0; i < sizeof(POS) / sizeof(float); ++i) {
    *ptr = 0.0f;
  }

  SHvarVdml_sendll(&tag);
}
#endif

// exported vars
int sym_stop;
int sym_cont;
int sym_kill;
int sym_intr;

void
minit_defaultvars(void)
{
  //minit_eyepos();
  //minit_handpos();
  sym_stop = snm_intern("stop");
  sym_cont = snm_intern("cont");
  sym_intr = snm_intern("intr");
  sym_kill = snm_intern("kill");
}


void
cinit_eyepos(void)
{
  _SHVARTAG tag;
  SHvarVdml_initC(&tag, "eyepos");
}

void
cinit_handpos(void)
{
  _SHVARTAG tag;
  SHvarVdml_initC(&tag, "eyepos");
}

void
cinit_defaultvars(void)
{
  cinit_eyepos();
  cinit_handpos();
}

