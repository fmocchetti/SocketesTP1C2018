/*
 * distribucion.c
 *
 *  Created on: May 28, 2018
 *      Author: fmocchetti
 */


int distribuir(char * clave, char * valor, unsigned char operacion) {
	t_instancia * instancia;

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

	sem_post(&(instancia->instance_sem));
    pthread_mutex_unlock(&mutex);

	return 0;
}

int modificar_valor_clave(char * clave, char * valor, int instancia) {

	return 0;
}

int store_clave(char * clave, int instancia) {

	return 0;
}
