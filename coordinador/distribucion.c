/*
 * distribucion.c
 *
 *  Created on: May 28, 2018
 *      Author: fmocchetti
 */

#include "coordinador.h"

int informar_planificador(char * clave, unsigned char status) {
	thread_planificador->clave = clave;
	thread_planificador->status = status;
	sem_wait(&mutex_instancia);
	sem_post(&mutex_planificador);
	return 0;
}



int distribuir(char * clave, char * valor) {
	t_instancia * instancia;

	switch(algoritmo_elegido) {
		case EL:
			ultima_instancia  %= total_instancias;
			instancia = list_get(list_instances, ultima_instancia);
			ultima_instancia++;
			break;
		case KE:
			break;
		case LSU:
			break;
	}

	instancia->clave = clave;
	instancia->valor = valor;
	instancia->operacion = 21;

	sem_post(&mutex_instancia);
	return instancia->id;
}

int modificar_valor_clave(char * clave, char * valor, int instancia) {
	t_instancia * o_instancia;

	log_info(logger, "Modificar valor %d", instancia);
	//pthread_mutex_lock(&mutex);
	log_info(logger, "Pase Mutex");
	o_instancia = list_get(list_instances, instancia);
	o_instancia->clave = clave;
	o_instancia->valor = valor;
	o_instancia->operacion = 22;
    //pthread_mutex_unlock(&mutex);
    log_info(logger, "Desbloquie Mutex");
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	return instancia;
}

int store_clave(char * clave, int instancia) {
	t_instancia * o_instancia;
	log_info(logger, "Store valor %d", instancia);
	//pthread_mutex_lock(&mutex);
	log_info(logger, "Pase Mutex");
	o_instancia = list_get(list_instances, instancia);
	o_instancia->clave = clave;
	o_instancia->operacion = 23;
    //pthread_mutex_unlock(&mutex);
	log_info(logger, "Desbloquie Mutex");
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	return instancia;
}
