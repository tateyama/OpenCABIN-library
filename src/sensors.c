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

int oclSensorHead = OCL_SENSOR_HEAD;
//NUM posoffset[3] = {0.2f, 1.7f, 0.2f};
//NUM posoffset[3] = {-0.5f, 1.8f, -0.92f}; // CABIN
//NUM posoffset[3] = {1.54f, .3f, -1.06f}; // tsukuba
NUM posoffset[3] = OCL_FOB_POSOFFSET;


NUM *oclGetRawSensor(int idx);


//SHVAR(float *, eyepos);
//SHVARarray(float *, eyepos, 6);
static SHVARarray(NUM, sensor0, OCL_POSNUM);
static SHVARarray(NUM, sensor1, OCL_POSNUM);
static SHVARarray(NUM, sensor2, OCL_POSNUM);
static SHVARarray(NUM, sensor3, OCL_POSNUM);

static int sensorids[OCL_MAX_SENSORS];
static NUM *sensorvals[OCL_MAX_SENSORS];
static void (*funcs[OCL_MAX_SENSORS])();

#define sensors_createM(idx, NAME) \
  GSHVARINITarrayM(NAME); \
  sensorids[idx] = _SHVARID(NAME).id; \
  sensorvals[idx] = NAME; \
  sensorclear(NAME); \
  SHVARUPDATEbufLL(NAME)

static void
nullfunc(int idx, NUM val[])
{
  // do nothing
}

static void writeval(int idx,  NUM val[]);

int
sensors_write(int idx, NUM val[])
{
  (*(funcs[idx]))(idx, val);
}

static void
sensorclear(NUM *sensor)
{
  int i;

#if 0
  for (i = 0; i < OCL_POSNUM; ++i) {
    sensor[i] = 0.0f;
  }
  //sensor[3] = 1.0f; // quaternion
#else
  for (i = 0; i < 3; ++i) {
    sensor[i] = posoffset[i] * 100.0f;
  }
  sensor[3] = 1.0f;
  sensor[4] = 0.0f;
  sensor[5] = 0.0f;
  sensor[6] = 0.0f;
#endif
}

void
oclSensorActivate(int idx)
{
#if 0
  if (idx > 4) {
    fprintf(stderr, "sensor %d is not prepared in this code.\n", idx);
    return; 
  }
#endif
  funcs[idx] =  writeval;
}

int
sensors_initM(void)
{
  int i;

  sensors_createM(OCL_SENSOR0, sensor0);
  sensors_createM(OCL_SENSOR1, sensor1);
  sensors_createM(OCL_SENSOR2, sensor2);
  sensors_createM(OCL_SENSOR3, sensor3);

  for (i = 0; i < OCL_MAX_SENSORS; ++i) {
    funcs[i] = nullfunc;
  }

  oclSensorActivate(OCL_SENSOR0);
  oclSensorActivate(OCL_SENSOR1);
  oclSensorActivate(OCL_SENSOR2);
  oclSensorActivate(OCL_SENSOR3);
}

static NUM *
init_sensorC(int idx, char *name)
{
  _SHVARTAG tag;
  NUM *retval;

  retval = (NUM *)SHvarVdmlg_initC(&tag, name);
  sensorids[idx] = tag.id;
  sensorvals[idx] = retval;
  return retval;
}

static NUM z_axis[3] = {0.0, 0.0, 1.0};
static NUM y_axis[3] = {0.0, 1.0, 0.0};
static NUM x_axis[3] = {1.0, 0.0, 0.0};
static NUM quatx[4], quaty[4], quatz[4], quatzy[4], quatzy_1[4];
static NUM qoffset1[4], qoffset1_1[4];
static NUM qoffset2[4], qoffset2_1[4];

void
init_quat_consts()
{
  // offset quat
#if 0
  // Logo toward front screen
  rot2quatDeg(quatz, -90.0f, z_axis);  // z -90
  rot2quatDeg(quaty, -180.0f, y_axis); // y -180
  quat_multi(qoffset1, quatz, quaty);
#else
  // Logo toward right screen
  rot2quatDeg(qoffset1,  180.0f, x_axis); // x 180
#endif
  rot2quatDeg(qoffset2,  180.0f, x_axis); // x 180
  //quat_inv(qoffset1_1, qoffset1);
}

static void
writeval(int idx,  NUM val[])
{
#if 1
  // raw sensor values
  memcpy(sensorvals[idx], val, sizeof(NUM) * OCL_POSNUM);
  vdmlm_sendls(sensorids[idx]);
#else
  NUM pos[OCL_POSNUM];
  NUM qsensor[4], quattmp1[4], qresult[4];
  NUM posresult[3];

  // canonical sensor values
  degreeEuler2quat(qsensor, &val[3]);
  quat_multi(quattmp1, qoffset1, qsensor);
  quat_multi(qresult, quattmp1, qoffset2);

  quat_posRotate(posresult, qoffset1, eyeposraw);

  pos[0] = posresult[1] / 100.0f - posoffset[0];
  pos[1] = posresult[2] / 100.0f - posoffset[1];
  pos[2] = posresult[3] / 100.0f - posoffset[2];
  pos[3] = qresult[0];
  pos[4] = qresult[1];
  pos[5] = qresult[2];
  pos[6] = qresult[3];

  memcpy(sensorvals[idx], pos, sizeof(NUM) * OCL_POSNUM);
  vdmlm_sendls(sensorids[idx]);
#endif
}

