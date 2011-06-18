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

static NUM z_axis[3] = {0.0, 0.0, 1.0};
static NUM y_axis[3] = {0.0, 1.0, 0.0};
static NUM x_axis[3] = {1.0, 0.0, 0.0};

static NUM quatx[4], quaty[4], quatz[4], quatzy[4], quatzy_1[4];

rotate_init()
{
  // offset quat
  rot2quatDeg(quatz, -90.0f, z_axis);  // z -90
  rot2quatDeg(quaty, -180.0f, y_axis); // y -180
  rot2quatDeg(quatx,  180.0f, x_axis); // x 180
  quat_multi(quatzy, quatz, quaty);
}

void
oclGetEyes(NUM eyeposL[3], NUM eyeposR[3], NUM headDir[3])
{
}

void
oclGetPosQuat(int idx, NUM pos[7])
{
  
}

void
oclGetPos(int idx, NUM pos[3])
{
}

void
oclGetPosEuler(int idx, NUM pos[6])
{
}

static void
myRotate(void)
{
  NUM qoffset[4], qoffset_1[4], qsensor[4], qsensor_1[4];
  NUM quattmp1[4], quattmp2[4], quattmp3[4], quattmp4[4];
  NUM matrix[16], euler[3];
  NUM posresult[4], pos[3];
  NUM eyeL[3], eyeR[3], eyeDir[3];
  NUM posoffset[3] = {0.2f, 1.7f, 0.2f};

  // sensor quat
  degreeEuler2quat(qsensor, &eyepos[3]);

  quat_inv(quatzy_1, quatzy);

  quat_multi(quattmp1, quatzy, qsensor);
  quat_multi(quattmp2, quattmp1, quatx);
  quat2euler(euler, quattmp2);

  // position conversion begin
  quat_posRotate(posresult, quatzy, eyepos);
  pos[0] = posresult[1] / 100.0f - posoffset[0];
  pos[1] = posresult[2] / 100.0f - posoffset[1];
  pos[2] = posresult[3] / 100.0f - posoffset[2];
  // position end

  calc_eyes(eyeL, eyeR, eyeDir, quattmp2, pos);
  renderEyeDir(eyeL, eyeR, eyeDir);
  
  {
    static int count = 0;
    if (count++ > 10) {
      count = 0;
      printf("%10.5f %10.5f %10.5f (%10.5f %10.5f %10.5f)\n", euler[0], euler[1], euler[2],
	     pos[0], pos[1], pos[2]);
    }
  }

  glTranslatef(pos[0], pos[1], pos[2]);
#if 1
#if 1
  // euler offset test
  glRotatef(euler[0], 0.0f, 0.0f, 1.0f);
  glRotatef(euler[1], 0.0f, 1.0f, 0.0f);
  glRotatef(euler[2], 1.0f, 0.0f, 0.0f);
#else
  quat_matrix(matrix, quattmp2);
  glMultMatrixf(matrix);
#endif
#endif
}
