# OpenCABIN Library
#
#  Copyright (C) 2006,2007 KGT Inc.
#
#   This file is part of the OpenCABIN Library developed
#   under CnC project sponsored by National Institute of
#   Information and Communications Technology (NICT).
#
#   The OpenCABIN Library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   The OpenCABIN Library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU Lesser General Public License for more details.

CC=gcc -g

libutil.a: avlib.o rw.o inetbind.o tcpconnect.o myexit.o myerror.o tatemcast.o
	rm -f $@
	ar q $@ $^

avlib.o: avlib.h avlib.c

myexit.o: myexit.c myexit.h

rw.o: rw.c rw.h

clean:
	rm -f *.o *~ libutil.a
