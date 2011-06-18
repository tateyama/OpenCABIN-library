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

#include <ocl.h>

int oclStereop = 0;
int oclScreen = OCL_FRONT;

// pmrenderer.c
void runidle(void);
int procs_foreach(int (*cbfunc)(int app_id, void *procdata, void *val), void *val, char *cmtstr);
void (*app_drawfunc(int app_id))(void *);

void
rg_init()
{
};

static int
draw_cb(int app_id, void *procdata, void *val)
{
  glPushMatrix();
  (*app_drawfunc(app_id))(procdata);
  glPopMatrix();
  return 0;
}

static void
draw(void)
{
  int i;
  struct process *procptr;

  //glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(90.0f, 1.0f, 0.05f, 100.0f);
  gluLookAt(0.0f, 0.0f, 1.25f,  /* eye */
	    0.0f, 1.25f, 1.25f,      /* center of a screen */
	    0.0f, 0.0f, 1.0f);      /* up */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  procs_foreach(draw_cb, 0, "rg_draw()");
  //glPopMatrix();
  glutSwapBuffers();
  //printf("rg_draw\n");
}

static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#if 1
  //glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glFrustum(-1.0, 1.0, -h, h, 1.25, 60000.0);
#else
  gluLookAt(0.0, 0.0, 30.0,  /* eye is at (0,0,30) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
#endif
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(0.0, 0.0, -40.0);
  printf("reshape\n");
  //glutPostRedisplay();
}

static void
menu(int val)
{
#if 0
  switch (val) {
  case 0:
    GLCABINindex = FRONT;
    break;
  case 1:
    GLCABINindex = BOTTOM;
    break;
  }
#endif
}

static void
keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 0x1b:
    fprintf(stderr, "escape\n");
    exit(1);
    break;
  }
}

static int changed = 0;

void
rg_postRedisplay(void)
{
  changed = 1;
}

static int wait_ms = 10;

static void
timer(int val)
{
  runidle();

  if (changed) {
    glutPostRedisplay();
    changed = 0;
  }

  glutTimerFunc(wait_ms, timer, 0);
}

static char winname[] = "rbase";

void
rg_mainloop(void)
{
  int i;
  int count = 0;
  int ac = 1;
  char *av[] = {"rctrl", 0};
  glutInit(&ac, av);
  //oclStereop = 1;
#ifdef __APPLE__
  oclStereop = 0;
#endif
  if (oclStereop) {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_STEREO);
  } else {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  }
#ifndef __APPLE__
//#define FULLSCREEN
#endif
#ifndef FULLSCREEN
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1024, 768);
  glutCreateWindow(winname);
#else
  glutCreateWindow(winname);
  glutFullScreen();
#endif
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

#if 0
  for (i = 0; i < APPNUM; ++i) {
    if (renapparray[i].dlhandle) {
      (*renapparray[i].ginit)();
      ++count;
    }
  }
#endif
#if 0
  if (count > 0) {
    glutIdleFunc(rpm_idle);
  }
#else
  glutTimerFunc(wait_ms, timer, 0);
#endif

  glutKeyboardFunc(keyboard);

  glutCreateMenu(menu);
  glutAddMenuEntry("front", 0);
  glutAddMenuEntry("bottom", 1);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
};
