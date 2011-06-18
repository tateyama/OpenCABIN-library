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

#include "tatemcast.h"

struct sockaddr_in *
mc_dstNew(struct sockaddr_in *saptr, char *addr_str, int port)
{
    struct sockaddr_in *addr_ptr;

    memset((void *) saptr, 0, sizeof(struct sockaddr_in));
    saptr->sin_family  = AF_INET;
    saptr->sin_addr.s_addr = inet_addr(addr_str);
    saptr->sin_port        = htons(port);
    return saptr;
}

int
mcsender_init(char *addrstr, int port, unsigned char ttl, char *devaddrstr)
{
  int sockfd;
  struct in_addr inaddr;

  sockfd = mcrecver_init(addrstr, 0 /* port */, devaddrstr);
  setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(unsigned char));
  if (devaddrstr && (0 != inet_aton(devaddrstr, &inaddr))) {
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (void *) &inaddr, sizeof(struct in_addr));
  }

  return sockfd;
}

int
mcrecver_init(char *addrstr, int port, char *devaddr)
{
  struct sockaddr_in saddr;
  int sockfd;
  struct ip_mreq mreq;
  struct in_addr inaddr;

  memset(&saddr, 0, sizeof(struct sockaddr_in));
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);
  saddr.sin_family = AF_INET;

  if (-1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0))) {
    perror("socket");
    exit(1);
  }

  if (-1 == bind(sockfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in))) {
    perror("bind");
    exit(1);
  }

  mreq.imr_multiaddr.s_addr = inet_addr(addrstr);
//  if (devaddr && (0 != inet_aton(devaddr, &inaddr))) {
  if (devaddr) {
	 if (0 == inet_aton(devaddr, &inaddr)) {
		 perror(devaddr);
		 exit(1);
	}
	 mreq.imr_interface.s_addr = inet_addr(devaddr);
	 //printf("devaddr: %s\n", devaddr);
  } else {
	 mreq.imr_interface.s_addr = INADDR_ANY;
  }
  setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq, sizeof(struct ip_mreq));

  return sockfd;
}


int
mcread(int sockfd, unsigned char *buf, int bufsize)
{
  socklen_t len = 0;
  
  return recvfrom(sockfd, buf, bufsize, 0, NULL, &len);
}

int
mcwrite(int sockfd, struct sockaddr_in *addr_ptr, unsigned char *buf, int bufsize)
{
  return sendto(sockfd, buf, bufsize, 0,
		(struct sockaddr *)addr_ptr, sizeof(struct sockaddr_in));
}
