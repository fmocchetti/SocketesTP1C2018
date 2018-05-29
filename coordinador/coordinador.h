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
#include "distribucion.h"

void _instancia(int socket_local);
void _planificador(int socket_local);
void _esi(int socket_local);
void inicializar_instancia (int socket);

t_list* list_instances;

int total_instancias;

typedef struct {
    int id;
} t_instancia;

static t_instancia *instancia_create(int id) {
	t_instancia *new = malloc(sizeof(t_instancia));
    new->id = id;
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
	t_clave *new = malloc( sizeof(t_clave) );
	new->esi = instancia;
	new->instancia = instancia;
	new->tomada = tomada;
	return new;
}

static void clave_destroy(t_clave *self){
	free(self);
}


#endif /* COORDINADOR_H_ */
