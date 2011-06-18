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
/* cabin protocol */

/* client type */
#define CLIENTTYPE_RENDERER 0xd4b86729

/* renderer comm */
#define RENDCMD_LOADAPP 0
#define RENDCMD_RUNAPP 1
#define RENDCMD_KILLPROC 2
#define RENDCMD_NEWMEM 3
#define RENDCMD_DELMEM 4
#define RENDCMD_MEMUPDATE 5
#define RENDCMD_NEWCH 6
#define RENDCMD_DELCH 7
#define RENDCMD_CHMESSAGE 8
//
#define RENDCMD_NEWSYMVAL 9
#define RENDCMD_SETSYMVAL 10
#define RENDCMD_SYMNAM 11
#define RENDCMD_VDML 12 // <- with size
#define RENDCMD_VDMLUPDATE 13 // <- without size
#define RENDCMD_DISPOFF 14
#define RENDCMD_DISPON 15
#define RENDCMD_EXIT 16
#define RENDCMD_ABORT 17
#define RENDCMD_MAX 18
