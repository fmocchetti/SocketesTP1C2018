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

	total_instancias = 0;
	list_instances = list_create();
	diccionario_claves = dictionary_create();

	create_server(32, 20 * 60 * 1000, THREAD_CONNECTION, 12346);

	exit_gracefully(0);
}

void inicializar_instancia (int socket) {
	char identificador = INICIALIZAR_INSTANCIA;
	size_t messageLength = sizeof(int) * 3;
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
	unsigned char buffer = 0;
	t_instancia * local_struct = instancia_create(total_instancias);

	inicializar_instancia(socket_local);

	sem_init(&(local_struct->instance_sem), 1, 0);

	list_add(list_instances, local_struct);

	log_info(logger, "Cantidad de instancias -> %d", list_size(list_instances));

	local_struct->clave = malloc(strlen("clave"));
	local_struct->clave = "clave";
	local_struct->valor = malloc(strlen("valor"));
	local_struct->valor = "valor";
	local_struct->operacion = ESI_SET;

	sem_post(&(local_struct->instance_sem));
	pthread_mutex_unlock(&mutex);

	while(1) {

		sem_wait(&(local_struct->instance_sem));
		pthread_mutex_lock(&mutex);

		int size_clave = strlen(local_struct->clave);
		int size_valor = strlen(local_struct->valor);
		int messageLength = 5 + size_clave + 4 + size_valor;

		char * mensajes = (char *) malloc (messageLength);
		memcpy(mensajes, &(local_struct->operacion), 1);
		memcpy(mensajes+1, &size_clave, 4);
		memcpy(mensajes+5, local_struct->clave, size_clave);
		memcpy(mensajes+size_clave+5, &size_valor, 4);
		memcpy(mensajes+size_clave+9, local_struct->valor, size_valor);

		log_info(logger, "Enviandole a la instancia %d bytes", messageLength);
		send(socket_local, mensajes, messageLength, 0);
		free(mensajes);

        rc = recv(socket_local, &buffer, 1, 0);
        if (rc == 0) {
		 log_error(logger, "  recv() failed");
		 close_conn = TRUE;
		 break;
        }

        pthread_mutex_unlock(&mutex);
	}


	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
	}
}

void _esi(int socket_local) {
	int rc = 0, close_conn = 0, id_esi = 1, instancia_destino = -1;
	int message_length = 0;
	unsigned char identificador = 0;
	char * clave = NULL;
	char * valor = NULL;
	t_clave * clave_diccionario = NULL;

	while(1) {

        rc = recv(socket_local, &identificador, 1, 0);
        if (rc == 0) {
        	log_error(logger, "  recv() failed");
        	close_conn = TRUE;
        	break;
        }

        switch(identificador) {
        	case ESI_GET:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, clave, message_length, 0);
        		clave[message_length] = '\0';
        		log_info(logger, "La ESI %d me esta pidiendo la clave %s", id_esi, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
        			if(clave_diccionario->tomada) {
        				//informar esi error
        				//informar planificador que clave esta esperando esa esi (ESI_GET, clave)
        			} else {
        				//informar esi todo ok
        				//Ver si esto sirve: informar planificador que clave tomo esa esi (esi_tomo_clave, clave)
        			}
        		} else {
        			instancia_destino = distribuir(clave, NULL, ESI_GET);
        			dictionary_put(diccionario_claves, clave, clave_create(id_esi, instancia_destino, true));
        			identificador = ESI_OK;
        			send(socket_local, &identificador, 1, 0);
        			//informar esi todo ok
					//Ver si esto sirve: informar planificador que clave tomo esa esi
        		}
        		break;
        	case ESI_SET:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, &clave, message_length, 0);
        		clave[message_length] = '\0';
        		rc = recv(socket_local, &message_length, 4, 0);
        		valor = malloc(message_length + 1);
				rc = recv(socket_local, &valor, message_length, 0);
				valor[message_length] = '\0';
        		log_info(logger, "La ESI %d quiere guardar el valor %s en la clave %s", id_esi, valor, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
        			if(clave_diccionario->tomada && clave_diccionario->esi != id_esi) {
        				//informar esi error
        			} else if (!(clave_diccionario->tomada)) {
        				//informar esi error
        			}else {
        				modificar_valor_clave(clave, valor, clave_diccionario->instancia);
        				identificador = ESI_OK;
						send(socket_local, &identificador, 1, 0);
        				//informar esi todo ok
        			}
        		} else {
        			//informar esi error
        			log_error(logger, "Intentando hacer un SET a una clave inexistente");
        		}
        	    break;
        	case ESI_STORE:
        		rc = recv(socket_local, &message_length, 4, 0);
        		clave = malloc(message_length + 1);
        		rc = recv(socket_local, &clave, message_length, 0);
        		clave[message_length] = '\0';
        		log_info(logger, "La ESI %d quiere hacer STORE de la clave %s", id_esi, clave);
        		if(dictionary_has_key(diccionario_claves, clave)) {
					clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
					if(clave_diccionario->tomada && clave_diccionario->esi != id_esi) {
						//informar esi error
					} else if (!(clave_diccionario->tomada)) {
						//informar esi error
					}else {
						store_clave(clave, clave_diccionario->instancia);
						//informar esi todo ok
						//informar planificador que clave se libero (ESI_STORE, clave)
						identificador = ESI_OK;
						send(socket_local, &identificador, 1, 0);
					}
				} else {
					//informar esi error
					log_error(logger, "Intentando hacer un STORE a una clave inexistente");
				}
        	    break;
        	default:
        		break;
        }
	}


	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
	}

}

void _planificador(int socket_local) {

}
