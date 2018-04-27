/*
 * socketServer.h
 *
 *  Created on: Apr 18, 2018
 *      Author: fmocchetti
 */

#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>
#include <commons/log.h>

#define SERVER_PORT  12345
#define TRUE             1
#define FALSE            0

void create_server(int max_connections, int timeout, char* identidad);
void configure_logger();

#endif /* SOCKETSERVER_H_ */
