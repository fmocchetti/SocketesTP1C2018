/*
 * coordinador.h
 *
 *  Created on: May 27, 2018
 *      Author: fmocchetti
 */

#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/config.h>
#include "distribucion.h"

void _instancia(int socket_local);
void _planificador(int socket_local);
void _esi(int socket_local);
void inicializar_instancia (int socket);

t_list* list_instances;
t_config * config_file;
t_log * logger;
t_log * log_operaciones;

int total_instancias;
pthread_mutex_t mutex;
sem_t mutex_planificador;
sem_t mutex_instancia;

typedef struct {
    int id;
    sem_t instance_sem;
    char * clave;
    char * valor;
    unsigned char operacion;
} t_instancia;

typedef struct {
    unsigned char status;
    char * clave;
} t_planificador;

t_planificador * thread_planificador;

static t_planificador *planificador_create(void) {
	t_planificador *new = malloc(sizeof(t_planificador));
    new->status = 0;
    new->clave = 0;
    return new;
}

static void planificador_destroy(t_planificador *self) {
	free(self->clave);
    free(self);
}


static t_instancia *instancia_create(int id) {
	t_instancia *new = malloc(sizeof(t_instancia));
    new->id = id;
    new->clave = 0;
    new->valor = 0;
    new->operacion = 0;
    total_instancias++;
    return new;
}

static void instancia_destroy(t_instancia *self) {
    free(self);
}

typedef struct {
	int esi;
	int instancia;
	bool tomada;
} t_clave;

t_dictionary * diccionario_claves;

static t_clave *clave_create(int esi, int instancia, bool tomada){
	t_clave * new = malloc (sizeof(t_clave));
	new->esi = esi;
	new->instancia = instancia;
	new->tomada = tomada;
	return new;
}

static void clave_destroy(t_clave *self){
	free(self);
}


#endif /* COORDINADOR_H_ */
