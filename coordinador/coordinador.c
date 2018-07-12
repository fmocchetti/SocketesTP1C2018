#include "socket.h"

#define IDENTIDAD "coordinador"

int listening_port = 0;
int cantidad_entradas = 20;
int size_key = 200;
int retardo = 300;

int main (int argc, char *argv[])
{
	configure_logger();

	printf("%s\n",IDENTIDAD);

	config_file = config_create("coordinador.conf");

	pthread_mutex_init(&mutex, NULL);
	sem_init(&mutex_planificador, 1, 0);
	sem_init(&mutex_instancia, 1, 0);
	total_instancias = 0;
	list_instances = list_create();
	diccionario_claves = dictionary_create();
	cantidad_entradas = config_get_int_value(config_file, "cantidad_entradas");
	size_key = config_get_int_value(config_file, "size_entrada");
	retardo = config_get_int_value(config_file, "retardo");
	algoritmo_elegido =  config_get_int_value(config_file, "algoritmo_distribucion");

	create_server(32, 20 * 60 * 1000, THREAD_CONNECTION, config_get_int_value(config_file, "listening_port"));

	exit_gracefully(0);
}

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

void _instancia(int socket_local) {
	int rc = 0, close_conn = 0;
	int size_clave = 0, size_valor = 0, messageLength = 0;
	unsigned char buffer = 0;
	char * mensajes = NULL;
	t_instancia * local_struct = instancia_create(total_instancias);

	inicializar_instancia(socket_local);

	sem_init(&(local_struct->instance_sem), 1, 0);

	list_add(list_instances, local_struct);

	log_info(logger, "Cantidad de instancias -> %d", list_size(list_instances));

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
		send(socket_local, mensajes, messageLength, 0);
		free(mensajes);

        rc = recv(socket_local, &buffer, 1, 0);
        if (rc == 0) {
		 log_error(logger, "  recv() failed");
		 close_conn = TRUE;
		 break;
        }
        log_info(logger, "La instancia termino de procesar");
        sem_post(&mutex_instancia);

        //pthread_mutex_unlock(&mutex);
	}


	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
	}
}

void _esi(int socket_local) {
	int rc = 0, close_conn = 0, id_esi = 0, instancia_destino = -1, clavesDisponibles = 0;
	int message_length = 0;
	unsigned char identificador = 0;
	char * clave = NULL;
	char * valor = NULL;
	t_clave * clave_diccionario = NULL;
	char * claves_tomadas[10];

	recv(socket_local, &id_esi, 4, 0);
	log_info(logger, "Esta esi es la %d", id_esi);
	while(1) {
        rc = recv(socket_local, &identificador, 1, 0);
        if (rc == 0) {
        	log_error(logger, "  recv() failed");
        	close_conn = TRUE;
        	break;
        }

        log_info(logger, "La ESI %d quiere realizar un: %d", id_esi, identificador);
        switch(identificador) {
        	case ESI_GET:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, clave, message_length, 0);
        		clave[message_length] = '\0';
        		log_info(logger, "La ESI %d me esta pidiendo la clave '%s'", id_esi, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
        			log_info(logger, "Existe la clave '%s' en el diccionarop", clave);
        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
        			if(clave_diccionario->tomada) {
        				log_info(logger, "La clave '%s' esta tomada", clave);
        				identificador = ESI_BLOCK;
    					send(socket_local, &identificador, 1, 0);
    					sem_post(&mutex_instancia);
        			} else {
        				log_info(logger, "La clave '%s' no esta tomada", clave);
            			identificador = ESI_OK;
            			send(socket_local, &identificador, 1, 0);
            			instancia_destino = distribuir(clave, NULL);
            			clave_diccionario->tomada = true;
            			clave_diccionario->esi = id_esi;
            			clave_diccionario->instancia = instancia_destino;
            			claves_tomadas[clavesDisponibles++] = clave;
        			}
        		} else {
        			log_info(logger, "No existe la clave '%s' en el diccionarop", clave);
        			instancia_destino = distribuir(clave, NULL);
        			dictionary_put(diccionario_claves, clave, clave_create(id_esi, instancia_destino, true));
        			identificador = ESI_OK;
        			send(socket_local, &identificador, 1, 0);
        			claves_tomadas[clavesDisponibles++] = clave;
        		}
        		informar_planificador(clave, COORDINADOR_GET);
        		log_info(log_operaciones, "ESI %d - GET %s", id_esi, clave);
        		break;
        	case ESI_SET:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, clave, message_length, 0);
        		clave[message_length] = '\0';
        		rc = recv(socket_local, &message_length, 4, 0);
        		valor = malloc(message_length + 1);
				rc = recv(socket_local, valor, message_length, 0);
				valor[message_length] = '\0';
        		log_info(logger, "La ESI %d quiere guardar el valor '%s' en la clave '%s'", id_esi, valor, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
        			if(clave_diccionario->tomada && clave_diccionario->esi != id_esi) {
        				log_info(logger, "Esta clave esta tomada por la esi %d", clave_diccionario->esi);
        				identificador = ESI_ERROR;
    					send(socket_local, &identificador, 1, 0);
        				//informar esi error
        			} else if (!(clave_diccionario->tomada)) {
        				log_info(logger, "Esta clave no estaba tomada");
        				identificador = ESI_ERROR;
    					send(socket_local, &identificador, 1, 0);
        				//informar esi error
        			}else {
        				modificar_valor_clave(clave, valor, clave_diccionario->instancia);
        				identificador = ESI_OK;
						send(socket_local, &identificador, 1, 0);
        				//informar esi todo ok
        			}
        		} else {
        			//informar esi error
    				identificador = ESI_ERROR;
					send(socket_local, &identificador, 1, 0);
        			log_error(logger, "Intentando hacer un SET a una clave inexistente");
        		}
        		informar_planificador(clave, COORDINADOR_SET);
        		log_info(log_operaciones, "ESI %d - SET %s %s", id_esi, clave, valor);
        	    break;
        	case ESI_STORE:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, clave, message_length, 0);
        		clave[message_length] = '\0';
        		log_info(logger, "La ESI %d quiere hacer STORE de la clave '%s'", id_esi, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
					clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
					if(clave_diccionario->tomada && clave_diccionario->esi != id_esi) {
						//informar esi error
	    				identificador = ESI_ERROR;
						send(socket_local, &identificador, 1, 0);
					} else if (!(clave_diccionario->tomada)) {
						//informar esi error
	    				identificador = ESI_ERROR;
						send(socket_local, &identificador, 1, 0);

					}else {
						store_clave(clave, clave_diccionario->instancia);
						//informar esi todo ok
						//informar planificador que clave se libero (ESI_STORE, clave)
						identificador = ESI_OK;
						send(socket_local, &identificador, 1, 0);
						clave_diccionario->tomada = false;
						clave_diccionario->esi = -1;
					}
				} else {
					//informar esi error
    				identificador = ESI_ERROR;
					send(socket_local, &identificador, 1, 0);
					log_error(logger, "Intentando hacer un STORE a una clave inexistente");
				}
        		informar_planificador(clave, COORDINADOR_STORE);
        		log_info(log_operaciones, "ESI %d - STORE %s", id_esi, clave);
        	    break;
        	default:
        		break;
        }
	}


	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
		liberar_claves(claves_tomadas, clavesDisponibles);

	}

}

