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
#include <stdarg.h>
#include <stdio.h>
#include <sys/errno.h>

void _myerror_init(void *);
void _myerror(int stat, char *str, va_list ap);
void _myperror(int stat, char *str, va_list ap);

void (*myerror_init_impl)(void *) = _myerror_init;
void (*myerror_impl)(int stat, char *str, va_list ap) = _myerror;
void (*myperror_impl)(int stat, char *str, va_list ap) = _myperror;

void
myerror_setfuncs(void (*initf)(void *),
		 void (*errorf)(int stat, char *str, va_list ap),
		 void (*perrorf)(int stat, char *str, va_list ap))
{
  if (0 != initf) {
    myerror_init_impl = initf;
  }
  if (0 != errorf) {
    myerror_impl = errorf;
  }
  if (0 != perrorf) {
    myerror_impl = perrorf;
  }
}

void
myerror_init(void *v)
{
  (*myerror_init_impl)(v);
}

void
myerror(int stat, char *str, ...)
{
  va_list ap;
  va_start(ap, str);
  (*myerror_impl)(stat, str, ap);
  va_end(ap);
}

void
myperror(int stat, char *str, ...)
{
  va_list ap;
  va_start(ap, str);
  (*myperror_impl)(stat, str, ap);
  va_end(ap);
}

void
_myerror_init(void *v)
{
}

void
_myerror(int stat, char *str, va_list ap)
{
  vfprintf(stderr, str, ap);
}

void
_myperror(int stat, char *str, va_list ap)
{
  vfprintf(stderr, str, ap);
  fprintf(stderr, "%s\n", strerror(errno));
}


#ifdef STANDALONE

void
testinit(void *a)
{
  printf("aoeu %d\n", a);
}

int
main()
{
  myerror_setfuncs(testinit, 0, 0);
  myerror_init(0);
  myerror(1, "test%d\n", 10);
  errno = 1;
  myperror(1, "test%d: ", 20);
}
#endif
