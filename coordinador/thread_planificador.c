#include "socket.h"

void _planificador(int socket_local) {
	int rc = 0, close_conn = 0;
	int size_clave = 0, size_value = 0, cantidad_claves = 0, id_esi = 0;
	t_instancia * instancia_destino = NULL;
	unsigned char identificador = 0;
	char * clave = NULL;
	t_clave * clave_diccionario = NULL;

	thread_planificador = planificador_create();

	while(1) {
		log_info(logger, "Espero solicitud planificador");
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
				sem_post(&mutex_esi);
				log_info(logger, "Desbloqueo ESI");
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
	        			}
	        		} else {
	        			dictionary_put(diccionario_claves, clave, clave_create(0, -1, true));
	        		}
	        		log_info(logger, "Se bloqueo la clave");
	        		//free(clave);//////////TE AGREGUE ESTE FREE CHANGUI
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
					clave_diccionario->tomada = false;
					log_error(logger, "clave desbloqueada");
				} else {
					log_error(logger, "Intentando desbloquear una clave inexistente");
				}
				//free(clave);//////////TE AGREGUE ESTE FREE CHANGUI
				break;
			case PLANIFICADOR_STATUS:
				recv(socket_local, &size_clave, 4, 0);
				clave = (char *)malloc (size_clave + 1);
				recv(socket_local, clave, size_clave, 0);
				clave[size_clave] = '\0';

				log_info(logger, "Voy a consultar estado de la clave '%s'", clave);

				if(dictionary_has_key(diccionario_claves, clave)) {
					clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);

					identificador = INSTANCIA_STATUS;
					t_instancia * o_instancia;

					log_info(logger, "La clave esta en la instancia: %d", clave_diccionario->instancia);
					o_instancia = list_get(list_instances, (clave_diccionario->instancia));
					if(!o_instancia->status){
						dictionary_remove(diccionario_claves, clave);
						o_instancia = simular(clave);

						if(!o_instancia) {
							identificador = PLANIFICADOR_NO_INSTANCIAS;
							send(socket_local, &identificador, 1, 0);
						} else {
							char * response = (char *) malloc(5);
							identificador = PLANIFICADOR_SIMULACION;

							memcpy(response, &identificador, 1);
							memcpy(response+1, &(o_instancia->id), 4);

							send(socket_local, response, 5, 0);
							log_info(logger, "Enviandole simulacion al planificador, la instancia es %d", o_instancia->id);
							free(response);
						}

					} else {
						o_instancia->clave = clave;
						o_instancia->valor = NULL;
						o_instancia->operacion = INSTANCIA_STATUS;
						sem_post(&(o_instancia->instance_sem));
						sem_wait(&mutex_instancia);
						sem_wait(&mutex_status);

						log_info(logger, "La clave %s tiene el valor %s", o_instancia->clave, o_instancia->valor);
						identificador = PLANIFICADOR_CLAVE_EXISTENTE;

						if(o_instancia->valor)
							size_value = strlen(o_instancia->valor);
						else
							size_value = 0;

						messageLength = size_value + 4 + 1 + 4;
						char * response = (char *) malloc(messageLength);

						memcpy(response, &identificador, 1);
						memcpy(response+1, &(o_instancia->id), 4);
						if(size_value > 0) {
							memcpy(response+5, &(size_value), 4);
							memcpy(response+9, o_instancia->valor, size_value);
						} else {
							size_value = 255;
							memcpy(response+5, &(size_value), 4);
						}

						send(socket_local, response, messageLength, 0);
						log_info(logger, "Enviandole al planificador %d bytes", messageLength);
						free(response);
					}
				} else {
					t_instancia * o_instancia = simular(clave);

					if(!o_instancia) {
						identificador = PLANIFICADOR_NO_INSTANCIAS;
						send(socket_local, &identificador, 1, 0);
					} else {
						char * response = (char *) malloc(5);
						identificador = PLANIFICADOR_SIMULACION;

						memcpy(response, &identificador, 1);
						memcpy(response+1, &(o_instancia->id), 4);

						send(socket_local, response, 5, 0);
						log_info(logger, "Enviandole simulacion al planificador, la instancia es %d", o_instancia->id);
						free(response);
					}

				}
				break;
			case PLANIFICADOR_GET_BLOQUEAR:
				recv(socket_local, &cantidad_claves, 4, 0);
				for(int i=0; i < cantidad_claves; i++) {
					recv(socket_local, &size_clave, 4, 0);
					clave = (char *)malloc (size_clave + 1);
					recv(socket_local, clave, size_clave, 0);
					clave[size_clave] = '\0';
					recv(socket_local, &id_esi, 4, 0);

					log_info(logger, "Voy a bloquear la clave '%s'", clave);
					printf("%s\n",clave);
	        		if(dictionary_has_key(diccionario_claves, clave)) {
	        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
	        			if(!clave_diccionario->tomada) {
	        				clave_diccionario->esi = id_esi;
	            			clave_diccionario->tomada = true;
	            			clave_diccionario->instancia = -1;
	        			}
	        		} else {
	        			dictionary_put(diccionario_claves, clave, clave_create(id_esi, -1, true));
	        		}
	        		sem_wait(&mutex_instancia);
	        		log_info(logger, "Se bloqueo la clave en la instancia");
	        		//free(clave);//////////TE AGREGUE ESTE FREE CHANGUI
				}
				break;
        }
	}

	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
	}

}