void liberar_claves(char ** claves_tomadas, int cantidad_claves) {
	int i = 0;
	t_clave * clave_diccionario;

	for(i = 0; i < cantidad_claves; i++) {
		clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, claves_tomadas[i]);
		clave_diccionario->tomada = false;
		log_info(logger, "Liberando la clave: %s", claves_tomadas[i]);
	}

}

void _planificador(int socket_local) {
	int rc = 0, close_conn = 0, instancia_destino = -1;
	int size_clave = 0, cantidad_claves = 0;
	unsigned char identificador = 0;
	char * clave = NULL;
	t_clave * clave_diccionario = NULL;

	thread_planificador = planificador_create();

	while(1) {
        rc = recv(socket_local, &identificador, 1, 0);
        if (rc == 0) {
        	log_error(logger, "  recv() failed");
        	close_conn = TRUE;
        	break;
        }

        log_info(logger, "El Planificador me esta diciendo: %d" , identificador);

        switch(identificador) {
			case PLANIFICADOR_CLAVE:
				sem_wait(&mutex_planificador);
				log_info(logger, "Contestando pedido de clave");
				int messageLength = 1;
				int size_clave = strlen(thread_planificador->clave);
				if(thread_planificador->status != COORDINADOR_SET) {
					messageLength += 5 + size_clave;
				}

				log_info(logger, "Le voy a mandar al planificador un %d", thread_planificador->status);
				char * mensajes = (char *) malloc (messageLength);
				memcpy(mensajes, &(thread_planificador->status), 1);
				if(thread_planificador->status != COORDINADOR_SET) {
					memcpy(mensajes+1, &size_clave, 4);
					memcpy(mensajes+5, thread_planificador->clave, size_clave);
					messageLength -= 1;
				}

				log_info(logger, "Enviandole al planificador %d bytes", messageLength);
				send(socket_local, mensajes, messageLength, 0);
				free(mensajes);

				break;
			case PLANIFICADOR_BLOQUEAR:
				recv(socket_local, &cantidad_claves, 4, 0);
				for(int i=0; i < cantidad_claves; i++) {
					recv(socket_local, &size_clave, 4, 0);
					clave = (char *)malloc (size_clave + 1);
					recv(socket_local, clave, size_clave, 0);
					clave[size_clave] = '\0';

					log_info(logger, "Voy a bloquear la clave '%s'", clave);
					printf("%s\n",clave);
	        		if(dictionary_has_key(diccionario_claves, clave)) {
	        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
	        			if(!clave_diccionario->tomada) {
	            			clave_diccionario->tomada = true;
	            			instancia_destino = distribuir(clave, NULL);
	            			clave_diccionario->instancia = instancia_destino;
	        			}
	        		} else {
	        			instancia_destino = distribuir(clave, NULL);
	        			dictionary_put(diccionario_claves, clave, clave_create(0, instancia_destino, true));
	        		}
	        		//free(clave);
				}
				break;
			case PLANIFICADOR_DESBLOQUEAR:
				recv(socket_local, &size_clave, 4, 0);
				clave = (char *)malloc (size_clave + 1);
				recv(socket_local, clave, size_clave, 0);
				clave[size_clave] = '\0';

				log_info(logger, "Voy a desbloquear la clave '%s'", clave);
				if(dictionary_has_key(diccionario_claves, clave)) {
					clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
					store_clave(clave, clave_diccionario->instancia);
					clave_diccionario->tomada = false;
					log_error(logger, "clave desbloqueada");
				} else {
					log_error(logger, "Intentando desbloquear una clave inexistente");
				}
				//free(clave);
				break;
        }
	}

	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
	}

}
