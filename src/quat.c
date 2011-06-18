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

/*#define BUGGY /**/

#include <math.h>
#include <stdio.h>

#include "quat.h"

/*
 * q = (s, v) = (s, vx, vy, vz)
 * q1 * q2 = (s1 * s2 - v1.v2, s1v2 + s2v1 + v1 * v2)
 */

NUM *
pos2quat(NUM retval[], NUM pos[])
{
    retval[0] = 0.0;
    retval[1] = pos[0];
    retval[2] = pos[1];
    retval[3] = pos[2];
    return retval;
}

NUM *
rot2quat(NUM retval[], NUM ang, NUM vec[])
{
  NUM ang2 = ang / 2.0;
  NUM s = (NUM)sin(ang2);
  retval[0] = (NUM)cos(ang2);
  retval[1] = vec[0] * s;
  retval[2] = vec[1] * s;
  retval[3] = vec[2] * s;
  return retval;
}

NUM *
rot2quatDeg(NUM retval[], NUM ang, NUM vec[])
{
  return rot2quat(retval, ang * M_PI / 180.0, vec);
}


NUM *
quat_inv(NUM retval[], NUM q[])
{
    retval[0] = q[0];
    retval[1] = -q[1];
    retval[2] = -q[2];
    retval[3] = -q[3];
    return retval;
}

