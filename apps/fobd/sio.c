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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
//#include <asm/termios.h>
#include <termios.h>

#include "sio.h"

// #define DEFAULT_SPEED B2400
// #define DEFAULT_SPEED B4800
// #define DEFAULT_SPEED B9600
// #define DEFAULT_SPEED B19200
 #define DEFAULT_SPEED B38400
// #define DEFAULT_SPEED B57600
// #define DEFAULT_SPEED B115200

static speed_t g_speed = DEFAULT_SPEED;

int sio_fd = 0;
//static int sio_fd = 0;
static char sio_fn[BUFSIZ];
static struct termios sio_save_termios;

#ifndef LINUX
void
cfmakeraw(struct termios *termios_p)
{
           termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                            |INLCR|IGNCR|ICRNL|IXON);
            termios_p->c_oflag &= ~OPOST;
            termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
            termios_p->c_cflag &= ~(CSIZE|PARENB);
            termios_p->c_cflag |= CS8;
}

#endif

int
sio_init(char *fn, speed_t speed)
{
    struct termios new_termios;
    //strncpy(sio_fn, fn, BUFSIZ);
    strcpy(sio_fn, fn);

    if (-1 == (sio_fd = open(sio_fn, O_RDWR))) {
	perror(sio_fn);
	exit(1);
    }

    if (-1 == tcgetattr(sio_fd, &sio_save_termios)) {
	perror(sio_fn);
	fprintf(stderr, "An error occured while tcgettattr().");
	exit(1);
    }

    //new_termios = sio_save_termios;
    //printf("%d\n", sio_save_termios.c_cc[VMIN]);

    //new_termios = sio_save_termios;
    bzero(&new_termios, sizeof(struct termios));

    cfmakeraw(&new_termios);

#if 0
    cfsetispeed(&new_termios, g_speed); // 101
    cfsetospeed(&new_termios, g_speed);
#else
    cfsetispeed(&new_termios, speed);
    cfsetospeed(&new_termios, speed);
#endif

    new_termios.c_cflag |= CLOCAL | CREAD;
    new_termios.c_cc[VTIME] = 0;
    new_termios.c_cc[VMIN] = 1;

    if (-1 == tcsetattr(sio_fd, TCSANOW, &new_termios)) {
	perror(sio_fn);
	fprintf(stderr, "An error occured while tcsettattr().");
	//exit(1);
    }

    return sio_fd;
}

#if 0
void
sio_flush()
{
    if (-1 == tcflush(sio_fd, TCOFLUSH)) {
	perror("tcflush(TCOFLUSH)");
	fprintf(stderr, "An error occured at \"%s\"\n", sio_fn);
	sio_exit();
    }
}
#endif

void
sio_iflush()
{
    if (-1 == tcflush(sio_fd, TCIFLUSH)) {
	perror("tcflush(TCIFLUSH)");
	fprintf(stderr, "An error occured at \"%s\"\n", sio_fn);
	sio_exit();
    }
}

void
sio_exit()
{
#if 0
    // reoppen
    close(sio_fd);

    if (-1 == (sio_fd = open(sio_fn, O_RDWR))) {
	perror(sio_fn);
	exit(1);
    }
#endif
    
    if (-1 == tcsetattr(sio_fd, TCSANOW, &sio_save_termios)) {
	perror(sio_fn);
	fprintf(stderr, "An error occured at tcsettattr().\n");
	exit(1);
    }
    close(sio_fd);
}

void
sio_send(char *buf, int size)
{
    int retval;

    while (size > 0) {
	if (0 > (retval = write(sio_fd, buf, size))) {
	    perror("write()");
	    fprintf(stderr, "An error occured at \"%s\"\n", sio_fn);
	    sio_exit();
	}
	//fprintf(stderr, "retval: %d\n", retval);
	size -= retval;
	buf += retval;
	//sio_flush();
	usleep(0); // kernel may need re-schedule (?)
    }
}

static char sio_rbuf[BUFSIZ];
static int sio_rbuf_ind = 0;

