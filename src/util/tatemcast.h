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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct sockaddr_in *mc_dstNew(struct sockaddr_in *, char *addr_str, int port);
int mcsender_init(char *addrstr, int port, unsigned char ttl, char *devaddrstr);
int mcrecver_init(char *addrstr, int port, char *devaddrstr);
int mcread(int sockfd, unsigned char *buf, int bufsize);
int mcwrite(int sockfd, struct sockaddr_in *serv_addr_ptr, unsigned char *buf, int bufsize);
