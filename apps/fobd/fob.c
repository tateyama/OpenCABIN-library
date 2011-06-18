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
#include <sio.h>
#include <fob.h>
#include <sys/time.h>

int fob_addrmode = -1;
int fob_maxaddrs = 0;
int fob_sensors[126];
int fob_num_sensors = 0;
int fob_num_birds = 0;

void fob_flush(void);

void
loop_count(void)
{
	static int count = -1;
	static int unit = 10;
	static struct timeval tv0;

	if (-1 == count) {
		gettimeofday(&tv0, NULL);
	}

	++count;
	if (count > unit) {
		struct timeval tv;
		long usec;
		float fps;

		gettimeofday(&tv, NULL);
		usec = (tv.tv_sec - tv0.tv_sec) * 1000000 + tv.tv_usec - tv0.tv_usec;
		fps = (float)count * 1000000.0f / (float)usec;
		printf("\n%f fps (unit: %d)\n", fps, unit);
		unit = (int)(fps);
		count = 0;
		tv0 = tv;
	}
}

unsigned char
fob_errorcode(void)
{
	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(0x0a); // Error Code (FoBman p.119, p135, p149)
	return sio_getc();
}

void
fob_fbbreset(void)
{
	sio_putc(0x2F); // FBB Reset (FoBman p.75)
}

void
fob_stream_stop(void)
{
	sio_putc(0x3F); // stream stop (FoBman p.103)
}

void
fob_hemisphere(unsigned char axis, unsigned char sign)
{
	sio_putc(0x4C); // stream stop (FoBman p.76)
	sio_putc(axis);
	sio_putc(sign);
}


void
fob_status_print(void)
{
	unsigned int retval1, retval2, retval, status;
	int num, i;

	// Bird Status
	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(0); // Bird Status (FoBman p.112)
	retval1 = sio_getc();
	retval2 = sio_getc();
	status = ((retval2 & 0xff) << 8) | (retval1 & 0xff);
	printf("status: %04x\n", status);
	printf("master?:  %c\n", (status & 0x8000) ? 'O' : 'X');
	printf("init?:    %c\n", (status & 0x4000) ? 'O' : 'X');
	printf("error?:   %c\n", (status & 0x2000) ? 'O' : 'X');
	printf("running?: %c\n", (status & 0x1000) ? 'O' : 'X');
	printf("hostsync?: %c\n", (status & 0x800) ? 'O' : 'X');
	printf("expanded address mode?: %c\n", (status & 0x400) ? 'O' : 'X');
	printf("crtSync?: %c\n", (status & 0x200) ? 'O' : 'X');
	printf("noSync?: %c\n", (status & 0x100) ? 'O' : 'X');
	printf("factory test?: %c\n", (status & 0x80) ? 'O' : 'X');
	printf("XOFF?: %c\n", (status & 0x40) ? 'O' : 'X');
	printf("sleep?: %c\n", (status & 0x20) ? 'O' : 'X');
	printf("stream?: %c\n", (status & 0x1) ? 'O' : 'X');

	// Flock System Status

	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(36); // Flock System Status mode (FoBman p.130)

	printf("Flock System Status (max %d birds)\n", fob_maxaddrs);
	for (i = 0; i < fob_maxaddrs; ++i) {
		retval = sio_getc();
		printf("%3d: %s, %s, %s, %s, %s, %s, %s, %s\n", i,
				(retval & 0x80) ? "ACCESSIBLE" : "not accessible",
				(retval & 0x40) ? "RUNNING" : "not running",
				(retval & 0x20) ? "a SENSOR exists" : "no sensors",
				(retval & 0x10) ? "ERT" : "not ert",
				(retval & 0x8) ? "ERT#3" : "no ert#3",
				(retval & 0x4) ? "ERT#2" : "no ert#2",
				(retval & 0x2) ? "ERT#1" : "no ert#1",
				(retval & 0x1) ? "ERT#0 or SRT" : "no ert#0 nor srt");
	}
}

