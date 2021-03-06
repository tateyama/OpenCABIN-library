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

#include <sys/time.h>
#include <stdlib.h>
#include <termios.h>

int sio_init(char *fn, speed_t speed);
void sio_exit();
void sio_flush();
void sio_iflush();
void sio_send(char *buf, int size);
void sio_send_str(char *str);
void sio_recv(char *buf, int bufsize);
void sio_gets(char *buf, int bufsize);
void sio_putc(unsigned char c);
unsigned char sio_getc(void);
void rtsclear(int fd);
