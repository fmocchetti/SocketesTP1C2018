#include "socket.h"

void inicializar_instancia (int socket) {
	unsigned char identificador = INICIALIZAR_INSTANCIA;
	int messageLength = sizeof(int) * 3;
	void * buffer = malloc (messageLength + 6);
	memcpy(buffer, &identificador, 1);
	memcpy(buffer+1, &messageLength, 4);
	memcpy(buffer+5, &cantidad_entradas, 4);
	memcpy(buffer+9, &size_key, 4);
	memcpy(buffer+13, &retardo, 4);
	send(socket, buffer, 5 + messageLength, 0);
	log_info(logger, "Inicializacion enviada correctamente");
}

bool find_instancia(void * element) {
	t_instancia * elemento = (t_instancia *) element;
	if(elemento->id == instancia_to_find) {
		return true;
	}

	return false;
}

void restaura_clave(int identificador_instancia, int socket_local) {
	char * message = NULL;
	int size_clave = 0, claves =0, size_package = 0;
	unsigned char identificador = 57;

	void _evaluar_restaurar(char * clave, t_clave * elemento) {
		if(elemento->instancia == identificador_instancia) {
			size_clave = strlen(clave);
			message = (char *)realloc(message,size_package+strlen(clave)+4);
			memcpy(message+size_package, &size_clave, 4);
			memcpy(message+size_package+4, clave, strlen(clave));
			size_package += strlen(clave)+4;
			claves++;
		}
	}

	dictionary_iterator(diccionario_claves, (void *)_evaluar_restaurar);

	if(!claves) identificador = 58;
	send(socket_local, &identificador, 1, 0);
	if(identificador == 57) {
		send(socket_local, &claves, 4, 0);
		send(socket_local, message, size_package, 0);
	}

	return;
}



void _instancia(int socket_local) {
	int rc = 0, close_conn = 0, identificador_instancia = 0;
	int size_clave = 0, size_valor = 0, messageLength = 0;
	bool restore = false;
	unsigned char buffer = 0;
	char * mensajes = NULL;
	t_instancia * local_struct;

	rc = recv(socket_local, &identificador_instancia, sizeof(identificador_instancia), 0);
	if (rc == 0) {
		log_error(logger, "  recv() failed");
		close_conn = TRUE;
	}

	log_info(logger, "Esta instancia es: %d", identificador_instancia);

	if(total_instancias != 0) {
		instancia_to_find = identificador_instancia;
		local_struct = list_find(list_instances, (void *)find_instancia);
		if(!local_struct) {
			local_struct = instancia_create(identificador_instancia, cantidad_entradas);
			list_add(list_instances, local_struct);
			total_instancias++;
		} else {
			local_struct->status = true;
			restore = true;
		}
	} else {
		local_struct = instancia_create(identificador_instancia, cantidad_entradas);
		list_add(list_instances, local_struct);
		total_instancias++;
	}

    instancias_activas++;

	inicializar_instancia(socket_local);

	if(restore)
		restaura_clave(identificador_instancia-1, socket_local);

	sem_init(&(local_struct->instance_sem), 1, 0);

	log_info(logger, "Cantidad de instancias -> %d", list_size(list_instances));

	if(algoritmo_elegido == KE) {
		letras_instancia = (25 / instancias_activas) + (25 % instancias_activas);
		log_info(logger, "Cantidad de letras por instancia -> %d", letras_instancia);
	}


	while(1) {

		sem_wait(&(local_struct->instance_sem));

		log_info(logger, "Pase el semaforo de la instancia %d", local_struct->id);
		//pthread_mutex_lock(&mutex);
		log_info(logger, "Pase el Mutex de la instancia");

		size_clave = strlen(local_struct->clave);
		messageLength = 5 + size_clave;
		if(local_struct->operacion == ESI_SET) {
			size_valor = strlen(local_struct->valor);
			messageLength += 4 + size_valor;
		}

		mensajes = (char *) malloc (messageLength);
		memcpy(mensajes, &(local_struct->operacion), 1);
		memcpy(mensajes+1, &size_clave, 4);

		log_info(logger,"Clave a mandar %s", local_struct->clave);
		memcpy(mensajes+5, local_struct->clave, size_clave);
		if(local_struct->operacion == ESI_SET) {
			memcpy(mensajes+size_clave+5, &size_valor, 4);
			log_info(logger,"Valor a mandar %s", local_struct->valor);
			memcpy(mensajes+size_clave+9, local_struct->valor, size_valor);
		}

		log_info(logger, "Enviandole a la instancia %d bytes", messageLength);
		rc = send(socket_local, mensajes, messageLength, 0);

		log_info(logger, "El send me dice %d", rc);
        if (rc <= 0) {
        	log_error(logger, "  recv() failed");
        	close_conn = TRUE;
        	break;
        }

		free(mensajes);
        rc = recv(socket_local, &buffer, 1, 0);
        if (rc <= 0) {
        	log_error(logger, "  recv() failed");
        	close_conn = TRUE;
        	break;
        }

        log_info(logger, "La instancia termino de procesar");
        sem_post(&mutex_instancia);

        //pthread_mutex_unlock(&mutex);
	}


	if (close_conn) {
		//shutdown(socket_local, SHUT_RDWR);
		log_error(logger, "La instancia %d esta caida", identificador_instancia);
		instancias_activas--;
		local_struct->status = false;
		if(algoritmo_elegido == KE) {
			letras_instancia = (25 / instancias_activas) + (25 % instancias_activas);
			log_info(logger, "Cantidad de letras por instancia -> %d", letras_instancia);
		}
		sem_post(&mutex_instancia);
	}
}
