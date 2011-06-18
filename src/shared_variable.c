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
// shared_variable.c
//
// shared variable implementation for OpenCABINlib Applications

// Master function
// return id

#include <stdio.h>
#include "cabprot.h"
#include "shared_variable.h"
#include "vdml.h"

void valm_init(void);
int valm_set(int type, int symid, int val);
int valc_getval(int typeid, int symid);
void valc_init(void);

void
SHvar_minit()
{
  valm_init();
  snm_init();
}

void
SHvar_cinit()
{
  valc_init();
  snc_init();
}

//==================================================
// Direct Value (int) functions


// Master function
void
SHvar_initDirect(_SHVARTAG *tagptr, char *name, int *var)
{
  int namesym;
  int varid;

  namesym = snm_intern(name);
  varid = valm_set(0, namesym, *var);
  tagptr->id = namesym;
}

// Master function
void
SHvarg_initDirect(_SHVARTAG *tagptr, char *name, int *var)
{
  int namesym;
  int varid;

  namesym = snm_intern(name);
  varid = valgm_set(0, namesym, *var);
  tagptr->id = namesym;
}

// Client function
void
SHvar_initDirectC(_SHVARTAG *tagptr, char *name)
{
  int namesym = snc_query(name);
  tagptr->id = namesym;
}

// Client function
void
SHvar_refDirect(_SHVARTAG *tagptr, int *var)
{
  *var = valc_getval(0, tagptr->id);
}

void
SHvarg_refDirect(_SHVARTAG *tagptr, int *var)
{
  *var = valgc_getval(0, tagptr->id);
}

// Master function
void
SHvar_update(_SHVARTAG *tagptr, int *var)
{
  valm_set(0, tagptr->id, *var);
}

// Master function
void
SHvarg_update(_SHVARTAG *tagptr, int *var)
{
  valgm_set(0, tagptr->id, *var);
}

//==================================================
void
SHvar_init(_SHVARTAG *tagptr, char *name, void *var, int size)
{
  
}

// Renderer & Master function
void
SHvar_ref(_SHVARTAG *tagptr, void *var)
{
  
}

//==================================================
// master
unsigned char *
SHvarVdml_newM(_SHVARTAG *tagptr, char *name, int size)
{
  struct vdml *ptr;
  int namesym = snm_intern(name);
  int valid = valm_getval(0, namesym);
  if (-1 == valid) {
    valid = vdmlm_new(size, &ptr);
    valm_set(0, namesym, valid);
  }
  tagptr->id = valid;
  return ptr->buf;
}

//==================================================
unsigned char *
SHvarVdmlg_newM(_SHVARTAG *tagptr, char *name, int size)
{
  struct vdml *ptr;
  int namesym = snm_intern(name);
  int valid = valgm_getval(0, namesym);
  if (-1 == valid) {
    valid = vdmlm_new(size, &ptr);
    valgm_set(0, namesym, valid);
  }
  tagptr->id = valid;
  return ptr->buf;
}

// lossless send (master)
void
SHvarVdml_sendll(_SHVARTAG *tagptr)
{
  vdmlm_sendll(tagptr->id);
};

// lossy send (master)
void
SHvarVdml_sendls(_SHVARTAG *tagptr)
{
  vdmlm_sendls(tagptr->id);
};

unsigned char *
SHvarVdmlg_initM(_SHVARTAG *tagptr, char *name)
{
  //int namesym = snc_query(name);
  int namesym = snm_intern(name);
  int valid = valgm_getval(0, namesym);
  tagptr->id = valid;
  //vdmlm_new(size);
  return vdmlm_getbuf(valid);
}

unsigned char *
SHvarVdml_refM(_SHVARTAG *tagptr)
{
  return vdmlm_getbuf(tagptr->id);
}

//==================================================
unsigned char *
SHvarVdmlg_initC(_SHVARTAG *tagptr, char *name)
{
  int namesym = snc_query(name);
  int valid = valgc_getval(0, namesym);
  tagptr->id = valid;
  return vdmlc_getbuf(valid);
}

unsigned char *
SHvarVdml_initC(_SHVARTAG *tagptr, char *name)
{
  int namesym = snc_query(name);
  //int valid = valc_getval(getPOID(), namesym);
  int valid = valc_getval(0, namesym);
  tagptr->id = valid;
  return (unsigned char *)vdmlc_getbuf(valid);
}

unsigned char *
SHvarVdml_refC(_SHVARTAG *tagptr)
{
  return vdmlc_getbuf(tagptr->id);
}

//==================================================
// shared variable client routine

#if 0
static
void
rcmd_shv(int fd)
{
  if (0 >= readforce(newfd, (char *)&size, sizeof(size))) {
    cab_error(1, "cannot get loadapp command from master\n");
    exit(1);
  }
}

shvc_init()
{
  // 
  rbase_regrcmd(RENDCMD_SHV, rcmd_shv);

  // client
  snc_init();
}

//==================================================
// net thread
rcmd_shv0()
{
  
  snc_newsymnam(int id, char *str);
};
#endif
