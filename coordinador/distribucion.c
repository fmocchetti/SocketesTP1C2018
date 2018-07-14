/*
 * distribucion.c
 *
 *  Created on: May 28, 2018
 *      Author: fmocchetti
 */

#include "coordinador.h"

int informar_planificador(char * clave, unsigned char status) {
	log_info(logger, "Esperando instancia");
	sem_wait(&mutex_instancia);
	log_info(logger, "Pase bloqueo de instancia");
	thread_planificador->clave = clave;
	thread_planificador->status = status;
	sem_post(&mutex_planificador);
	log_info(logger, "Desbloqueo planificador");
	return 0;
}



int distribuir(char * clave, char * valor) {
	t_instancia * instancia;
	char letra_inicial = NULL;

	if(!total_instancias) exit_gracefully(-1);

	switch(algoritmo_elegido) {
		case EL:
			log_info(logger, "Ultima instancia: %d", ultima_instancia);
			log_info(logger, "Total instancias: %d", total_instancias);
			ultima_instancia  %= total_instancias;
			instancia = list_get(list_instances, ultima_instancia);
			ultima_instancia++;
			break;
		case KE:
			letra_inicial = clave[0] - 96;
			if(letra_inicial <= 0) letra_inicial = 1;
			instancia = list_get(list_instances, letra_inicial / letras_instancia);
			break;
		case LSU:
			instanciaLSU = buscarMasLibre(total_instancias);
			instancia = list_get(list_instances, instanciaLSU);
			instancia->entradasLibres -= 1;
			break;
	}

	if(!instancia) exit_gracefully(-1);

	instancia->clave = clave;
	instancia->valor = valor;
	instancia->operacion = 21;

	sem_post(&(instancia->instance_sem));
	return instancia->id - 1;
}

int modificar_valor_clave(char * clave, char * valor, int instancia) {
	t_instancia * o_instancia;

	log_info(logger, "Modificar valor %d", instancia);
	log_info(logger, "Pase Mutex");
	o_instancia = list_get(list_instances, instancia);
	o_instancia->clave = clave;
	o_instancia->valor = valor;
	o_instancia->operacion = 22;
    log_info(logger, "Desbloquie Mutex");
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	return instancia;
}

int store_clave(char * clave, int instancia) {
	t_instancia * o_instancia;
	log_info(logger, "Store valor %d", instancia);
	log_info(logger, "Pase Mutex");
	o_instancia = list_get(list_instances, instancia);
	log_debug(logger, "Clave a mandar a la instancia %s", clave);
	o_instancia->clave = clave;
	o_instancia->operacion = 23;
	log_info(logger, "Desbloquie Mutex");
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	return instancia;
}

int buscarMasLibre(int totalInstancias){
	int posicionMasLibre;
	int masEntradasLibres = 0;
	t_instancia *instanciaA;
	totalInstancias--;
	for(; totalInstancias >= 0; totalInstancias--){
		instanciaA = list_get(list_instances, totalInstancias);
		if(masEntradasLibres < instanciaA->entradasLibres){
			posicionMasLibre = totalInstancias;
			masEntradasLibres = instanciaA->entradasLibres;
		}
	}
	return posicionMasLibre;
}
