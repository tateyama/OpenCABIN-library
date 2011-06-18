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

//float g_angle = 0.0f;
SHVARfloat(g_angle);
//externSHVAR(float *, eyepos);
static int g_rotate = 1;

// teapot stat
struct tstat {
  float x, y, z;
  float r, g, b;
};

static struct tstat *
tstat_new(void)
{
  struct tstat *retval;

  if (NULL == (retval = (struct tstat *)malloc(sizeof(struct tstat)))) {
    perror("malloc()");
    exit(1);
  }

  retval->x = 0.0f;
  retval->y = 3.0f;
  retval->z = 1.0f;

  retval->r = 0.0f;
  retval->g = 1.0f;
  retval->b = 0.0f;

  return retval;
}

#include <stdlib.h>
static void print_av(int ac, char **av);

static struct tstat *
tstat_parse_av(struct tstat *retval, int ac, char **av)
{
  if (ac < 2) return retval;
  retval->x = (float)strtod(av[1], NULL);

  if (ac < 3) return retval;
  retval->y = (float)strtod(av[2], NULL);

  if (ac < 4) return retval;
  retval->z = (float)strtod(av[3], NULL);

  if (ac < 5) return retval;
  retval->r = (float)strtod(av[4], NULL);

  if (ac < 6) return retval;
  retval->g = (float)strtod(av[5], NULL);

  if (ac < 7) return retval;
  retval->b = (float)strtod(av[6], NULL);

  return retval;
}

static struct tstat *
tstat_print(struct tstat *retval)
{
  printf("xyz(%10.5f, %10.5f, %10.5f) rgb(%10.5f, %10.5f, %10.5f)\n",
	 retval->x, retval->y, retval->z,
	 retval->r, retval->g, retval->b);
  return retval;
};

static void myTeapot(GLfloat x, GLfloat y, GLfloat z, GLfloat ambr, GLfloat ambg, GLfloat ambb, GLfloat scale);

static void
tstat_draw(struct tstat *retval)
{
  myTeapot(retval->x, retval->y, retval->z, retval->r, retval->g, retval->b, 0.5f);
}


static void
print_av(int ac, char **av)
{
  int i;
  printf("ac: %d, av: ", ac);
  for (i = 0; i < ac; ++i) {
    printf("\"%s\" ", av[i]);
  }
  printf("%x\n", av[i]);
}

void *
minit(int ac, char **av)
{
	printf("minit(): ");
	print_av(ac, av);

	//printf("renderers_num: %d\n", oclNumRenderers());
#if 0
  extern float eyepos[6];
  int i;
  for (i = 0; i < 6; ++i) {
    printf("eyepos[%d] = %f\n", i, eyepos[i]);
  }
#endif
  g_angle = 0.0f;
  SHVARINITfloat(g_angle);
  return 0;
}


void
midle(void *v)
{
  if (g_rotate) {
  g_angle += 2.0f;
  if (g_angle > 360.0f) {
    g_angle -= 360.0f;
  }

  SHVARUPDATE(g_angle);
  }
}

#include <sys/time.h>

void
gidle(void *v)
{
  SHVARREFfloat(g_angle);
  oclPostRedisplay();
}

static void
envinit(void)
{
  GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
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
  struct tstat *pot = (struct tstat *)v;
  tstat_print(pot);
}

static void
myTeapot(GLfloat x, GLfloat y, GLfloat z, GLfloat ambr, GLfloat ambg, GLfloat ambb, GLfloat scale)
{
  GLfloat mat[4] = {ambr, ambg, ambb, 1.0};

  glPushMatrix();
  glTranslatef(x, y, z);
  glRotatef(g_angle, 0.0f, 0.0f, 1.0f);
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  mat[0] = ambr;
  mat[1] = ambg;
  mat[2] = ambb;
  mat[3] = 1.0;
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
  glutSolidTeapot(scale);
  glPopMatrix();
}

void
gdraw(void *v)
{
  struct tstat *pot = (struct tstat *)v;

  GLfloat front_diffuse1[] = {0.3, 0.3, 0.3};
  GLfloat front_specular1[] = {0.5, 0.5, 0.5};
  //SHVARREFbufC(eyepos);

  envinit();

  glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse1);
  glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular1);
  glMaterialf(GL_FRONT, GL_SHININESS, 60.0);

  //tstat_print(pot);
#if 0
  myTeapot(0.0f, 3.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.5f);
#else
  tstat_draw(pot);
#endif
  //printf("%10.5f %10.5f %10.5f\n", eyepos[3], eyepos[4], eyepos[5]);
}

void
mcleanup(void *v)
{
}

void *
rinit(int ac, char **av)
{
  struct tstat *retval;
  printf("rinit(): &g_angle = %x; ", &g_angle);
  print_av(ac, av);
  //retval = tstat_print(tstat_parse_av(tstat_print(tstat_new()), ac, av));
  retval = tstat_print(tstat_parse_av(tstat_new(), ac, av));

  SHVARINITfloatC(g_angle);
  //GSHVARINITbufC(eyepos);
  //SHVARREFbufC(eyepos);
  //return 0;
  return (void *)retval;
}

void
mjoystick(int no, int type, int number, int value, int time)
{
  //printf("@ %x %d %2d %3d\n", no, type, number, value);
  if ((no == 0) && (type == OCL_JS_BUTTON) && (number == OCL_JS_BX) && (value == 1)) {
	if (g_rotate) {
		printf("> rotate off\n");
		g_rotate = 0;
	} else {
		printf("> rotate on\n");
		g_rotate = 1;
	}
  }
}
