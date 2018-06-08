/*
 * algoritmos.h
 *
 *  Created on: 12 may. 2018
 *      Author: pdelucchi
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>
#include "protocolo.h"

t_list * listos;
t_list * bloqueados;
t_list * ejecucion;
t_list * terminados;
t_list * claves_tomadas;
sem_t new_process;
//sem_t replanificar;
sem_t give_me_a_new_process;
sem_t mutex_listos;
sem_t mutex_ejecucion;
sem_t sem_pausar_planificacion;
sem_t sem_pausar_algoritmo;
//pthread_mutex_t mutex_ejecucionn = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_ejecucion;
t_dictionary * claves_bloqueadas;
int replanificar;
int socket_coord;

void laWeaReplanificadoraFIFO(t_list *, t_list *);
void estadoListas();
void fifo();
bool identificador_ESI(void*);

bool sort_by_estimacion(void*, void*);
void element_destroyer(void *);
float calculoProxRafaga(float ,float, float);
void sjfsd();
void sjfcd();
void ESI_GET(char *, int, unsigned char);
void ESI_STORE(char *);
void clave_dictionary_destroy(t_dictionary *data);
void clave_destroy(claves *);
void coord_communication(int,unsigned char,unsigned char);
bool identificador_clave(void *);

#endif /* ALGORITMOS_H_ */