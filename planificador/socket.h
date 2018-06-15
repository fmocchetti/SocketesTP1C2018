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
#include <netdb.h>
#include <errno.h>
#include <strings.h>
#include "protocolo.h"
#include "algoritmos.h"
#include <semaphore.h>
#include <pthread.h>


#define TRUE             1
#define FALSE            0


enum mensajes {
	IDENTIFY = 10,
	INICIALIZAR_INSTANCIA = 11,
	CONEXION_ESI = 18,
	COORDINADOR_GET = 24,
	COORDINADOR_SET = 	25,
	COORDINADOR_STORE = 26
};

enum { IDENTIFY_ESI = 1, IDENTIFY_INSTANCIA = 2, PLANIFICADOR = 3};

typedef struct {
	int socket;
	int identidad;
} thread_handle_struct;

enum {POLL, THREAD_CONNECTION, SELECT };

void configure_logger();
void create_server(int max_connections, int timeout, int server_type, int port);
void listen_on_poll(struct pollfd * fds, int max_connections, int timeout, int listen_sd);
void connection_thread();
void thread_on_connection(int listen_sd);
void exit_gracefully(int return_nr);

#endif /* SOCKET_H_ */
