/*
 * instancia.h
 *
 *  Created on: 8 jun. 2018
 *      Author: Pablo Monti
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_


#include "tabla.h"
#include "socket_client.h"

#include <pthread.h>
#include "ClaveValor.h"
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <commons/log.h>
#include "tabla.h"
#include <pthread.h>

typedef struct{
	t_list* tabla;
	char* storage;
	char* puntoDeMontaje;
	t_log* logger;
	int intervaloDeDump;
}parametros_dump;

pthread_mutex_t lock_dump;

#endif /* INSTANCIA_H_ */
