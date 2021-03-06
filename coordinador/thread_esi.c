#include "socket.h"

void liberar_claves(char ** claves_tomadas, int cantidad_claves) {
	int i = 0;
	t_clave * clave_diccionario;

	for(i = 0; i < cantidad_claves; i++) {
		if(dictionary_has_key(diccionario_claves, claves_tomadas[i])) {
			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, claves_tomadas[i]);
			clave_diccionario->tomada = false;
			log_info(logger, "Liberando la clave: %s", claves_tomadas[i]);
		}
	}

}

void _esi(int socket_local) {
	int rc = 0, close_conn = 0, id_esi = 0, instancia_destino = -1, clavesDisponibles = 0;
	int message_length = 0;
	unsigned char identificador = 0;
	char * clave = NULL;
	char * valor = NULL;
	t_clave * clave_diccionario = NULL;
	t_instancia * instancia = NULL;
	char * claves_tomadas[50];
	bool exit_status = false;

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
        			log_info(logger, "Existe la clave '%s' en el diccionario", clave);
        			clave_diccionario = (t_clave * ) dictionary_get(diccionario_claves, clave);
        			if(clave_diccionario->tomada) {
        				log_error(logger, "La clave '%s' esta tomada", clave);
        				identificador = ESI_BLOCK;
    					send(socket_local, &identificador, 1, 0);
        			} else {
        				log_info(logger, "La clave '%s' no esta tomada", clave);
            			clave_diccionario->tomada = true;
            			clave_diccionario->esi = id_esi;
            			//clave_diccionario->instancia = -1;
            			claves_tomadas[clavesDisponibles++] = clave;
            			identificador = ESI_OK;
            			send(socket_local, &identificador, 1, 0);
        			}
        		} else {
        			log_info(logger, "No existe la clave '%s' en el diccionario", clave);
        			dictionary_put(diccionario_claves, clave, clave_create(id_esi, -1, true));
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
        				log_error(logger, "Esta clave esta tomada por la esi %d", clave_diccionario->esi);
        				identificador = ESI_ERROR_CLAVE_NO_BLOQ;
        				exit_status = true;
    					send(socket_local, &identificador, 1, 0);
        				//informar esi error
        			} else if (!(clave_diccionario->tomada)) {
        				log_error(logger, "Esta clave no estaba tomada");
        				identificador = ESI_ERROR_CLAVE_NO_BLOQ;
        				exit_status = true;
    					send(socket_local, &identificador, 1, 0);
        				//informar esi error
        			}else {
        				if(clave_diccionario->instancia < 0) {
        					log_error(logger, "Esta clave no estaba en ninguna instancia");
							while(!instancia) {
								instancia = distribuir(clave);
								log_info(logger, "Elegi instancia, intento hacer el set");
								instancia = modificar_valor_clave(clave, valor, (instancia->id - 1));
								if(!instancia) {
									instancia = NULL;
									log_error(logger, "Esa instancia fallo, voy a probar si hay otra");
								}
							}
							clave_diccionario->instancia = (instancia->id - 1);
							identificador = ESI_OK;
        				} else {
        					log_error(logger, "Esta clave ya estaba en alguna instancia %d", clave_diccionario->instancia);
        					instancia = modificar_valor_clave(clave, valor, clave_diccionario->instancia);
							if(!instancia || !instancia->status) {
								dictionary_remove(diccionario_claves, clave);
								exit_status = true;
								identificador = ESI_ERROR_CLAVE_INACC;
								log_error(logger, "Intentando hacer store de una clave inaccesible");
							} else {
								identificador = ESI_OK;
							}
        				}
						//informar esi
						log_info(logger, "Informo a la ESI %d", identificador);
						send(socket_local, &identificador, 1, 0);
        			}
        		} else {
        			//informar esi error
    				identificador = ESI_ERROR_CLAVE_NO_IDEN;
    				exit_status = true;
					send(socket_local, &identificador, 1, 0);
        			log_error(logger, "Intentando hacer un SET a una clave inexistente");
        		}
				if(instancia && instancia->status)
					informar_planificador(clave, COORDINADOR_SET);
				else
					informar_planificador(clave, COORDINADOR_ERROR);
        		log_info(log_operaciones, "ESI %d - SET %s %s", id_esi, clave, valor);
        		instancia = NULL;
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
						log_error(logger, "Esta clave no estaba tomada");
	    				identificador = ESI_ERROR_CLAVE_NO_BLOQ;
	    				exit_status = true;
						send(socket_local, &identificador, 1, 0);
					} else if (!(clave_diccionario->tomada)) {
						//informar esi error
						log_error(logger, "Esta clave no estaba tomada");
						exit_status = true;
	    				identificador = ESI_ERROR_CLAVE_NO_BLOQ;
						send(socket_local, &identificador, 1, 0);

					}else {
						instancia = store_clave(clave, clave_diccionario->instancia);
						//informar esi todo ok
						//informar planificador que clave se libero (ESI_STORE, clave)
						log_info(logger, "Esperando instancia");
						sem_wait(&mutex_instancia);
						if(!instancia || !instancia->status) {
							dictionary_remove(diccionario_claves, clave);
							exit_status = true;
							identificador = ESI_ERROR_CLAVE_INACC;
							log_error(logger, "Intentando hacer store de una clave inaccesible");
						} else {
							identificador = ESI_OK;
							log_info(logger, "Informo a la ESI %d", identificador);
						}
						send(socket_local, &identificador, 1, 0);
						clave_diccionario->tomada = false;
						clave_diccionario->esi = -1;
					}
				} else {
					//informar esi error
    				identificador = ESI_ERROR_CLAVE_NO_IDEN;
    				exit_status = true;
					send(socket_local, &identificador, 1, 0);
					log_error(logger, "Intentando hacer un STORE a una clave inexistente");
				}
				if(instancia && instancia->status)
					informar_planificador(clave, COORDINADOR_STORE);
				else
					informar_planificador(clave, COORDINADOR_ERROR);
        		log_info(log_operaciones, "ESI %d - STORE %s", id_esi, clave);
        		instancia = NULL;
        	    break;
        	default:
        		//informar esi error
				identificador = ESI_ERROR_CLAVE_LARGA;
				exit_status = true;
				send(socket_local, &identificador, 1, 0);
				log_error(logger, "La clave era demasiado larga");
        		informar_planificador(clave, COORDINADOR_ERROR);
        		break;
        }
	}


	if (close_conn) {
		shutdown(socket_local, SHUT_RDWR);
		if(!exit_status) liberar_claves(claves_tomadas, clavesDisponibles);

	}
}
