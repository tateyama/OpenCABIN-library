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

// config
#define OCL_MAX_SENSORS 16

#define OCL_SENSOR0 2 /* FBB ID */
#define OCL_SENSOR1 3
#define OCL_SENSOR2 4
#define OCL_SENSOR3 5

// params
#define OCL_MAX_SENSORS_DEFAULT 16

#ifndef OCL_SENSOR_HEAD
#define OCL_SENSOR_HEAD OCL_SENSOR0
#endif

#ifndef OCL_SENSOR0
#define OCL_SENSOR0 2
#endif

#ifndef OCL_SENSOR1
#define OCL_SENSOR1 3
#endif
