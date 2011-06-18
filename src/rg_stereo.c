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

int oclScreen = OCL_SCREEN;
#if 1
int oclStereop = OCL_STEREO;
#else
int oclStereop = 1;
#endif

int oclFullScreen = OCL_FULLSCREEN;

// pmrenderer.c
void runidle(void);
int oclint_procs_foreach(int (*cbfunc)(int app_id, int proc_id, void *procdata, void *val), void *val, char *cmtstr);
void (*oclint_app_drawfunc(int app_id))(void *);
int oclint_appid_existp(int appid);

void
rg_init()
{
};

static int
draw_cb(int app_id, int proc_id, void *procdata, void *val)
{
  int d;
  if (-1 == (d = oclint_proc_getDisplay(proc_id))) {
    fprintf(stderr, "draw_cb(): proc_id: %d; no such procid\n", proc_id);
    return 0;
  }

  if (0 == d) return 0; // display off

#if 1 // insane check
  if (!oclint_appid_existp(app_id)) {
    fprintf(stderr, "draw_cb(): app_id: %d; no such appid\n", app_id);
    return 0;
  }
#endif

  glPushMatrix();
  (*oclint_app_drawfunc(app_id))(procdata);
  glPopMatrix();
  return 0;
}

void
oclMonoDraw(NUM pos[3])
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  oclSetFrustum(pos);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //printf("monodraw "); fflush(stdout);

  oclint_procs_foreach(draw_cb, 0, "rg_draw()");
}

void
oclStereoDraw(void)
{
  NUM eyeL[3], eyeR[3], eyeDir[3];

  oclGetEyes(oclSensorHead, eyeL, eyeR, eyeDir);

  glDrawBuffer(GL_BACK_LEFT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  oclMonoDraw(eyeL);

  glDrawBuffer(GL_BACK_RIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  oclMonoDraw(eyeR);

  glutSwapBuffers();
}

void
oclPassiveStereoDraw(void)
{
  NUM eyeL[3], eyeR[3], eyeDir[3];

  oclGetEyes(oclSensorHead, eyeL, eyeR, eyeDir);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(OCL_VIEWPORT_LEFT_X, OCL_VIEWPORT_LEFT_Y,
	     OCL_VIEWPORT_LEFT_WIDTH, OCL_VIEWPORT_LEFT_HEIGHT);
  oclMonoDraw(eyeL);

  glViewport(OCL_VIEWPORT_RIGHT_X, OCL_VIEWPORT_RIGHT_Y,
	     OCL_VIEWPORT_RIGHT_WIDTH, OCL_VIEWPORT_RIGHT_HEIGHT);
  oclMonoDraw(eyeR);

  glutSwapBuffers();
}

static void
draw(void)
{
  int i;
  NUM eyeL[3], eyeR[3], eyeDir[3];
  struct process *procptr;

  //glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  switch (oclStereop) {
  case 1: // active stereo
    oclStereoDraw();
    break;
  case 0: // mono
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    oclGetEyes(oclSensorHead, eyeL, eyeR, eyeDir);
    oclMonoDraw(eyeL);
    glutSwapBuffers();
    break;
  case 2:
    oclPassiveStereoDraw();
    break;
  }
}

static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  // (/ (- 1024.0 750) 2)  (/ (- 768.0 750) 2)
  //glViewport(0, 0, (GLint) width, (GLint) height);
  //glViewport(137, 9, 750, 750);
  glViewport(OCL_VIEWPORT_LEFT_X, OCL_VIEWPORT_LEFT_Y,
	     OCL_VIEWPORT_LEFT_WIDTH, OCL_VIEWPORT_LEFT_HEIGHT);
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
  if (oclStereop == 1) {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_STEREO);
  } else {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  }
#ifndef __APPLE__
//#define FULLSCREEN
#endif

  if (oclFullScreen) {
    glutCreateWindow(winname);
    glutFullScreen();
  } else {
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 768);
    glutCreateWindow(winname);
  }

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
