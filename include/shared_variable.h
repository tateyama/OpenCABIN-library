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
// shared_variable.h
//

#define _SHVARID(NAME) _shvarid_ ## NAME

struct _shvartag {
  int id;
  int size;
};

typedef struct _shvartag _SHVARTAG;

//==================================================

// static ?
// file line

// declaration (master, client)
#define SHVAR(TYPE, NAME) \
  TYPE NAME; \
  struct _shvartag _SHVARID(NAME)
// _SHVARTAG _SHVARID(NAME)

#define externSHVAR(TYPE, NAME) \
  extern TYPE NAME;					\
  extern struct _shvartag _SHVARID(NAME)
// _SHVARTAG _SHVARID(NAME)

#define staticSHVAR(TYPE, NAME) \
  static TYPE NAME;				\
  static struct _shvartag _SHVARID(NAME)
// _SHVARTAG _SHVARID(NAME)

// initialization (constructor) (master)
#define SHVARINIT(NAME, TYPE)			\
  SHvar_init(&_SHVARID(NAME), #NAME, (char*) &NAME, sizeof(TYPE))

// update value (master)
#define SHVARUPDATE(NAME) \
  SHvar_update(&_SHVARID(NAME), (int *)&NAME)

// update value (master)
#define GSHVARUPDATE(NAME) \
  SHvarg_update(&_SHVARID(NAME), (int *)&NAME)

// fetch value (client)
#define SHVARREF(NAME) \
  SHvar_ref(_SHVARID(NAME), &NAME)

//==================================================

#define SHVARA(TYPE, NAME, NUM) \
  TYPE NAME[NUM]; \
  struct _SHVARTAG _SHVARID(NAME)

#define SHVARINITA(NAME, TYPE, NUM)

//==================================================

#define SHMALLOC(size) \
  SHmem_new(size)

#define SHMUPDATE(mem) \
  SHmem_update(mem)

//==================================================
#define SHVARbuf(NAME, type) \
  type NAME; \
  _SHVARTAG _SHVARID(NAME);

#define SHVARbufptr(NAME, type) \
  type *NAME; \
  _SHVARTAG _SHVARID(NAME);

//==================================================

#define SHVARINITbufM(NAME) \
  (NAME) = SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define SHVARINITbufMstar(NAME) \
  (NAME) = *(SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size))

#define SHVARINITbufMcopy(NAME) \
  memcopy(&(NAME), SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size), _SHVARID(NAME).size)

#define SHVARINITbufMcopyptr(NAME) \
  memcopy((NAME), SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size), _SHVARID(NAME).size)

//--------------------------------------------------

#define GSHVARINITbufM(NAME) \
  (NAME) = SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define GSHVARINITbufMstar(NAME) \
  (NAME) = *(SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size))

#define GSHVARINITbufMcopy(NAME) \
  memcopy(&(NAME), SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size), _SHVARID(NAME).size)

#define GSHVARINITbufMcopyptr(NAME) \
  memcopy((NAME), SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size), _SHVARID(NAME).size)

//==================================================

