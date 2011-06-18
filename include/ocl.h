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
#ifdef __cplusplus
extern "C" {
#endif
  
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <sys/types.h>
#include <sys/uio.h>
#else
#include <GL/glut.h>
#endif
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shared_variable.h>
#include <quat.h>

  void oclPostRedisplay(void);

  // master
  void *minit(int ac, char **av); // returns mprocdata
  void midle(void *mprocdata);
  void mcleanup(void *mprocdata);

  // renderer
  void *rinit(int, char **); // returns procdata
  void gidle(void *procdata);
  void ginit(void *procdata);
  void gdraw(void *procdata);
  void rcleanup(void *procdata);
  void mjoystick(int no, int type, int number, int value, int time);

  extern int oclScreen;
  extern NUM oclNear, oclFar;
  extern int oclEyeSensor;
#ifdef __cplusplus
}
#endif

#define OCL_FRONT 0
#define OCL_LEFT 1
#define OCL_RIGHT 2
#define OCL_BOTTOM 3
#define OCL_TOP 4
#define OCL_CEILING OCL_TOP
#define OCL_FLOOR OCL_BOTTOM

// positions & euler angles
//#define OCL_POSDIM 6
// positions & quaternions
#define OCL_POSDIM 7

#define OCL_POSNUM OCL_POSDIM

//#define OCL_MAX_SENSORS 16

extern int oclSensorHead;

#include <termios.h>
#include "oclconf.h"
#include "sensors.h"
#include "joystick.h"
