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

void consola(){

 char input[100];

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
 		status();
 	} else if(!strncmp(input,"deadlock",8)){
 		deadlock();
 	}
 	opciones();
 	printf("LLegue al final!\n");
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
	send(socket,&key,tamanio_clave,0);


	log_info(logger, "Proceso Bloqueado key:%s, id:%d\n", key, id);
	ESI_GET(key,id,0);

}

void desbloquear(){
	char key[40];
	printf("inserte clave\n");
	//fgets(key, sizeof(key), stdin);
	scanf("%s", key);
	desbloquear_del_diccionario(key);
	log_info(logger, "Proceso Desbloqueado key:%s\n", key);
}

void listar(){
	char key[40];
	int id = 0;
	int dicc_size = 0;
	int queue_vacia = 0;
	int id_esi_bloqueado = 0;
	int resultado_lista_satisfy = 0;
	printf("inserte clave\n");
	//fgets(key, sizeof(key), stdin);
	scanf("%s", key);
	t_queue * queue_clave;
	t_queue * queue_temporal;


	if(!list_is_empty(claves_tomadas)) {
		strcpy(clave_bloqueada_global,key);
		resultado_lista_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave);

		printf("Satisface la lista? %d\n\n",resultado_lista_satisfy);

		id = list_size(claves_tomadas);
		printf("Size de la lista de claves tomadas %d\n\n",id);
		claves* clave_temporal = (claves*) malloc(sizeof(claves));
		clave_temporal = list_get(claves_tomadas, 0);
		printf("La CLAVE tomada es %s\n\n",clave_temporal->claveAEjecutar);


		if(resultado_lista_satisfy){
			claves* clave_temporal = (claves*) malloc(sizeof(claves));
			clave_temporal = list_find(claves_tomadas, (void*)identificador_clave);
			log_info(logger,"El recurso se encuentra tomado actualmente por la ESI %d\n",clave_temporal->id_ESI);
		}
		else{
			log_info(logger,"El recurso no se encuentra tomado actualmente por una ESI\n");
		}
	}

	if(dictionary_has_key(claves_bloqueadas,key)){
	queue_clave = dictionary_get(claves_bloqueadas,key);
	queue_temporal = queue_create();

	queue_vacia = queue_is_empty(queue_clave);
	while(!queue_vacia){
		id_esi_bloqueado = (int)queue_pop(queue_clave);
		log_info(logger,"La ESI del ID %d se encuentra bloqueada esperando al recurso %s",id_esi_bloqueado,key);
		queue_push(queue_temporal,(int)id_esi_bloqueado);
		queue_vacia = queue_is_empty(queue_clave);
		}
	queue_clean(queue_clave);
	dictionary_remove_and_destroy(claves_bloqueadas, key, (void*)clave_dictionary_destroy);
	dictionary_put(claves_bloqueadas, key, queue_temporal);
	}
	else{
		log_info(logger,"No hay ESIs bloqueadas esperando al rescurso %s",key);
	}
}//PENSAR SI TENGO QUE LIBERAR LAS QUEUES EN ALGUN MOMENTO

void kill(){
	int id = 0;
	int resultado_satisfy = 0;
	printf("Inserte ID de la ESI a matar\n");
	scanf("%d", &id);

	id_esi_global = id;
	printf("------RECIBI MENOS DE 1\n");
	resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);
	printf("------RECIBI MENOS DE 2\n");
	while(resultado_satisfy == 1){
	//////////elimino de lista de claves tomadas la ESI y hago un Store avisando que otra clave puede pasarse a ready
		printf("------RECIBI MENOS DE 0\n");
		claves* clave_temporal = (claves*) malloc(sizeof(claves));
		clave_temporal = list_remove_by_condition(claves_tomadas,identificador_clave_por_idESI);
		ESI_STORE(clave_temporal->claveAEjecutar);
		free(clave_temporal);
		//list_remove_and_destroy_by_condition(claves_tomadas,(void*)identificador_clave_por_idESI,(void*)clave_destroy);
		resultado_satisfy = list_any_satisfy(claves_tomadas, (void*)identificador_clave_por_idESI);

		}
		//hago close del socket
	resultado_satisfy = list_any_satisfy(ejecucion, (void*)identificador_ESI);
	if(resultado_satisfy==1){
		ESI *nodo_lista_ejecucion = (ESI*) malloc(sizeof(ESI));
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		laWeaReplanificadoraFIFO(muertos,ejecucion);
		_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
		nodo_lista_ejecucion->cantidadDeLineas = 0;
		free(nodo_lista_ejecucion);
	printf("1- El proceso %s murio viejo\n", id);
	}
	else{
		printf("NO ESTA EN EJECUCION\n");
	}
	resultado_satisfy = list_any_satisfy(listos, (void*)identificador_ESI);
	if(resultado_satisfy==1){
		ESI *nodo_lista_ejecucion = (ESI*) malloc(sizeof(ESI));
		nodo_lista_ejecucion =  (ESI*) list_get(ejecucion, 0);
		laWeaReplanificadoraFIFO(muertos,ejecucion);
		_exit_with_error(nodo_lista_ejecucion->socket_esi, "La ESI en ejecucion murio", NULL);
		nodo_lista_ejecucion->cantidadDeLineas = 0;
		free(nodo_lista_ejecucion);
		printf("2- El proceso %s murio viejo\n", id);
	}
	else{
		printf("NO ESTA EN LISTOS\n");
	}
}

void status(){
	char *key = malloc(sizeof(char));
	printf("Inserte key\n");
	gets(key);
	printf("status de la instancia\n");
	free(key);
}

void deadlock(){
	printf("deadlocks del sistema");
}



