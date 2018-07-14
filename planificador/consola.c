/*
 * consola.c
 *
 *  Created on: 5 jun. 2018
 *      Author: pdelucchi
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "consola.h"
void looking_for_deadlocks();

void consola(){


 char input[100] = "";

 printf( "----------Bienvenido al planificador-----------\n" );
 opciones();

 while(strncmp(input,"salir",5) != 0){
 	//char *input = malloc(sizeof(char));
 	//gets(input);
 	//fgets(input, sizeof(input), stdin);
	 scanf("%s", input);

 	if(!strncmp(input,"pausar", 6)){
 		pausar();
 	} else if(!strncmp(input,"continuar", 9)){
 		continuar();
 	} else if(!strncmp(input,"bloquear", 8)){
 		esi_bloqueada_de_entrada = 1;
 		bloquear(socket_coord);
 		esi_bloqueada_de_entrada = 0;
 	} else if(!strncmp(input,"desbloquear", 11)){
 		desbloquear();
 	} else if(!strncmp(input,"listar", 6)){
 		listar();
 	} else if(!strncmp(input,"kill", 4)){
 		kill();
 	} else if(!strncmp(input,"status", 6)){
 		//status();
 	} else if(!strncmp(input,"deadlock",8)){
 		deadlock();
 	}
 	else{
 		log_error(logger,"mensaje incorrecto!\n");
 	}
 	opciones();
  }


 printf("Nos vimo perro");
 }

void configure_logger_consola() { //Configuracion del log del planificador
  logger = log_create("consola.log", "Consola", true, LOG_LEVEL_INFO);
}

void opciones(){
	printf("Seleccione la opcion deseada:\n");
	printf("comando       - descripcion\n");
	printf("-----------------------------------------------\n");
	printf("pausar        - Pausar Planificacion\n");
	printf("continuar     - Continuar Planificacion\n");
	printf("bloquear      - Bloquear proceso\n");
	printf("desbloquear   - Desbloquear proceso\n");
	printf("listar        - Listar procesos enconlados\n");
	printf("kill          - matar un proceso\n");
	printf("status        - Status\n");
	printf("deadlock      - Deadlocks de la instancia\n");
	printf("-----------------------------------------------\n");
	printf("Ingrese Input\n");
}

void pausar(){
	int sem_value = 0;
	log_info(logger,"Entre a Pausa");
	sem_getvalue(&sem_pausar_planificacion,&sem_value);
	if(sem_value<1){
		log_info(logger,"La planificacion ya se encuentra pausada");
		printf("La planificacion ya se encuentra pausada\n");
	}
	else{
		sem_wait(&sem_pausar_planificacion);
		sem_wait(&sem_pausar_algoritmo);
		log_info(logger,"Planificacion Pausada");
	}

}

void continuar(){
	int sem_value = 0;
	log_info(logger,"Entre a Continuar");
	sem_getvalue(&sem_pausar_planificacion,&sem_value);
	if(sem_value>0){
		log_info(logger,"La planificacion no se encuentra pausada");
	}
	else{
		sem_post(&sem_pausar_planificacion);
		sem_post(&sem_pausar_algoritmo);
		sem_post(&sem_pausar_algoritmo);
		log_info(logger,"Planificacion Restaurada");
	}
}

void bloquear(int socket){
	char key[40];
	int id = 0;
	unsigned char mensaje_coord = 33;
	int cantidad_bloqueados = 1;
	int tamanio_clave = 0;

	printf("inserte clave\n");
	scanf("%s", key);
	printf("inserte id\n");
	scanf("%d",&id);

	send(socket,&mensaje_coord,1,0);
	send(socket,&cantidad_bloqueados,sizeof(cantidad_bloqueados),0);

	tamanio_clave = strlen(key);
	printf("largo de la clave %d\n", tamanio_clave);
	send(socket,&tamanio_clave,sizeof(tamanio_clave),0);
	send(socket,key,tamanio_clave,0);

	log_info(logger, "Proceso Bloqueado key:%s, id:%d\n", key, id);
	ESI_GET(key,id,0);

}

void desbloquear(){
	char key[40];
	printf("inserte clave\n");
	//fgets(key, sizeof(key), stdin);
	scanf("%s", key);
	desbloquear_del_diccionario(key,socket_coord);
}

/*
void listar_tomadas(){
	void mostrar_listos(claves* esi1) {
		     printf("'%s'\n",esi1->claveAEjecutar);
		     }
		claves_tomadas = list_map(claves_tomadas, (void*) mostrar_listos);
}*/

void listar(){
	char key[40];
	printf("inserte clave\n");
	//fgets(key, sizeof(key), stdin);
	scanf("%s", key);
	listar_tomadas(key);

}