void
sio_gets(char *buf, int bufsize)
{
    int retval, i;

    if (sio_rbuf_ind) {
	memcpy(buf, sio_rbuf, sio_rbuf_ind);
	bufsize -= sio_rbuf_ind;
	buf += sio_rbuf_ind;
	sio_rbuf_ind = 0;
    }

    while (bufsize > 0) {
	if (0 > (retval = read(sio_fd, buf, bufsize))) {
	    if ((errno == EINTR)||(errno == EBADF)) {
		//sio_send("c", 1);
		fprintf(stderr, "interrupted.\n");
		sio_exit();
		exit(1);
	    }
	    perror("read()");
	    fprintf(stderr, "An error occured at \"%s\" (errno = %d)\n", sio_fn, errno);
	    sio_exit();
	}
	//fprintf(stderr, "retval: %d\n", retval);
	for (i = 0; i < retval; i++) {
	    if (buf[i] == '\n') {
		memcpy(sio_rbuf, buf + i + 1, retval - i - 1);
		sio_rbuf_ind = retval - i - 1;
		buf[i + 1] = '\0';
		return;
	    }
	}
	bufsize -= retval;
	buf += retval;
	//usleep(0); // kernel may need re-schedule (?)
    }
};

void
sio_recv(char *buf, int bufsize)
{
    int retval, i;
    static struct timeval tv0 = {0, 0}, tv;
    if (sio_rbuf_ind) {
	fprintf(stderr, "sio_rbuf_ind: %d\n", sio_rbuf_ind);
	if (sio_rbuf_ind > bufsize) {
	    memcpy(buf, sio_rbuf, bufsize);
	    sio_rbuf_ind -= bufsize;
	    memcpy(sio_rbuf, sio_rbuf + bufsize, sio_rbuf_ind);
	    return;
	}
	    
	memcpy(buf, sio_rbuf, sio_rbuf_ind);
	bufsize -= sio_rbuf_ind;
	buf += sio_rbuf_ind;
	sio_rbuf_ind = 0;
    }

    //gettimeofday(&tv0, NULL);
    while (bufsize > 0) {
	if (0 > (retval = read(sio_fd, buf, bufsize))) {
	    if ((errno == EINTR)||(errno == EBADF)) {
		//sio_send("c", 1);
		fprintf(stderr, "interrupted.\n");
		sio_exit();
		exit(1);
	    }
	    perror("read()");
	    fprintf(stderr, "An error occured at \"%s\" (errno = %d)\n", sio_fn, errno);
	    sio_exit();
	}
#if 0
	gettimeofday(&tv, NULL);
	fprintf(stderr, "read(): %d bytes (%7d)\n", retval,
		(tv.tv_sec - tv0.tv_sec) * 1000000 + tv.tv_usec - tv0.tv_usec);
	tv0 = tv;
#endif
	bufsize -= retval;
	buf += retval;
	//usleep(0); // kernel may need re-schedule (?)
    }
};

void
sio_send_str(char *str)
{
    sio_send(str, strlen(str));
}

void
sio_putc(unsigned char c)
{
        //printf("sio_putc(%d)\n", c);
        switch (write(sio_fd, &c, 1)) {
        case -1:
                perror("write()");
                exit(1);
        case 0:
                fprintf(stderr, "write() returns 0\n");
                exit(1);
        }
}

unsigned char
sio_getc(void)
{
        unsigned char retval;
        switch (read(sio_fd, &retval, 1)) {
        case -1:
                perror("read()");
                exit(1);
        case 0:
                fprintf(stderr, "read() returns 0\n");
                exit(1);
        }
        return retval;
}

#ifndef TIOCM_RTS
#define TIOCM_RTS       0x004
#include <asm/ioctls.h>
#endif

void
rtsclear(int fd)
{
	int retval = TIOCM_RTS;
	if (fd <= 0) {
		  fd = sio_fd;
	}
	if (-1 == ioctl(fd, TIOCMBIC, &retval)) {
		perror("ioctl(TIOCMBIC)");
		exit(1);
	}
}

void
rtsset(int fd)
{
	int retval = TIOCM_RTS;
	if (fd <= 0) {
		  fd = sio_fd;
	}
	if (-1 == ioctl(fd, TIOCMBIS, &retval)) {
		perror("ioctl(TIOCMBIS)");
		exit(1);
	}
}

void
rtsget(void)
{
	int retval;

	if (-1 == ioctl(sio_fd, TIOCMGET, &retval)) {
	        perror("ioctl()");
		exit(1);
	}
	//printf("%x %d\n", retval, (retval & TIOCM_RTS) ? 1 : 0);

}

void
sio_flush(void)
{
	tcdrain(sio_fd);
}