NUM *
quat_multi(NUM retval[], NUM q1[], NUM q2[])
{
    retval[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
    retval[1] = q1[0] * q2[1] + q2[0] * q1[1] + q1[2] * q2[3] - q1[3] * q2[2];
    retval[2] = q1[0] * q2[2] + q2[0] * q1[2] + q1[3] * q2[1] - q1[1] * q2[3];
    retval[3] = q1[0] * q2[3] + q2[0] * q1[3] + q1[1] * q2[2] - q1[2] * q2[1];
    return retval;
}

NUM *
quat_posRotate(NUM retval[], NUM quat[], NUM pos[])
{
    NUM qpos[4];
    NUM qinv[4];
    NUM qtmp[4];
    
    return quat_multi(retval, quat, quat_multi(qtmp, pos2quat(qpos, pos), quat_inv(qinv, quat)));
}

NUM *
quat_quatRotate(NUM retval[], NUM rotQuat[], NUM quat[])
{
    NUM qpos[4];
    NUM qinv[4];
    NUM qtmp[4];
    
    return quat_multi(retval, rotQuat, quat_multi(qtmp, quat, quat_inv(qinv, rotQuat)));
}

#ifdef OPENGL
void
glMultQuatMatrixf(NUM quat[])
{
  GLfloat matbuf[16];
  glMultMatrixf(quat_matrix(matbuf, quat));
}
#endif

MNUM *
quat_matrix(MNUM retval[], NUM quat[])
{
  NUM w2 = quat[0] * quat[0];
  NUM x2 = quat[1] * quat[1];
  NUM y2 = quat[2] * quat[2];
  NUM z2 = quat[3] * quat[3];
  NUM wx = quat[0] * quat[1];
  NUM wy = quat[0] * quat[2];
  NUM wz = quat[0] * quat[3];
  NUM xy = quat[1] * quat[2];
  NUM xz = quat[1] * quat[3];
  NUM yz = quat[2] * quat[3];
  
  retval[0] = w2 + x2 - y2 -z2;
  retval[1] = xy + xy + wz + wz;
  retval[2] = xz + xz - wy - wy;
  retval[3] = 0.0;
  
  retval[4] = xy + xy - wz - wz;
  retval[5] = w2 - x2 + y2 - z2;
  retval[6] = yz + yz + wx + wx;
  retval[7] = 0.0;
  
  retval[8] = xz + xz + wy + wy;
  retval[9] = yz + yz - wx - wx;
  retval[10] = w2 - x2 - y2 + z2;
  retval[11] = 0.0;
  
  retval[12] = 0.0;
  retval[13] = 0.0;
  retval[14] = 0.0;
  retval[15] = w2 + x2 + y2 + z2;
  
  return retval;
}

#if 0
#ifdef RY
        /* CAVE yokodorid-RY (performer way?) */
    glRotatef(pos[3], 0.0, 0.0, 1.0);
    glRotatef(pos[4], 1.0, 0.0, 0.0);
    glRotatef(pos[5], 0.0, 1.0, 0.0);
#else
        /* COSMOS, CABIN */
    glRotatef(pos[3], 0.0, 0.0, 1.0);
    glRotatef(pos[4], 0.0, 1.0, 0.0);
    glRotatef(pos[5], 1.0, 0.0, 0.0);
#endif
#endif

#if 0
#ifndef RY
#define RY /**/
#endif
#endif

NUM *
euler2quat(NUM retval[], NUM euler[])
{
    static NUM z_axis[3] = {0.0, 0.0, 1.0};
    static NUM y_axis[3] = {0.0, 1.0, 0.0};
    static NUM x_axis[3] = {1.0, 0.0, 0.0};
    NUM qtmp1[4];
    NUM qtmp2[4];
    NUM qtmp3[4];

    rot2quat(qtmp1, euler[0], z_axis);

#ifdef RY
    rot2quat(qtmp2, euler[1], x_axis);
#else
    rot2quat(qtmp2, euler[1], y_axis);
#endif

    quat_multi(qtmp3, qtmp1, qtmp2);

#ifdef RY
    rot2quat(qtmp1, euler[2], y_axis);
#else
    rot2quat(qtmp1, euler[2], x_axis);
#endif

    quat_multi(retval, qtmp3, qtmp1);

    return retval;
}

NUM *
degreeEuler2quat(NUM retval[], NUM euler[])
{
    NUM rad[3];

    rad[0] = euler[0] * M_PI / 180.0;
    rad[1] = euler[1] * M_PI / 180.0;
    rad[2] = euler[2] * M_PI / 180.0;
    
    return euler2quat(retval, rad);
}


//#include "quat.h"
//#include <math.h>

NUM *
quat2euler(NUM euler[3], NUM quat[4])
{
  // azimuth z
  NUM tmp = 
  euler[0] = (NUM)atan2(2.0 * (quat[0] * quat[3] + quat[1] * quat[2]),
			(1.0 - 2.0 * (quat[2] * quat[2] + quat[3] * quat[3]))
			) * 180.0 / M_PI;
  // elevation y
  euler[1] = (NUM)asin(2.0 * (quat[0] * quat[2] - quat[3] * quat[1])) * 180.0 / M_PI;
  
  // roll x
  tmp = 
  euler[2] = (NUM)atan2(2.0 * (quat[0] * quat[1] + quat[2] * quat[3]),
			(1.0 - 2.0 * (quat[1] * quat[1] + quat[2] * quat[2]))
			) * 180.0 / M_PI;
  return euler;
}

NUM *
quat2euler2(NUM euler[3], NUM quat[4])
{
  // azimuth z
  NUM tmp = (1.0 - 2.0 * (quat[2] * quat[2] + quat[3] * quat[3]));
  if (tmp < 1e-5) tmp = 1e-5;
  euler[0] = (NUM)atan2(2.0 * (quat[0] * quat[3] + quat[1] * quat[2]), tmp) * 180.0 / M_PI;

  // elevation y
  euler[1] = (NUM)asin(2.0 * (quat[0] * quat[2] - quat[3] * quat[1])) * 180.0 / M_PI;
  
  // roll x
  tmp = 1.0 - 2.0 * (quat[1] * quat[1] + quat[2] * quat[2]);
  if (tmp < 1e-5) tmp = 1e-5;

  euler[2] = (NUM)atan2(2.0 * (quat[0] * quat[1] + quat[2] * quat[3]), tmp) * 180.0 / M_PI;
  return euler;
}

unsigned char *
quat_toString(unsigned char buf[], NUM quat[])
{
    sprintf(buf, "(%f, %f, %f, %f)", quat[0], quat[1], quat[2], quat[3]);
    return buf;
}

/*
 *  0  4  8 12  0
 *  1  5  9 13  1
 *  2  6 10 14  2
 *  3  7 11 15  3
 */

static NUM *
mat_v(NUM *result_v, NUM *mat, NUM *in_v)
{
    result_v[0] = mat[0] * in_v[0] + mat[4] * in_v[1] + mat[8] * in_v[2] + mat[12] * 1.0;
    result_v[1] = mat[1] * in_v[0] + mat[5] * in_v[1] + mat[9] * in_v[2] + mat[13] * 1.0;
    result_v[2] = mat[2] * in_v[0] + mat[6] * in_v[1] + mat[10] * in_v[2] + mat[14] * 1.0;
    result_v[3] = 1.0;
    result_v[4] = 0.0;
    result_v[5] = 0.0;
    result_v[6] = 0.0;

    return result_v;
}


/*
 *  0  4  8 12  0  4  8 12
 *  1  5  9 13  1  5  9 13
 *  2  6 10 14  2  6 10 14
 *  3  7 11 15  3  7 11 15
 */

static NUM *
mat_multi(NUM *result, NUM *mat1, NUM *mat2)
{
    result[0] = mat1[0] * mat2[0] + mat1[4] * mat2[1] + mat1[8] * mat2[2] + mat1[12] * mat2[3];
    result[4] = mat1[0] * mat2[4] + mat1[4] * mat2[5] + mat1[8] * mat2[6] + mat1[12] * mat2[7];
    result[8] = mat1[0] * mat2[8] + mat1[4] * mat2[9] + mat1[8] * mat2[10] + mat1[12] * mat2[11];
    result[12] = mat1[0] * mat2[12] + mat1[4] * mat2[13] + mat1[8] * mat2[14] + mat1[12] * mat2[15];

    result[1] = mat1[1] * mat2[0] + mat1[5] * mat2[1] + mat1[9] * mat2[2] + mat1[13] * mat2[3];
    result[5] = mat1[1] * mat2[4] + mat1[5] * mat2[5] + mat1[9] * mat2[6] + mat1[13] * mat2[7];
    result[9] = mat1[1] * mat2[8] + mat1[5] * mat2[9] + mat1[9] * mat2[10] + mat1[13] * mat2[11];
    result[13] = mat1[1] * mat2[12] + mat1[5] * mat2[13] + mat1[9] * mat2[14] + mat1[13] * mat2[15];

    result[2] = mat1[2] * mat2[0] + mat1[6] * mat2[1] + mat1[10] * mat2[2] + mat1[14] * mat2[3];
    result[6] = mat1[2] * mat2[4] + mat1[6] * mat2[5] + mat1[10] * mat2[6] + mat1[14] * mat2[7];
    result[10] = mat1[2] * mat2[8] + mat1[6] * mat2[9] + mat1[10] * mat2[10] + mat1[14] * mat2[11];
    result[14] = mat1[2] * mat2[12] + mat1[6] * mat2[13] + mat1[10] * mat2[14] + mat1[14] * mat2[15];

    result[3] = mat1[3] * mat2[0] + mat1[7] * mat2[1] + mat1[11] * mat2[2] + mat1[15] * mat2[3];
    result[7] = mat1[3] * mat2[4] + mat1[7] * mat2[5] + mat1[11] * mat2[6] + mat1[15] * mat2[7];
    result[11] = mat1[3] * mat2[8] + mat1[7] * mat2[9] + mat1[11] * mat2[10] + mat1[15] * mat2[11];
    result[15] = mat1[3] * mat2[12] + mat1[7] * mat2[13] + mat1[11] * mat2[14] + mat1[15] * mat2[15];

    return result;
}

/*
 *  0  4  8 12
 *  1  5  9 13
 *  2  6 10 14
 *  3  7 11 15
 */

static NUM *
mat_trans(NUM *result, NUM *pos)
{
    result[0] = result[5] = result[10] = result[15] = 1.0;
    result[1] = result[2] = result[3] = result[4] = result[6]
	= result[7] = result[8] = result[9] = result[11] = 0.0;
    result[12] = pos[0];
    result[13] = pos[1];
    result[14] = pos[2];
    return result;
}

static NUM *
pos_inv(NUM *result, NUM *pos)
{
  result[0] = -pos[0];
  result[1] = -pos[1];
  result[2] = -pos[2];
  return result;
}

static NUM *
pos2csmat(NUM *result, NUM *pos)
{
    NUM mat1[16], mat2[16], pos1[3], qtmp[4];
    /*
    return mat_multi(result, mat_trans(mat1, pos), quat_matrix(mat2, &pos[3]));
    */
    return mat_multi(result, mat_trans(mat1, pos_inv(pos1, pos)),
		     quat_matrix(mat2, quat_inv(qtmp, &pos[3])));
}

void
cs_conversion(NUM *newpos, NUM *orgpos, NUM *target_cs)
{
    static NUM mat[16];

    pos2csmat(mat, target_cs);
    mat_v(newpos, mat, orgpos);
}

#ifdef QUAT_TEST_ONLY
#if 1

NUM *
vec_multi(NUM retval[], NUM v1[], NUM v2[])
{
    retval[0] = v1[1] * v2[2] - v1[2] * v2[1];
    retval[1] = v1[2] * v2[0] - v1[0] * v2[2];
    retval[2] = v1[0] * v2[1] - v1[1] * v2[0];

    return retval;
}

unsigned char *
vec_toString(unsigned char buf[], NUM vec[])
{
    sprintf(buf, "(%f, %f, %f)", vec[0], vec[1], vec[2]);
    return buf;
}

#endif

NUM quat_nin_start[4];
NUM quat_nin_current[4];
NUM quat_obj_start[4];
NUM quat_obj_current[4];

void
set(NUM q[], NUM p[])
{
    q[0] = p[0];
    q[1] = p[1];
    q[2] = p[2];
    q[3] = p[3];
}

void
press()
{
    set(quat_nin_start, quat_nin_current);
    set(quat_obj_start, quat_obj_current);
}

void
move()
{
    NUM qtmp[4];
    NUM q_d[4];

    quat_inv(qtmp, quat_nin_start);
    quat_multi(q_d, quat_nin_start, qtmp);
    quat_multi(quat_obj_current, q_d, quat_obj_start);
}

void
click()
{
    press();
    move();
}

int
main2(int ac, char *av[])
{
    NUM y_axis[3] = {0.0, 1.0, 0.0};
    unsigned char strbuf1[BUFSIZ];
    unsigned char strbuf2[BUFSIZ];
    unsigned char strbuf3[BUFSIZ];

    NUM qtmp0[4];
    NUM qtmp1[4];
    NUM qtmp2[4];

    puts(quat_toString(strbuf1, rot2quat(qtmp0, 0.0, y_axis)));
    puts(quat_toString(strbuf1, quat_inv(qtmp1, qtmp0)));
    puts(quat_toString(strbuf1, quat_multi(qtmp2, qtmp0, qtmp1)));
    puts(quat_toString(strbuf1, quat_multi(qtmp2, qtmp1, qtmp0)));

    return 0;
}

int
main(int ac, char *av[])
{
    NUM y_axis[3] = {0.0, 1.0, 0.0};
    unsigned char strbuf1[BUFSIZ];
    
    puts(quat_toString(strbuf1, rot2quat(quat_obj_current, 0.0, y_axis)));
    puts(quat_toString(strbuf1, rot2quat(quat_nin_current, 0.0, y_axis)));
    click();
    puts(quat_toString(strbuf1, quat_obj_current));
    return 0;
}
#endif