#define SHVARINITbufC(NAME) \
  (NAME) = SHvarVdml_initC(&_SHVARID(NAME), #NAME)

#define SHVARINITbufCstar(NAME) \
  (NAME) = *(SHvarVdml_initC(&_SHVARID(NAME), #NAME))

#define SHVARINITbufCcopy(NAME) \
  memcpy((void *)&(NAME), SHvarVdml_initC(&_SHVARID(NAME), #NAME), (_SHVARID(NAME)).size)

#define SHVARINITbufCcopyptr(NAME) \
  memcpy((void *)(NAME), SHvarVdml_initC(&_SHVARID(NAME), #NAME), (_SHVARID(NAME)).size)

//--------------------------------------------------
#define GSHVARINITbufC(NAME) \
  NAME = SHvarVdmlg_initC(&_SHVARID(NAME), #NAME)
//  NAME = SHvarVdml_initC(&_SHVARID(NAME), #NAME)
//  SHvarVdmlg_initC(&_SHVARID(NAME), #NAME)

#define GSHVARINITbufCstar(NAME) \
  (NAME) = *(SHvarVdmlg_initC(&_SHVARID(NAME), #NAME))

#define GSHVARINITbufCcopy(NAME) \
  memcpy(&(NAME), SHvarVdmlg_initC(&_SHVARID(NAME), #NAME), (_SHVARID(NAME)).size)

#define GSHVARINITbufCcopyptr(NAME) \
  memcpy((NAME), SHvarVdmlg_initC(&_SHVARID(NAME), #NAME), (_SHVARID(NAME)).size)

//--------------------------------------------------

#define SHVARREFbufC(NAME) \
  (NAME) = SHvarVdml_refC(&_SHVARID(NAME))

#define SHVARREFbufCstar(NAME) \
  (NAME) = *(SHvarVdml_refC(&_SHVARID(NAME)))

#define SHVARREFbufCcopy(NAME) \
  memcpy((void *)&(NAME), SHvarVdml_refC(&_SHVARID(NAME)), (_SHVARID(NAME)).size)

#define SHVARREFbufCcopyptr(NAME) \
  memcpy((void *)(NAME), SHvarVdml_refC(&_SHVARID(NAME)), (_SHVARID(NAME)).size)

#define GSHVARREFbufC(NAME) SHVARREFbufC(NAME)
#define GSHVARREFbufCstar(NAME) SHVARREFbufCstar(NAME)
#define GSHVARREFbufCcopy(NAME) SHVARREFbufCcopy(NAME)
#define GSHVARREFbufCcopyptr(NAME) SHVARREFbufCcopyptr(NAME)

//--------------------------------------------------

#define SHVARUPDATEbufLS(NAME) \
  SHvarVdml_sendls(&_SHVARID(NAME))

#define SHVARUPDATEbufLSstar(NAME, TYPE) \
  *((TYPE *)SHvarVdml_refM(&_SHVARID(NAME))) = (NAME); \
  SHvarVdml_sendls(&_SHVARID(NAME))

#define SHVARUPDATEbufLScopy(NAME) \
  memcpy(SHvarVdml_refM(&_SHVARID(NAME)), (void *)&(NAME), (_SHVARID(NAME)).size); \
  SHvarVdml_sendls(&_SHVARID(NAME))


#define SHVARUPDATEbufLL(NAME) \
  SHvarVdml_sendll(&_SHVARID(NAME))

#define SHVARUPDATEbufLLstar(NAME, TYPE) \
  *((TYPE *)SHvarVdml_refM(&_SHVARID(NAME))) = (NAME); \
  SHvarVdml_sendll(&_SHVARID(NAME))

#define SHVARUPDATEbufLLcopy(NAME) \
  memcpy(SHvarVdml_refM(&_SHVARID(NAME)), (void *)&(NAME), (_SHVARID(NAME)).size); \
  SHvarVdml_sendll(&_SHVARID(NAME))

//==================================================
// float (direct)
//--------------------------------------------------

// declaration (master, client)
#define SHVARfloat(NAME) \
  float NAME;\
  _SHVARTAG _SHVARID(NAME)

// fetch value (client)
#define SHVARREFfloat(NAME) \
  SHvar_refDirect(&_SHVARID(NAME), (int *)&NAME)

#define SHVARINITfloat(NAME)  \
  SHvar_initDirect(&_SHVARID(NAME), #NAME, (int *) &NAME)

#define SHVARINITfloatM(NAME) SHVARINITfloat(NAME)

#define SHVARINITfloatC(NAME)  \
  SHvar_initDirectC(&_SHVARID(NAME), #NAME)

#define SHVARUPDATEfloat(NAME) SHVARUPDATE(NAME)

//==================================================
// G float (direct)
//--------------------------------------------------

#define GSHVARfloat(NAME) SHVARfloat(NAME)

#define GSHVARREFfloat(NAME) \
  SHvarg_refDirect(&_SHVARID(NAME), (int *)&NAME)

#define GSHVARINITfloat(NAME)  \
  SHvarg_initDirect(&_SHVARID(NAME), #NAME, (int *) &NAME)

#define GSHVARINITfloatM(NAME) GSHVARINITfloat(NAME)

#define GSHVARINITfloatC(NAME) SHVARINITfloatC(NAME)

#define GSHVARUPDATEfloat(NAME) GSHVARUPDATE(NAME)

//==================================================
// int (direct)
//--------------------------------------------------

// declaration (master, client)
#define SHVARint(NAME) \
  int NAME;\
  _SHVARTAG _SHVARID(NAME)

// fetch value (client)
#define SHVARREFint(NAME) \
  SHvar_refDirect(&_SHVARID(NAME), (int *)&NAME)

#define SHVARINITint(NAME)  \
  SHvar_initDirect(&_SHVARID(NAME), #NAME, (int *) &NAME)

#define SHVARINITintM(NAME) SHVARINITint(NAME)

#define SHVARINITintC(NAME)  \
  SHvar_initDirectC(&_SHVARID(NAME), #NAME)

#define SHVARUPDATEint(NAME) SHVARUPDATE(NAME)

//==================================================
// G int (direct)
//--------------------------------------------------

// declaration (master, client)

#define GSHVARint(NAME) SHVARint(NAME)

#define GSHVARREFint(NAME) \
  SHvarg_refDirect(&_SHVARID(NAME), (int *)&NAME)

#define GSHVARINITint(NAME)  \
  SHvarg_initDirect(&_SHVARID(NAME), #NAME, (int *) &NAME)

#define GSHVARINITintM(NAME) GSHVARINITint(NAME)

#define GSHVARINITintC(NAME) SHVARINITintC(NAME) 

#define GSHVARUPDATEint(NAME) GSHVARUPDATE(NAME)

//==================================================
// double (indirect)
//--------------------------------------------------

// declaration (master, client)
#define SHVARdouble(NAME) \
  double NAME;\
  _SHVARTAG _SHVARID(NAME) = {0, sizeof(double)}

#define SHVARINITdoubleM(NAME) SHVARINITbufMstar(NAME)
  //NAME = *(SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size))

#define SHVARINITdouble(NAME)  SHVARINITdoubleM(NAME)

//#define SHVARINITdoubleC(NAME) SHVARINITbufC(NAME)
#define SHVARINITdoubleC(NAME) SHVARINITbufCstar(NAME)

#define SHVARREFdouble(NAME) SHVARREFbufCstar(NAME)
//#define SHVARREFdouble(NAME) SHVARREFbufCcopy(NAME)
//  NAME = *(SHvarVdml_refC(&_SHVARID(NAME)))

#define SHVARUPDATEdoubleLS(NAME) SHVARUPDATEbufLSstar(NAME, double)
//#define SHVARUPDATEdoubleLS(NAME) SHVARUPDATEbufLScopy(NAME)

#define SHVARUPDATEdoubleLL(NAME) SHVARUPDATEbufLLstar(NAME, double)
//#define SHVARUPDATEdoubleLL(NAME) SHVARUPDATEbufLLcopy(NAME)

#define SHVARUPDATEdouble(NAME) SHVARUPDATEdoubleLL(NAME)

//==================================================
// G double (indirect)
//--------------------------------------------------

#define GSHVARdouble(NAME) SHVARdouble(NAME)

#define GSHVARINITdoubleM(NAME) GSHVARINITbufMstar(NAME)
  //NAME = SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define GSHVARINITdouble(NAME) GSHVARINITdoubleM(NAME)

#define GSHVARINITdoubleC(NAME) GSHVARINITbufC(NAME)

#define GSHVARREFdouble(NAME) GSHVARREFbufCstar(NAME)

#define GSHVARUPDATEdoubleLS(NAME) SHVARUPDATEdoubleLS(NAME)

#define GSHVARUPDATEdoubleLL(NAME) SHVARUPDATEdoubleLL(NAME)

#define GSHVARUPDATEdouble(NAME) GSHVARUPDATEdoubleLL(NAME)


//==================================================
// array (indirect)
//--------------------------------------------------

// declaration
#define SHVARarray(type, NAME, num) \
  type *NAME; \
  _SHVARTAG _SHVARID(NAME) = {0, sizeof(type) * num}

#define SHVARINITarrayM(NAME) \
  NAME = SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define SHVARINITarrayC(NAME) SHVARINITbufC(NAME)

#define SHVARUPDATEarrayLS(NAME) SHVARUPDATEbufLS(NAME)

#define SHVARUPDATEarrayLL(NAME) SHVARUPDATEbufLL(NAME)

#define SHVARUPDATEarray(NAME) SHVARUPDATEarrayLL(NAME)

//==================================================
// G array (indirect)
//--------------------------------------------------

// declaration
#define GSHVARarray(type, NAME, num) SHVARarray(type, NAME, num)

#define GSHVARINITarrayM(NAME) \
  NAME = SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define GSHVARINITarrayC(NAME) GSHVARINITbufC(NAME)

#define GSHVARUPDATEarrayLS(NAME) SHVARUPDATEbufLS(NAME)

#define GSHVARUPDATEarrayLL(NAME) SHVARUPDATEbufLL(NAME)

#define GSHVARUPDATEarray(NAME) GSHVARUPDATEarrayLL(NAME)

//==================================================
// struct (indirect)
//--------------------------------------------------

// struct human {
//   char name[10];
//   int age;
// };
//
// SHVARstruct(test, tarou);
// 
// void *minit(int ac, char **av) {
//   SHVARINITstruct(tarou);
//   strcpy(tarou->name, "test");
//   tarou->age = 20;
//   SHVARUPDATEstructLL(tarou);
// }
// 
// void
// midle(void *mdata)
// {
//   ++tarou->age;
//   SHVARUPDATEstructLS(tarou);
// }
//
// void *rinit(int ac, char **av) {
//   SHVARINITstructC(tarou)
// }

// declaration
#define SHVARstruct(type, NAME) \
  struct type *NAME;\
  _SHVARTAG _SHVARID(NAME) = {0, sizeof(struct type)}

#define SHVARINITstructM(NAME) \
  (NAME) = SHvarVdml_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define SHVARINITstructC(NAME) SHVARINITbufC(NAME)

#define SHVARUPDATEstructLS(NAME) SHVARUPDATEbufLS(NAME)

#define SHVARUPDATEstructLL(NAME) SHVARUPDATEbufLL(NAME)

#define SHVARUPDATEstruct(NAME) SHVARUPDATEstructLL(NAME)

//==================================================
// G struct (indirect)
//--------------------------------------------------

// declaration

#define GSHVARstruct(type, NAME) SHVARstruct(type, NAME)

#define GSHVARINITstructM(NAME) \
  (NAME) = SHvarVdmlg_newM(&(_SHVARID(NAME)), #NAME, _SHVARID(NAME).size)

#define GSHVARINITstructC(NAME) GSHVARINITbufC(NAME)

#define GSHVARUPDATEstructLS(NAME) SHVARUPDATEbufLS(NAME)

#define GSHVARUPDATEstructLL(NAME) SHVARUPDATEbufLL(NAME)

#define GSHVARUPDATEstruct(NAME) GSHVARUPDATEstructLL(NAME)

// ==================================================

void SHvar_initDirect(_SHVARTAG *tagptr, char *name, int *var);
void SHvarg_initDirect(_SHVARTAG *tagptr, char *name, int *var);
void SHvar_update(_SHVARTAG *tagptr, int *var);
void SHvarg_update(_SHVARTAG *tagptr, int *var);
void SHvar_refDirect(_SHVARTAG *tagptr, int *var);
void SHvarg_refDirect(_SHVARTAG *tagptr, int *var);
void SHvar_initDirectC(_SHVARTAG *tagptr, char *name);
unsigned char *SHvarVdml_initC(_SHVARTAG *tagptr, char *name); 
unsigned char *SHvarVdmlg_initC(_SHVARTAG *tagptr, char *name); 

unsigned char *SHvarVdml_newM(_SHVARTAG *tagptr, char *name, int size);
unsigned char *SHvarVdmlg_newM(_SHVARTAG *tagptr, char *name, int size);

unsigned char *SHvarVdml_refC(_SHVARTAG *tagptr);
unsigned char *SHvarVdmlg_refC(_SHVARTAG *tagptr);

unsigned char *SHvarVdml_refM(_SHVARTAG *tagptr);

void SHvarVdml_sendll(_SHVARTAG *tagptr);
void SHvarVdml_sendls(_SHVARTAG *tagptr);
