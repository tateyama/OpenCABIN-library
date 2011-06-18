#define TEST
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
// sample.c

#include <ocl.h>

//typedef float POS[6];

//externSHVAR(float *, eyepos);
//staticSHVAR(float *, eyepos);

void *
minit(int ac, char **av)
{
  return 0;
}


void
midle(void *v)
{
}

#include <sys/time.h>

void
gidle(void *v)
{
  //SHVARREFfloat(g_angle);
  oclPostRedisplay();
}

static void
envinit(void)
{
  GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat position[] = {0.0, -3.0, 3.0, 0.0};
  GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat local_view[] = {0.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

  glFrontFace(GL_CW);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void
ginit(void *v)
{
  //envinit();
}



void
renderEyeDir(NUM eyeL[3], NUM eyeR[3], NUM eyeDir[3])
{
  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_LINES);

  glVertex3f(eyeL[0], eyeL[1], eyeL[2]);
  glVertex3f(eyeL[0] + eyeDir[0], eyeL[1] + eyeDir[1], eyeL[2] + eyeDir[2]);

  glVertex3f(eyeR[0], eyeR[1], eyeR[2]);
  glVertex3f(eyeR[0] + eyeDir[0], eyeR[1] + eyeDir[1], eyeR[2] + eyeDir[2]);

  glEnd();
}

NUM *headpos = 0;


static void
myRotate(void)
{
  NUM qoffset[4], qoffset_1[4], qsensor[4], qsensor_1[4];
  NUM quattmp1[4], quattmp2[4], quattmp3[4], quattmp4[4];
  NUM quatx[4], quaty[4], quatz[4], quatzy[4], quatzy_1[4];
  NUM matrix[16], euler[3];
  NUM posresult[4], pos[3];
  NUM posoffset[3] = {0.2f, 1.7f, 0.2f};

  NUM eyeL[3], eyeR[3], eyeDir[3];
  NUM posoriq[7], poseuler[7];
  //int sensorIdx = oclSensorHead;
  int sensorIdx = OCL_SENSOR1;

  oclGetPosOriQuat(posoriq, sensorIdx);
  oclGetEyes(sensorIdx, eyeL, eyeR, eyeDir);
  renderEyeDir(eyeL, eyeR, eyeDir);

  oclGetPosEuler(poseuler, sensorIdx);

#if 1
  glTranslatef(poseuler[0], poseuler[1], poseuler[2]);
#else
  glTranslatef(posoriq[0], posoriq[1], posoriq[2]);
#endif

#if 1
  // euler offset test
  glRotatef(poseuler[3], 0.0f, 0.0f, 1.0f);
  glRotatef(poseuler[4], 0.0f, 1.0f, 0.0f);
  glRotatef(poseuler[5], 1.0f, 0.0f, 0.0f);
#else
  oclRotateSensor(sensorIdx);
#endif
}

void
gdraw(void *v)
{
  GLfloat front_diffuse1[] = {0.3, 0.3, 0.3};
  GLfloat front_specular1[] = {0.5, 0.5, 0.5};
  //SHVARREFbufC(eyepos);

  envinit();

  glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse1);
  glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular1);
  glMaterialf(GL_FRONT, GL_SHININESS, 60.0);

  glPushMatrix();

  myRotate();

  glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
  glScalef(.2f, .2f, .2f);
  renderAPy();

  glPopMatrix();
}

void
mcleanup(void *v)
{
}

void *
rinit(int ac, char **av)
{
  headpos = oclGetRawSensor(OCL_SENSOR_HEAD);
  return 0;
}
