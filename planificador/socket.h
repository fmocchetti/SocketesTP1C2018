/*
 * socketServer.h
 *
 *  Created on: Apr 18, 2018
 *      Author: fmocchetti/pdelucchi
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
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include <commons/log.h>
#include "protocolo.h"
#include "algoritmos.h"
#include <semaphore.h>
#include <pthread.h>
#include <commons/config.h>

//#define SERVER_PORT  12345
#define TRUE             1
#define FALSE            0

t_config * config_file;

enum mensajes {
	IDENTIFY = 10
};

enum { IDENTIFY_ESI = 1, IDENTIFY_INSTANCIA = 2, IDENTIFY_PLANIFICADOR = 3};

void create_server(int max_connections, int timeout);
void configure_logger();
int create_client(char * ip, char * port);
void wait_hello(int socket);
void send_message(int socket);
int connect_to_server(char * ip, char * port);

#endif /* SOCKETSERVER_H_ */
