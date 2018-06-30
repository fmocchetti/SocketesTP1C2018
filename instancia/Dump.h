/*
 * Dump.h
 *
 *  Created on: 3 jun. 2018
 *      Author: PabloSMonti
 */

#ifndef DUMP_H_
#define DUMP_H_
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
	char* puntoDeMontaje;
	unsigned int intervaloDeDump;
}parametros_dump;

int respaldar_informacion(t_list* tabla,char* puntoDeMontaje,t_log* logger);

//int respaldar_informacion_thread(parametros_dump* paramtros);

#endif /* DUMP_H_ */
