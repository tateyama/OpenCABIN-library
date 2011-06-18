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

//#define OCL_MULTICAST_NETDEV "10.1.0.10"
//#define OCL_MULTICAST_NETDEV "130.158.77.124" // multicast network device address (if there are 2 or more network devices)

#define OCL_SCREEN OCL_FRONT
//#define OCL_SCREEN OCL_LEFT
//#define OCL_SCREEN OCL_RIGHT
//#define OCL_SCREEN OCL_BOTTOM
//#define OCL_SCREEN OCL_TOP

#define OCL_FULLSCREEN 1
//#define OCL_FULLSCREEN 0

//#define OCL_STEREO 0 // mono
//#define OCL_STEREO 1 // active stereo (quadbuffer)
#define OCL_STEREO 2 // passive stereo (2 viewports)

#define OCL_VIEWPORT_LEFT_X 1152
#define OCL_VIEWPORT_LEFT_Y 0
#define OCL_VIEWPORT_LEFT_WIDTH 768
#define OCL_VIEWPORT_LEFT_HEIGHT 768

#define OCL_VIEWPORT_RIGHT_X 128
#define OCL_VIEWPORT_RIGHT_Y 0
#define OCL_VIEWPORT_RIGHT_WIDTH OCL_VIEWPORT_LEFT_WIDTH
#define OCL_VIEWPORT_RIGHT_HEIGHT OCL_VIEWPORT_LEFT_HEIGHT

#define OCL_FRONT_WIDTH 2.0f
#define OCL_FRONT_HEIGHT 2.0f
#define OCL_LEFT_WIDTH 2.0f
#define OCL_BOTTOM_HEIGHT 2.0f
#define OCL_FRONT_ORG_DIST_DEPTH 1.0f
#define OCL_FRONT_ORG_DIST_LEFT 1.0f

//#define OCL_FOB_POSOFFSET {0.2f, 1.7f, 0.2f}
//#define OCL_FOB_POSOFFSET {-0.5f, 1.8f, -0.92f} /* CABIN */
//#define OCL_FOB_POSOFFSET {1.54f, .3f, -1.06f} /* tsukuba */
#define OCL_FOB_POSOFFSET {0.94f, 1.59f, -1.35f} /* kyoto-u CAVE CS*/

#define OCL_FOB_SPEED B38400
//#define OCL_FOB_SPEED B115200
#define OCL_FOB_DEVICE "/dev/ttyS0"
#define OCL_FOB_HEMISPHERE "left"
//#define OCL_FOB_HEMISPHERE "front"

#define OCL_NEAR_DEFAULT 0.01
#define OCL_FAR_DEFAULT 10.0
//#define OCL_FAR_DEFAULT 200.0