void
fob_open(char *fn, speed_t speed, int usec)
{
	int fd;
	//char *fn = "/dev/ttyS0";
	unsigned int retval1, retval2, retval, status;
	int num, i;
	char strbuf[1024], *str;

	if (-1 == (fd = open(fn, O_RDONLY))) {
		perror(fn);
		exit(1);
	}
	rtsclear(fd);

	close(fd);
	usleep(usec);
	fd = sio_init(fn, speed);
	rtsclear(0);

	// Bird Status
	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(0); // Bird Status (FoBman p.112)
	retval1 = sio_getc();
	retval2 = sio_getc();
	status = ((retval2 & 0xff) << 8) | (retval1 & 0xff);
#if 0
	printf("status: %04x\n", status);
	printf("master?:  %c\n", (status & 0x8000) ? 'O' : 'X');
	printf("init?:    %c\n", (status & 0x4000) ? 'O' : 'X');
	printf("error?:   %c\n", (status & 0x2000) ? 'O' : 'X');
	printf("running?: %c\n", (status & 0x1000) ? 'O' : 'X');
	printf("hostsync?: %c\n", (status & 0x800) ? 'O' : 'X');
	printf("expanded address mode?: %c\n", (status & 0x400) ? 'O' : 'X');
	printf("crtSync?: %c\n", (status & 0x200) ? 'O' : 'X');
	printf("noSync?: %c\n", (status & 0x100) ? 'O' : 'X');
	printf("factory test?: %c\n", (status & 0x80) ? 'O' : 'X');
	printf("XOFF?: %c\n", (status & 0x40) ? 'O' : 'X');
	printf("sleep?: %c\n", (status & 0x20) ? 'O' : 'X');
	printf("stream?: %c\n", (status & 0x1) ? 'O' : 'X');
#endif
	if (0 == (status & 0x8000)) {
		fprintf(stderr, "FoB Hardware Config Error: not master\n");
		exit(1);
	}

	// FBB Addressing mode
	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(19); // FBB Addressing mode (FoBman p.123)
	retval1 = sio_getc();
	str = "Unknown";
	switch (retval1) {
	case 0:
		str = "Normal";
		num = 14;
		break;
	case 1:
		str = "Expanded";
		num = 30;
		break;
	case 3:
		str = "SuperExpanded";
		num = 126;
		break;
	default:
		fprintf(stderr, "FATAL ERROR: unknown addressing mode\n");
		exit(1);
		break;
	}

	fob_addrmode = retval1;
	fob_maxaddrs = num;
	fob_num_sensors = 0;

	printf("FBB addressing mode: %s (%x)\n", str, retval1);


	// Flock System Status

	sio_putc(0x4F); // Examine Value (FoBman p.73)
	sio_putc(36); // Flock System Status mode (FoBman p.130)

	//printf("Flock System Status (max %d birds)\n", num);
	for (i = 0; i < num; ++i) {
		retval = sio_getc();
#if 0
		printf("%3d: %s, %s, %s, %s, %s, %s, %s, %s\n", i,
				(retval & 0x80) ? "ACCESSIBLE" : "not accessible",
				(retval & 0x40) ? "RUNNING" : "not running",
				(retval & 0x20) ? "a SENSOR exists" : "no sensors",
				(retval & 0x10) ? "ERT" : "not ert",
				(retval & 0x8) ? "ERT#3" : "no ert#3",
				(retval & 0x4) ? "ERT#2" : "no ert#2",
				(retval & 0x2) ? "ERT#1" : "no ert#1",
				(retval & 0x1) ? "ERT#0 or SRT" : "no ert#0 nor srt");
#endif
		if (retval & 0x80) {
			++fob_num_birds;
		}
		if (retval & 0x20) {
			fob_sensors[fob_num_sensors] = i + 1;
			++fob_num_sensors;
		}
	}

#if 0
	printf("fob_addrmode: %d\n", fob_addrmode);
	printf("fob_maxaddrs: %d\n", fob_maxaddrs);
	printf("fob_num_sensors: %d\n", fob_num_sensors);
	for (i = 0; i < fob_num_sensors; ++i) {
		printf("fob_sensors[%d]: %d\n", i, fob_sensors[i]);
	}
#endif
	printf("fob_num_birds: %d\n", fob_num_birds);
}

void
fob_exit()
{
	fob_stream_stop();
	fob_flush();
	rtsset(0);
	fob_fbbreset();
	fob_flush();
	sio_exit();
};

void
fob_RS232toFOB(unsigned char addr)
{
	sio_putc(0xf0 | addr); 
}

void
fob_autoconfig(char *hemisphere)
{
	int i;
	unsigned char axis = 0, sign = 0;

	sio_putc(0x50); // Change Value
	sio_putc(0x32); // FBB Auto-Configuration p.131
	sio_putc(fob_num_birds);
	//sio_putc(3);
	sio_flush();

	if (hemisphere) {
		switch (hemisphere[0]) {
		case 'f':
			break;
		case 'l':
			axis = 6; sign = 1;
			break;
		default:
			break;
		}
	}

	usleep(700000);

	for (i = 0; i < fob_num_sensors; ++i) {
		fob_RS232toFOB(fob_sensors[i]);
		sio_putc(0x59); // Position/Angles (FoBman p.90)
		fob_RS232toFOB(fob_sensors[i]);
		fob_hemisphere(axis, sign);
	}

	sio_putc(0x50); // Change Value
	sio_putc(0x23); // Group mode (p.129);
	sio_putc(1);
}

void
fob_stream(void)
{
	sio_putc(0x40); // Stream (p.101)
}

void
fob_parse1(float p[])
{
}

int
fob_doit(float p[])
{
  unsigned short s0;

  //float p[16];
  int c0, c1, i;
  static int count = 0, c00 = 0;

  // FoB manual p.40 Response Format

  while (0 == (0x80 & (c0 = sio_getc()))) {
    printf("[%02x] ", c0);
    //putchar('.');
    fflush(stdout);
  };

  c1 = sio_getc();
  p[0] = fob_uchars2pos(c0, c1);
  //printf("%02x %02x ", c0, c1);
  for (i = 1; i < 3; ++i) {
    c0 = sio_getc();
    c1 = sio_getc();
    p[i] = fob_uchars2pos(c0, c1);
    //printf("%02x %02x ", c0, c1);
  }
  for (i = 3; i < 6; ++i) {
    c0 = sio_getc();
    c1 = sio_getc();
    p[i] = fob_uchars2ang(c0, c1);
    //printf("%02x %02x ", c0, c1);
  }
  c0 = sio_getc();
  ++count;
#if 0
  if (count > 30) {
    if (c00 >= c0) {
      putchar(0x1b); putchar('8'); // restore cursor
      //puts("                                                  ");
      loop_count();
      putchar(0x1b); putchar('8'); // restore cursor
    }
    printf("(%d) %10.5f %10.5f %10.5f %10.5f %10.5f %10.5f ",
	   c0, p[0], p[1], p[2], p[3], p[4], p[5]);
    fflush(stdout);
    if (count > 31) {
      count = 0;
    }
  }
#endif
  c00 = c0;
  return c0;
}

void
fob_loop(void)
{
	float p[16];
	int ind;

  fob_stream();
  //putchar(0x1b); putchar('7'); // save cursor

  while (1) {
    ind = fob_doit(p);
  }
}

void
fob_flush(void)
{
	sio_flush();
}
