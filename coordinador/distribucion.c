/*
 * distribucion.c
 *
 *  Created on: May 28, 2018
 *      Author: fmocchetti
 */

#include "coordinador.h"


int distribuir(char * clave, char * valor, unsigned char operacion) {
	t_instancia * instancia;

	return rand() % 5;

	switch(algoritmo_elegido) {
		case EL:
			ultima_instancia  %= total_instancias;
			instancia = list_get(list_instances, ultima_instancia);
			break;
		case KE:
			break;
		case LSU:
			break;
	}

	instancia->clave = clave;
	instancia->valor = valor;
	instancia->operacion = operacion;
    pthread_mutex_unlock(&mutex);
	sem_post(&(instancia->instance_sem));

	return 0;
}

int modificar_valor_clave(char * clave, char * valor, int instancia) {

	return rand() % 5;
	//return 0;
}

int store_clave(char * clave, int instancia) {

	return rand() % 5;
	//return 0;
}