void listar_tomadas(char* key){
	int list_vacia = 0;
	int id = 0;
	int resultado_lista_satisfy = 0;
	t_list * list_clave;
	list_vacia = list_is_empty(claves_tomadas);

	if(!list_is_empty(claves_tomadas)) {
		strcpy(clave_bloqueada_global,key);
		resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave);
		id = list_size(claves_tomadas);

		if(resultado_lista_satisfy){
			claves* clave_temporal = (claves*) malloc(sizeof(claves));
			clave_temporal = list_find(claves_tomadas, (void*)identificador_clave);
			log_info(logger,"El recurso se encuentra tomado actualmente por la ESI %d",clave_temporal->id_ESI);
		}
		else{
			log_info(logger,"El recurso no se encuentra tomado actualmente por una ESI");
		}
	}

	if(dictionary_has_key(claves_bloqueadas,key)){
	list_clave = dictionary_get(claves_bloqueadas,key);
		if(!list_is_empty(list_clave)){
			void mostrar_lista_diccionario(int id_esi) {
				log_info(logger,"La ESI del ID %d se encuentra bloqueada esperando al recurso", id_esi);
					 }
			list_clave = list_map(list_clave,(void*) mostrar_lista_diccionario);
		}
		else{
			//log_info(logger,"Borre la entrada del diccionario de la clave porque la lista no tenia entradas");
		}
	}
	else{
		log_info(logger,"No hay ESIs bloqueadas esperando al rescurso %s",key);
	}
}

void kill(){
	int id = 0;
	int resultado_satisfy = 0;
	printf("Inserte ID de la ESI a matar\n");
	scanf("%d", &id);

	id_esi_global_kill = id;
	id_esi_global = id;
	resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
	while(resultado_satisfy == 1){
	//elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
		printf("------RECIBI MENOS DE 0\n");
		claves* clave_temporal = NULL;
		clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
		ESI_STORE(clave_temporal->claveAEjecutar,0);
		free(clave_temporal);
		resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
		}
	//hago close del socket, chequeando todas las listas
	resultado_satisfy = list_any_satisfy(ejecucion, (void*)identificador_ESI_kill);
	if(resultado_satisfy==1){
		ESI *nodo_lista_ejecucion = NULL;
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		nodo_lista_ejecucion->cantidadDeLineas = 0;
		laWeaReplanificadoraFIFO(muertos,ejecucion);
		_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
		log_error(logger,"El proceso %d murio viejo", id);
	}
	else{
		log_info(logger,"NO ESTA EN EJECUCION");
	}
	resultado_satisfy = list_any_satisfy(listos, (void*)identificador_ESI_kill);
	if(resultado_satisfy==1){
		ESI *nodo_lista_ejecucion = NULL;
		nodo_lista_ejecucion = list_remove_by_condition(listos,identificador_ESI_kill);
		nodo_lista_ejecucion->cantidadDeLineas = 0;
		list_add(muertos,nodo_lista_ejecucion);
		_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en listos murio", NULL);
		log_error(logger,"El proceso %d murio viejo", id);
	}
	else{
		log_info(logger,"NO ESTA EN LISTOS");
	}
	resultado_satisfy = list_any_satisfy(bloqueados, (void*)identificador_ESI_kill);
	if(resultado_satisfy==1){
		ESI *nodo_lista_ejecucion = NULL;
		nodo_lista_ejecucion = list_remove_by_condition(bloqueados,identificador_ESI_kill);
		nodo_lista_ejecucion->cantidadDeLineas = 0;
		list_add(muertos,nodo_lista_ejecucion);
		_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en bloqueados murio", NULL);
		log_error(logger,"El proceso %d murio viejo", id);
	}
	else{
		log_info(logger,"NO ESTA EN BLOQUEADOS");
	}
}

void status(int socket){
	char key[40];
	int tamanio_clave = 0;
	int resultado_clave = 0;
	int key_long = 0;
	int instanciaAGuardar = 0;
	unsigned char contestacionCoord = 0;
	unsigned char pedirStatus = 0; // definir que numero es de protocolo

	printf("inserte clave\n");
	scanf("%s", key);

	send(socket,&pedirStatus,sizeof(int),0);
	tamanio_clave = strlen(key);
	send(socket,&tamanio_clave,sizeof(int),0);
	send(socket,key,tamanio_clave,0);

	resultado_clave = recv(socket, &contestacionCoord, 1,0);
	if(resultado_clave == 90){ //protocolo 90 porque pinto
		recv(socket, &key_long, sizeof(int),0);
		char * valorClave = malloc(key_long);
		recv(socket, &valorClave, sizeof(key_long),0);
		log_info(logger,"El valor de la clave es '%s'",valorClave);
	}
	else{
		log_info(logger,"La clave no esta en ninguna instancia");
		recv(socket, &instanciaAGuardar, sizeof(int),0);
		log_info(logger,"La instancia donde se guardaria seria %d",instanciaAGuardar);
	}
	//Aviso cuales ESIs esperan esta clave
	listar_tomadas(key);
}

