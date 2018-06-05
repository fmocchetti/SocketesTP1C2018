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

 char *input = malloc(sizeof(char));
 printf( "----------Bienvenido al planificador-----------\n" );
 opciones();
 while(strcmp(input,"salir") != 0){
	gets(input);
	printf("DICE %s\n", input);
	if(strcmp(input,"pausar")==0){
		pausar();
	}
	else{
		if(strcmp(input,"continuar")==0){
			continuar();
		}
		else{
			if(strcmp(input,"bloquear")==0){
				bloquear();
			}
			else{
				if(strcmp(input,"desbloquear")==0){
					desbloquear();
				}
				else{
					if(strcmp(input,"listar")==0){
						listar();
					}
					else{
						if(strcmp(input,"kill")==0){
							kill();
							}
						else{
							if(strcmp(input,"status")==0){
								status();
							}
							if(strcmp(input,"deadlock")==0){
								deadlock();
							}
							}
						}
					}
				}
			}
		}
	if(strcmp(input,"salir")!=0){
	opciones();
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
	log_info(logger,"Planificacion Pausada");
}

void continuar(){
	log_info(logger,"Planificacion Restaurada");
}

void bloquear(){
	char *key = malloc(sizeof(char));
	char *id = malloc(sizeof(char));

	printf("inserte clave\n");
	gets(key);
	printf("inserte id\n");
	gets(id);
	log_info(logger, "Proceso Bloqueado key:%s, id:%s\n", key, id);

	free(key);
	free(id);

}

void desbloquear(){
	char *key = malloc(sizeof(char));
	char *id = malloc(sizeof(char));

	printf("inserte clave\n");
	gets(key);
	printf("inserte id\n");
	gets(id);
	log_info(logger, "Proceso Desbloqueado key:%s, id:%s\n", key, id);

	free(key);
	free(id);

}

void listar(){
	char *resource = malloc(sizeof(char));

	printf("inserte recurso a consultar\n");
	gets(resource);
	log_info(logger, "Los procesos encolados son\n");
	free(resource);
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



