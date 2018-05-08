/*
 * socket.h
 *
 *  Created on: May 7, 2018
 *      Author: fmocchetti
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>
#include <pthread.h>
#include <commons/log.h>

#define TRUE             1
#define FALSE            0

enum {	POOL, THREAD_CONNECTION, SELECT };

void configure_logger();
void create_server(int max_connections, int timeout, int server_type, int port);
void listen_on_poll(struct pollfd * fds, int max_connections, int timeout, int listen_sd);
void connection_thread();
void thread_on_connection(int listen_sd);

#endif /* SOCKET_H_ */
