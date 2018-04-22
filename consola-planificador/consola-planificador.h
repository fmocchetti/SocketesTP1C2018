/*
 * consola-planificador.h
 *
 *  Created on: 21 abr. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_PLANIFICADOR_H_
#define CONSOLA_PLANIFICADOR_H_

// Aca incluimos las bibliotecas que vamos a usar
#include <stdio.h>
#include <openssl/md5.h> // Para calcular el MD5
#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <netdb.h> // Para getaddrinfo
#include <unistd.h> // Para close
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/collections/list.h>

// Definimos algunas constantes para nuestro código
#define IP "127.0.0.1"
#define PUERTO "12345"

// Definimos algunas variables globales
t_log * logger;



#endif /* CONSOLA_PLANIFICADOR_H_ */
