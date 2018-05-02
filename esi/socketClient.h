/*
 * socketClient.h
 *
 *  Created on: 24 abr. 2018
 *      Author: utnso
 */

#ifndef SOCKETCLIENT_H_
#define SOCKETCLIENT_H_

#include <stdio.h>
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

// Definimos algunas variables globales
t_log * logger;

int create_client(char*, char *);
void configure_logger();
int connect_to_server(char *, char *);
void wait_hello(int);
void send_message(int);
void _exit_with_error(int, char*, void*);
void exit_gracefully(int);


#endif /* SOCKETCLIENT_H_ */
