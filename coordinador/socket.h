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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <strings.h>
#include <pthread.h>
#include <commons/log.h>
#include <unistd.h>
#include "coordinador.h"

#define TRUE             1
#define FALSE            0


char * returned_value;
// Quien decidio este protocolo? Un hechicero lo hizo

enum mensajes {
	ESI_OK = 7,
	ESI_ERROR = 8,
	ESI_BLOCK = 9,
	IDENTIFY = 10,
	INICIALIZAR_INSTANCIA = 11,
	CONEXION_ESI = 18,
	ESI_GET = 21,
	ESI_SET = 	22,
	ESI_STORE = 23,
	COORDINADOR_GET = 24,
	COORDINADOR_SET = 	25,
	COORDINADOR_STORE = 26,
	COORDINADOR_ERROR = 27,
	PLANIFICADOR_CLAVE = 32,
	PLANIFICADOR_BLOQUEAR = 33,
	PLANIFICADOR_DESBLOQUEAR = 34,
	PLANIFICADOR_STATUS = 98,
	PLANIFICADOR_SIMULACION = 99,
	PLANIFICADOR_CLAVE_EXISTENTE = 100,
	PLANIFICADOR_NO_INSTANCIAS = 101,
	INSTANCIA_COMPACTAR = 200,
	INSTANCIA_STATUS = 201,
	INSTANCIA_ERROR = 202,
	INSTANCIA_VALOR = 203
};

enum { ESI = 1, INSTANCIA = 2, PLANIFICADOR = 3};

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
void _exit_with_error(int socket, char* error_msg, char * buffer);

#endif /* SOCKET_H_ */