void deadlock(){
	looking_for_deadlocks();
}

void looking_for_deadlocks(){
	int i = 0;
	int j = 0;
	int k = 0;
	int cantidad_lista_tomados = 0;
	int cantidad_lista_dicc = 0;
	int id_esi_a_buscar = 0;
	int resultado_satisfy = 0;
	int contabilidad_resultados = 0;
	t_list* lista_dicc;
	t_list* lista_dicc2;
	t_list* lista_aux = list_create();
	//claves* nodo_tomados = (claves*) malloc(sizeof(claves));
	//claves* nodo_tomados2 = (claves*) malloc(sizeof(claves));
	claves* nodo_tomados = NULL;
	claves* nodo_tomados2 = NULL;

	cantidad_lista_tomados = list_size(claves_tomadas);
	if(cantidad_lista_tomados > 0){
		//reviso la cantidad de claves que hay tomadas
		for(i= 0; i < cantidad_lista_tomados; i++){
			//hago un list get de cada una mediante el for
			nodo_tomados = list_get(claves_tomadas,i);
			//reviso si el diccionario tiene la key asociada
			if(dictionary_has_key(claves_bloqueadas,nodo_tomados->claveAEjecutar)){
				//si esta, agarro la lista asociada a la key
				lista_dicc = dictionary_get(claves_bloqueadas,nodo_tomados->claveAEjecutar);
				cantidad_lista_dicc = list_size(lista_dicc);
				//reviso cada entrada de la lista mediante el for, para ver si esta tomada por alguna esi
				for(j= 0; j < cantidad_lista_dicc; j++){
					//tomo el elemento de la lista, para buscarlo en la lista de claves tomadas
					id_esi_a_buscar = (int)list_get(lista_dicc,j);
					id_esi_global = id_esi_a_buscar;
					resultado_satisfy = list_any_satisfy(claves_tomadas,(void*)identificador_clave_por_idESI);
					if(resultado_satisfy==1){
						contabilidad_resultados = list_count_satisfying(claves_tomadas,(void*)identificador_clave_por_idESI);
						for(k= 0; k < contabilidad_resultados; k++){
						//si esta, busco el nodo de la lista que tiene dicha esi
						nodo_tomados2 = list_find(claves_tomadas,(void*)identificador_clave_por_idESI);

						//log_info(logger,"El nodo de la lista encontrada es de la ESI %d", nodo_tomados2->id_ESI);
						//log_info(logger,"La clave tomada por dicha ESI es '%s'", nodo_tomados2->claveAEjecutar);
						//busco si existe una clave en el diccionario que tenga asociada dicha key
						if(dictionary_has_key(claves_bloqueadas,nodo_tomados2->claveAEjecutar)){
							//tomo la lista de esa entrada del diccionario y evaluo si existe la primer esi
							lista_dicc2 = dictionary_get(claves_bloqueadas,nodo_tomados->claveAEjecutar);
							id_esi_global = nodo_tomados->id_ESI;
							resultado_satisfy = list_any_satisfy(claves_tomadas,(void*)identificador_clave_por_idESI);
							if(resultado_satisfy == 1){
								if(nodo_tomados->id_ESI!=nodo_tomados2->id_ESI){
									log_info(logger,"Existe deadlock entre las esis %d y %d",nodo_tomados->id_ESI,nodo_tomados2->id_ESI);
									break;
								}
								else{
									nodo_tomados2 = list_remove_by_condition(claves_tomadas,(void*)identificador_clave_por_idESI);
									list_add(lista_aux,nodo_tomados2);
									//i--;
									cantidad_lista_tomados--;
								}
							}
							else{
								log_info(logger,"Las claves no estan en deadlock");
							}
						}
						else{
							nodo_tomados2 = list_remove_by_condition(claves_tomadas,(void*)identificador_clave_por_idESI);
																list_add(lista_aux,nodo_tomados2);
																i--;
																cantidad_lista_tomados--;
							log_info(logger,"BARRIENDO LAS DEMAS CLAVES DE LA ESI RELACIONADA EN BUSCA DE DEADLOCK");
						}
						}
					}
					else{
						//log_info(logger,"No existe ninguna clave de las bloqueadas tomando otra clave");
					}
				}
			}
			else{
				//log_info(logger,"No existe la clave '%s' en el diccionario, nadie la esta esperando",nodo_tomados->claveAEjecutar);
			}

		}
		list_add_all(claves_tomadas,lista_aux);
		list_destroy(lista_aux);
	}
	else{
		log_info(logger,"No hay nada en la lista de tomados");
	}
}
