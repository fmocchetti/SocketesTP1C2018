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

 while(strcmp(input,"salir") != 0){
 	opciones();
 	//char *input = malloc(sizeof(char));
 	//gets(input);
 	fgets(input, sizeof(input), stdin);
 	printf("HOLAAAA\n");
 	printf("DICE %s\n", input);
 	if(!strncmp(input,"pausar", 6)){
 		pausar();
 	} else if(!strncmp(input,"continuar", 9)){
 		continuar();
 	} else if(!strncmp(input,"bloquear", 8)){
 		esi_bloqueada_de_entrada = 1;
 		bloquear();
 		esi_bloqueada_de_entrada = 0;
 	} else if(!strncmp(input,"desbloquear", 11)){
 		desbloquear();
 	} else if(!strncmp(input,"listar", 6)){
 		listar();
 	}else if(!strncmp(input,"kill", 4)){
 		kill();
 	} else if(!strncmp(input,"status", 6)){
 		status();
 	} else if(!strncmp(input,"deadlock",8)){
 		deadlock();
 	}
  }


 printf("Nos vimo perro");
 free(input);
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

void bloquear(){
	char key[40];
	int id = 0;

	printf("inserte clave\n");
	//gets(key);
	fgets(key, sizeof(key), stdin);
	printf("inserte id\n");
	//gets(id);
	//fgets(id, sizeof(id), stdin);
	scanf("%d",&id);
	log_info(logger, "Proceso Bloqueado key:%s, id:%d\n", key, id);
	ESI_GET(key,id,0);

}

void desbloquear(){
	char key[40];
	printf("inserte clave\n");
	fgets(key, sizeof(key), stdin);
	ESI_STORE(key);
	log_info(logger, "Proceso Desbloqueado key:%s\n", key);
}

void listar(){
	estadoListas();
}

void kill(){
	char *id = malloc(sizeof(char));
	printf("Inserte ID del proceso\n");
	gets(id);
	printf("El proceso %s murio viejo\n", id);
	free(id);
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



