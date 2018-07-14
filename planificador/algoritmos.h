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
#include <commons/config.h>
#include <commons/log.h>



t_log * logger;
t_config * config_file;
t_list * listos;
t_list * bloqueados;
t_list * ejecucion;
t_list * terminados;
t_list * muertos;
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
int esi_bloqueada_de_entrada;
char clave_bloqueada_global[40];
int id_esi_global;
int id_esi_global_kill;
int alpha;
int estimacion_inicial;
int algoritmo_elegido;
enum {SJFSD , SJFCD, HRRN};

void laWeaReplanificadoraFIFO(t_list *, t_list *);
void estadoListas();
void fifo();
bool identificador_ESI(void*);
bool identificador_ESI_kill(void*);

bool sort_by_estimacion(void*, void*);
bool sort_by_aging(void *, void *);
void element_destroyer(void *);
float calculoProxRafaga(float ,float, float);
void sjfsd();
void sjfcd();
void hrrn();
void ESI_GET(char *, int, unsigned char);
void ESI_STORE(char *,int);
void clave_dictionary_destroy(t_dictionary *data);
void clave_destroy(claves *);
void coord_communication(int,int,unsigned char,int);
bool identificador_clave(void *);
bool identificador_clave_por_idESI(void * data);
void nodo_lista_claves_destroyer(claves * data);
void get_keys_bloqueadas_de_entrada(int);
void desbloquear_del_diccionario(char *,int);
void aplicarHRRN(ESI*);
void obtenerPrioridad();
void envejecerLista(int);



#endif /* ALGORITMOS_H_ */
