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

NUM oclNear = OCL_NEAR_DEFAULT;
NUM oclFar = OCL_FAR_DEFAULT;

static void
frustumFront0(NUM pos[3])
{
  NUM d = OCL_FRONT_ORG_DIST_DEPTH - pos[1];
  NUM left, right, top, bottom;

  //if (d < 0.01) d = 0.01;

  left = -(OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / d + pos[0];
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / d + pos[0];
  top = (OCL_FRONT_HEIGHT - pos[2]) * oclNear / d + pos[2];
  bottom = -pos[2] * oclNear / d + pos[2];

  glFrustum(left, right, bottom, top, oclNear, oclFar);
}

static void
frustumFront(NUM pos[3])
{
  NUM d = OCL_FRONT_ORG_DIST_DEPTH - pos[1];
  NUM left, right, top, bottom;

  //if (d < 0.01) d = 0.01;

  left = -(OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / d;
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / d;
  top = (OCL_FRONT_HEIGHT - pos[2]) * oclNear / d;
  bottom = -pos[2] * oclNear / d;

  glFrustum(left, right, bottom, top, oclNear, oclFar);
  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0], pos[1] + 1.0f, pos[2], /* center is toward screen...  */
	    0.0, 0.0, 1.0f); /* up is in positive Y direction */
}

frustumFront2(NUM pos[3])
{
  NUM d = OCL_FRONT_ORG_DIST_DEPTH - pos[1];
  NUM left, right, top, bottom;

  //if (d < 0.01) d = 0.01;

  left = -(OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / d;
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / d;
  top = (OCL_FRONT_HEIGHT - pos[2]) * oclNear / d;
  bottom = -pos[2] * oclNear / d;

  glFrustum(left, right, bottom, top, oclNear, oclFar);

  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0], pos[1] + 1.0f, pos[2], /* center is toward screen...  */
	    0.0, 0.0, 1.0f); /* up is in positive Z direction */
}

static void
frustumBottom0(NUM pos[3])
{
  NUM z = pos[2];
  NUM left, right, top, bottom;

  if (z < 0.01) z = 0.01;

  left = pos[0] - (OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / z;
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / z + pos[0];
  bottom = pos[1] - (OCL_LEFT_WIDTH - OCL_FRONT_ORG_DIST_DEPTH + pos[1]) * oclNear / z;
  top = pos[1] + (OCL_FRONT_ORG_DIST_DEPTH - pos[1]) * oclNear / z;

  glFrustum(left, right, bottom, top, oclNear, oclFar);
}

static void
frustumBottom(NUM pos[3])
{
  NUM z = pos[2];
  NUM left, right, top, bottom;

  if (z < 0.01) z = 0.01;

  left =  - (OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / z;
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / z;
  bottom =  - (OCL_BOTTOM_HEIGHT - OCL_FRONT_ORG_DIST_DEPTH + pos[1]) * oclNear / z;
  top = (OCL_FRONT_ORG_DIST_DEPTH - pos[1]) * oclNear / z;

  glFrustum(left, right, bottom, top, oclNear, oclFar);
  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0], pos[1], pos[2]  - 1.0f, /* center is toward screen...  */
	    0.0, 1.0, 0.0f); /* up is in positive Y direction */
}

static void
frustumLeft(NUM pos[3])
{
  NUM left, right, top, bottom;

  left = -(OCL_LEFT_WIDTH - OCL_FRONT_ORG_DIST_DEPTH + pos[1]) * oclNear / (OCL_FRONT_ORG_DIST_LEFT + pos[0]);
  right = (OCL_FRONT_ORG_DIST_DEPTH - pos[1]) * oclNear / (OCL_FRONT_ORG_DIST_LEFT + pos[0]);
  top = (OCL_FRONT_HEIGHT - pos[2]) * oclNear / (OCL_FRONT_ORG_DIST_LEFT + pos[0]);
  bottom = - pos[2] * oclNear / (OCL_FRONT_ORG_DIST_LEFT + pos[0]);

  glFrustum(left, right, bottom, top, oclNear, oclFar);
  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0] - 1.0f, pos[1], pos[2], /* center is toward screen...  */
	    0.0, 0.0, 1.0f); /* up is in positive Z direction */
}

static void
frustumRight(NUM pos[3])
{
  NUM left, right, top, bottom, tmp;

  tmp = OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0];
  left = -(OCL_FRONT_ORG_DIST_DEPTH - pos[1]) * oclNear / tmp;
  right = (OCL_LEFT_WIDTH - OCL_FRONT_ORG_DIST_DEPTH + pos[1]) * oclNear / tmp;

  //tmp = OCL_FRONT_ORG_DIST_DEPTH - pos[0];
  bottom = -pos[2] * oclNear / tmp;
  top = (OCL_FRONT_HEIGHT - pos[2]) * oclNear / tmp;

  glFrustum(left, right, bottom, top, oclNear, oclFar);
  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0] + 1.0f, pos[1], pos[2], /* center is toward screen...  */
	    0.0, 0.0, 1.0f); /* up is in positive Z direction */
}

static void
frustumTop(NUM pos[3])
{
  NUM left, right, top, bottom, tmp;

  tmp = OCL_FRONT_HEIGHT - pos[2];
  left = -(OCL_FRONT_ORG_DIST_LEFT + pos[0]) * oclNear / tmp;
  right = (OCL_FRONT_WIDTH - OCL_FRONT_ORG_DIST_LEFT - pos[0]) * oclNear / tmp;
  top = (OCL_LEFT_WIDTH - OCL_FRONT_ORG_DIST_DEPTH + pos[1]) * oclNear / tmp;
  bottom = -(OCL_FRONT_ORG_DIST_DEPTH - pos[1]) * oclNear / tmp;

  glFrustum(left, right, bottom, top, oclNear, oclFar);
  gluLookAt(pos[0], pos[1], pos[2],  /* eye is here */
	    pos[0], pos[1], pos[2] + 1.0f, /* center is toward screen...  */
	    0.0, -1.0f, 0.0f); /* up is in negative Y direction */
}

static
void (*frustumFuncs[])() = {
  frustumFront, /*front*/
  frustumLeft, /* left */
  frustumRight, /* right */
  frustumBottom, /* bottom */
  frustumTop /* top */
};

void
oclSetFrustum(NUM pos[3])
{
  (*(frustumFuncs[oclScreen]))(pos);
}
