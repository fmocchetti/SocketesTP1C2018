/*
 * algoritmos.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <pthread.h>

t_list * listos;
t_list * bloqueados;
t_list * ejecucion;
t_list * terminados;
sem_t new_process;
sem_t give_me_a_new_process;
sem_t mutex_listos;
sem_t mutex_ejecucion;
//pthread_mutex_t mutex_ejecucionn = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_ejecucion;
typedef struct {
    int ID;
}t_proceso;

void laWeaReplanificadoraFIFO(t_list *, t_list *);
void estadoListas();
void fifo();

#endif /* ALGORITMOS_H_ */
