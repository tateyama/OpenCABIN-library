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

#include <ocl.h>
//#include <joystick.h>

void oclJS_changeStatus(int no, int type, int number, int value, int time);
void oclJS_getStatus(int no, struct ocl_jsstat *status);
void oclJS_delete(int no);

static struct ocl_jsstat g_jsstat[2];

int oclJS_New(void)
{
  
  return 0;
}

// master function
void
oclJS_changeStatus(int no, int type, int number, int value, int time)
{
  if (type == OCL_JS_BUTTON) {
    // OCL_JS_BUTTON
    g_jsstat[no].button[number] = value;
  } else {
    // OCL_JS_STICK
    g_jsstat[no].stick[number] = value;
  }

  // fire "mjoystick" apps
  //mpm_call_mjoystick(no, type, number, value, time);
  mpm_push_mjoystick(no, type, number, value, time);
}

void
oclJS_getStatus(int no, struct ocl_jsstat *stat)
{
  *stat = g_jsstat[no];
};

