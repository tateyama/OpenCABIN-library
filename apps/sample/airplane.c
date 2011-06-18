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

//#include <GL/glut.h>
#include <ocl.h>

inline void
mateColor(float r, float g, float b)
{
  GLfloat mate[4] = {r, g, b, 1.0f};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mate);
};

void
renderAPy(void)
{
    glBegin(GL_TRIANGLES);
    mateColor(1.0f, 0.0f, 1.0f);
    /* first triangle */
    glVertex3f( 0.0f,  0.0f, 0.0f);
    glVertex3f( 0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    /* third triangle */
    glVertex3f( 0.0f,  0.0f, 0.0f);
    glVertex3f( 0.2f,  0.5f, 0.0f);
    glVertex3f(-0.2f,  0.5f, 0.0f);
    /* fourth triangle */
    glVertex3f( 0.0f,  1.0f, 0.0f);
    glVertex3f( 0.2f,  0.5f, 0.0f);
    glVertex3f(-0.2f,  0.5f, 0.0f);
    glEnd();

    mateColor(1.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
    /* second triangle */
    glVertex3f( 0.0f,  0.0f, 0.0f);
    glVertex3f( 0.0f, -0.5f, 0.5f);
    glVertex3f( 0.0f, -0.5f, 0.0f);
    glEnd();
}

void
renderAPx(void)
{
}
