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
	sem_post(&mutex_planificador);
	log_info(logger, "Desbloqueo planificador");
	return 0;
}

t_instancia * simular(char * clave) {
	t_instancia * instancia = NULL;
	char letra_inicial = NULL;
	int indice_busqueda = 0;
	int simular = ultima_instancia;

	if(!total_instancias && !instancias_activas) {
		log_error(logger, "No hay instancias disponibles");
		return NULL;
	}

	log_info(logger, "Voy a buscar a que instancia se la debo asignar");

	switch(algoritmo_elegido) {
		case EL:
			log_info(logger, "Selecciono por Equitative Load");
			log_info(logger, "Ultima instancia: %d, total instancias: %d", ultima_instancia, total_instancias);
			while(!instancia) {
				simular  %= total_instancias;
				instancia = list_get(list_instances, simular);
				simular++;
				if(!instancia->status)
					instancia = NULL;
			}
			break;
		case KE:
			log_info(logger, "Selecciono por Clave");
			letra_inicial = clave[0] - 96;
			if(letra_inicial <= 0) letra_inicial = 1;
			indice_busqueda = letra_inicial / letras_instancia;
			log_info(logger, "Voy a buscar la instancia: %d", indice_busqueda);
			while(!instancia) {
				instancia = list_get(list_instances, indice_busqueda);
				if(!instancia->status) {
					indice_busqueda++;
					instancia = NULL;
				}
			}
			break;
		case LSU:
			log_info(logger, "Selecciono por Espacio Disponible");
			instanciaLSU = buscarMasLibre(total_instancias);
			instancia = list_get(list_instances, instanciaLSU);
			break;
	}

	if(!instancia) return NULL;
	return instancia;
}

t_instancia * distribuir(char * clave) {
	t_instancia * instancia = NULL;
	char letra_inicial = 0;
	int indice_busqueda = 0;

	log_info(logger, "Voy a buscar a que instancia se la debo asignar");


	if(!total_instancias || !instancias_activas) {
		log_error(logger, "No hay instancias disponibles, finalizando coordinador");
		exit_gracefully(-1);
	}

	switch(algoritmo_elegido) {
		case EL:
			log_info(logger, "Selecciono por Equitative Load");
			log_info(logger, "Ultima instancia: %d, total instancias: %d, instancias_activas: %d", ultima_instancia, total_instancias, instancias_activas);
			while(!instancia) {
				ultima_instancia  %= total_instancias;
				instancia = list_get(list_instances, ultima_instancia);
				ultima_instancia++;
				if(!instancia->status)
					instancia = NULL;
			}
			break;
		case KE:
			log_info(logger, "Selecciono por Clave");
			letra_inicial = clave[0] - 96;
			if(letra_inicial <= 0) letra_inicial = 1;
			indice_busqueda = letra_inicial / letras_instancia;
			log_info(logger, "Voy a buscar la instancia: %d", indice_busqueda);
			while(!instancia) {
				instancia = list_get(list_instances, indice_busqueda);
				if(!instancia->status) {
					indice_busqueda++;
					instancia = NULL;
				}
			}
			break;
		case LSU:
			log_info(logger, "Selecciono por Espacio Disponible");
			while(!instancia) {
				instanciaLSU = buscarMasLibre(total_instancias);
				instancia = list_get(list_instances, instanciaLSU);
				if(!instancia->status) {
					instancia = NULL;
				}
			}
			break;
	}

	if(!instancia) exit_gracefully(-1);
	return instancia;
}

int modificar_valor_clave(char * clave, char * valor, int instancia) {
	t_instancia * o_instancia;

	log_info(logger, "Modificar valor %d", instancia);
	o_instancia = list_get(list_instances, instancia);
	if(!o_instancia->status) {
		sem_post(&mutex_instancia);
		return NULL;
	}
	o_instancia->clave = clave;
	o_instancia->valor = valor;
	o_instancia->operacion = 22;
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	sem_wait(&mutex_instancia);
	if(!o_instancia->status) {
		o_instancia = NULL;
		log_error(logger, "Esa instancia fallo");
	} else {
		log_error(logger, "Dato enviado a la instancia");
	}
	return o_instancia;
}

int store_clave(char * clave, int instancia) {
	t_instancia * o_instancia;
	log_info(logger, "Store valor %d", instancia);
	o_instancia = list_get(list_instances, instancia);
	if(!o_instancia->status) {
		sem_post(&mutex_instancia);
		return NULL;
	}
	log_debug(logger, "Clave a mandar a la instancia %s", clave);
	o_instancia->clave = clave;
	o_instancia->operacion = 23;
	sem_post(&(o_instancia->instance_sem));
	log_info(logger, "Desbloquie instancia %d", instancia);
	return o_instancia;
}

int buscarMasLibre(int totalInstancias){
	int posicionMasLibre;
	int masEntradasLibres = 0;
	t_instancia *instanciaA;
	totalInstancias--;
	for(; totalInstancias >= 0; totalInstancias--){
		instanciaA = list_get(list_instances, totalInstancias);
		log_info(logger, "Consultando la instancia: %d, con %d entradas libres y esta en estado: %d", instanciaA->id, instanciaA->entradasLibres, instanciaA->status);
		if(instanciaA->status && masEntradasLibres < instanciaA->entradasLibres){
			posicionMasLibre = totalInstancias;
			masEntradasLibres = instanciaA->entradasLibres;
		}
	}
	return posicionMasLibre;
}