static void
calcOrientationQuat(NUM *qresult, NUM *eyeOriEuler)
{
  //NUM *eyeposraw = oclGetRawSensor(idx);
  NUM qsensor[4], quattmp1[4];

  degreeEuler2quat(qsensor, eyeOriEuler);

  quat_multi(quattmp1, qoffset1, qsensor);
  quat_multi(qresult, quattmp1, qoffset2);
}

static void
calcPosition(NUM *posResult, NUM *rotquat, NUM *pos)
{
  NUM postmp[4]; // <- quaternion!

  quat_posRotate(postmp, rotquat, pos);

  posResult[0] = postmp[1] / 100.0f - posoffset[0];
  posResult[1] = postmp[2] / 100.0f - posoffset[1];
  posResult[2] = postmp[3] / 100.0f - posoffset[2];
}

// qmix = qoff(quatzy) * qsensor
static void
calc_eyes(NUM retL[3], NUM retR[3], NUM retDir[3], NUM qmix[3], NUM spos[3])
{
  NUM postmp1[4], pos[3];
#if 0
  NUM eyeOffsetL[3] = {0.05f, 0.01f, -0.045f};
  NUM eyeOffsetR[3] = {0.05f, 0.01f, -0.120f};
#else
  NUM eyeOffsetL[3] = {0.04f, 0.015f, -0.055f};
  NUM eyeOffsetR[3] = {0.04f, 0.015f, -0.120f};
#endif
  NUM eyeDir[3] = {1.0f, 0.0f, 0.0f};

  quat_posRotate(postmp1, qmix, eyeOffsetL);

  retL[0] = postmp1[1] + spos[0];
  retL[1] = postmp1[2] + spos[1];
  retL[2] = postmp1[3] + spos[2];

  quat_posRotate(postmp1, qmix, eyeOffsetR);

  retR[0] = postmp1[1] + spos[0];
  retR[1] = postmp1[2] + spos[1];
  retR[2] = postmp1[3] + spos[2];

  quat_posRotate(postmp1, qmix, eyeDir);

  retDir[0] = postmp1[1];
  retDir[1] = postmp1[2];
  retDir[2] = postmp1[3];
}

// === API ==================================================
void
oclGetEyes(int idx, NUM eyeposL[3], NUM eyeposR[3], NUM headDir[3])
{
  NUM qresult[4], pos[3];
  NUM *posraw;

  if (idx < 0) idx = oclSensorHead;

  posraw = oclGetRawSensor(idx);
  calcOrientationQuat(qresult, &posraw[3]);
  calcPosition(pos, qoffset1, posraw);
  calc_eyes(eyeposL, eyeposR, headDir, qresult, pos);
}

// === API ==================================================
// orientation quaternion
void
oclGetPosOriQuat(NUM pos[7], int idx)
{
  NUM qresult[4];
  NUM *posraw;

  posraw = oclGetRawSensor(idx);
  calcOrientationQuat(&pos[3], &posraw[3]);
  calcPosition(pos, qoffset1, posraw);
}

// === API ==================================================
void
oclGetPos(NUM pos[3], int idx)
{
  NUM *posraw;

  posraw = oclGetRawSensor(idx);
  calcPosition(pos, qoffset1, posraw);
}

// === API ==================================================
void
oclGetPosEuler(NUM pos[6], int idx)
{
  NUM *posraw;
  NUM qtmp[4], euler[3];

  posraw = oclGetRawSensor(idx);

  calcOrientationQuat(qtmp, &posraw[3]);

  //calcPosition(pos, qtmp, posraw);
  calcPosition(pos, qoffset1, posraw);
#if 0
  quat2euler(euler, qtmp);
  pos[3] = euler[0];
  pos[4] = euler[1];
  pos[5] = euler[2];
#else
  quat2euler(&pos[3], qtmp);
#endif
}

// === API ==================================================
void
oclRotateSensor(int idx)
{
  NUM matrix[16], qtmp[4], *posraw;

  if (idx < 0) idx = oclSensorHead;

  posraw = oclGetRawSensor(idx);
  calcOrientationQuat(qtmp, &posraw[3]);

  quat_matrix(matrix, qtmp);
  glMultMatrixf(matrix);
}

int
sensors_initC(void)
{
  init_quat_consts();
  init_sensorC(OCL_SENSOR0, "sensor0");
  init_sensorC(OCL_SENSOR1, "sensor1");
  init_sensorC(OCL_SENSOR2, "sensor2");
  init_sensorC(OCL_SENSOR3, "sensor3");
  oclSensorHead = OCL_SENSOR_HEAD;
}

void
oclGetSensor(NUM retval[], int idx)
{
  memcpy(retval, sensorvals[idx], sizeof(NUM) * OCL_POSNUM);

}

// master & client
NUM *
oclGetRawSensor(int idx)
{
  return sensorvals[idx];
}
