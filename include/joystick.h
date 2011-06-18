/* OpenCABIN Library

   Copyright (C) 2006,2007,2008 KGT Inc.

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

struct ocl_jsstat {
        char button[32];
        int stick[6];
};

//void mjoystick(int no, int type, int number, int value, int time);

// type
#define OCL_JS_BUTTON 1
#define OCL_JS_STICK 2

// number
#define OCL_JS_BX 0	// x
#define OCL_JS_BC 1	// circle
#define OCL_JS_BB 2	// box
#define OCL_JS_BT 3	// triangle 
#define OCL_JS_BL1 4	// L1
#define OCL_JS_BL2 5	// L2
#define OCL_JS_BR1 6	// R1
#define OCL_JS_BR2 7	// R2
#define OCL_JS_BSE 8	// select
#define OCL_JS_BST 9	// start
#define OCL_JS_BJL 10	// Joystick Left Button
#define OCL_JS_BJR 11	// Joystick Left Button
#define OCL_JS_BUP 12	// up
#define OCL_JS_BRIGHT 13	// right
#define OCL_JS_BDOWN 14	// down
#define OCL_JS_BLEFT 15	// left

#define OCL_JS_BMAX 16	// maximum button number (plus one)

#define OCL_JS_SLX 0 //stick left x (plus right)
#define OCL_JS_SLY 1 //stick left y (plus down)
#define OCL_JS_SRX 2 //stick right x
#define OCL_JS_SRY 3 //stick right y
#define OCL_JS_S3X 4 //stick 3 x
#define OCL_JS_S3Y 5 //stick 3 y

#define OCL_JS_SMAX 6	// maximum stick number (plus one)

void oclJS_changeStatus(int no, int type, int number, int value, int time);
void oclJS_getStatus(int no, struct ocl_jsstat *status);
