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

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
//#include <termios.h>
#include <asm/termios.h>
#include <oclconf.h>

int
main(int ac, char **av)
{
	int fd = open(OCL_FOB_DEVICE, O_RDWR);
	int retval = TIOCM_RTS;

	if (fd == -1) exit(1);
	if (-1 == ioctl(fd, TIOCMBIS, &retval)) {
		perror("ioctl()");
		exit(1);
	}
	//printf("RTS set\n");
	printf("fob down\n");
	close(fd);
}
