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

#include <stdlib.h>
#include <termios.h>

extern int fob_addrmode;
extern int fob_maxaddrs;
extern int fob_sensors[126];
extern int fob_num_sensors;
extern int fob_num_birds;

void fob_open(char *fn, speed_t speed, int usec);
//void fob_open(int usec);
void fob_hemisphere(unsigned char axis, unsigned char sign);

static inline float
fob_uchars2pos(unsigned char c0, unsigned char c1)
{
  float f;
  unsigned int i1, i2, i;
	signed short *sptr;
	unsigned short s;
  s = 0xffff & (((unsigned int)c1 << 9) | (((unsigned int)c0 & 0x7f) << 2));
  //fprintf(stderr, "%02x %02x -> %04x\n", c1, c0, i);
  //f = *((short *)&s) * 144.0f / 32768.0f; // inch FoBman p.123
  f = *((short *)&s) * (144.0f * 2.54f) / 32768.0f; // centimeter
  return f;
}

static inline float
fob_uchars2ang(unsigned char c0, unsigned char c1)
{
  float f;
  unsigned int i1, i2, i;
	signed short *sptr;
	unsigned short s;
#if 0
  s = 0xffff & (((unsigned int)c1 << 9) | (((unsigned int)c0 & 0x7f) << 1));
  f = *((short *)&s) * 180.0f / 32768.0f; // degree
  //fprintf(stderr, "%02x %02x -> %04x\n", c1, c0, i);
#else
  s = 0xffff & ((((unsigned int)c1 & 0x7f) << 9) | (((unsigned int)c0 & 0x7f) << 2));
  f = *((short *)&s) * 180.0f / 32768.0f; // degree
#endif
  return f;
}

void fob_exit(void);
void fob_RS232toFOB(unsigned char addr);
void fob_autoconfig(char *hemisphere);
void fob_run(void);
void fob_loop(void);
unsigned char fob_errorcode(void);
int fob_doit(float p[]);
