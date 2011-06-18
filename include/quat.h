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

#if 0
typedef double NUM;
#else
typedef float NUM;
#endif
NUM *pos2quat(NUM retval[], NUM pos[]);
NUM *rot2quat(NUM retval[], NUM ang, NUM vec[]);
NUM *quat_inv(NUM retval[], NUM q[]);
NUM *quat_multi(NUM retval[], NUM q1[], NUM q2[]);
NUM *quat_rotate(NUM retval[], NUM quat[], NUM pos[]);
#ifdef OPENGL
void glMultQuatMatrixf(NUM quat[]);
#include <GL/gl.h>
typedef GLfloat MNUM;
#else
typedef NUM MNUM;
#endif
MNUM *quat_matrix(MNUM retval[], NUM quat[]);
NUM *euler2quat(NUM retval[], NUM euler[]);
NUM *degreeEuler2quat(NUM retval[], NUM euler[]);

NUM *quat2euler(NUM retval[], NUM quat[]);
unsigned char *quat_toString(unsigned char buf[], NUM quat[]);

NUM *rot2quatDeg(NUM retval[], NUM ang, NUM vec[]);
