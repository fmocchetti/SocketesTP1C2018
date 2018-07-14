/*
 * planificador.c
 *
 *  Created on: 12 may. 2018
 *      Author: pdelucchi
 */

#include "socket.h"
#include "protocolo.h"
#include "algoritmos.h"
#include "consola.h"
#include <pthread.h>


void generate_server();
void generate_planning();
void generate_console();

#define IDENTIDAD "planificador"



int main (int argc, char *argv[])
{
	//inicializo los semaforos a utilizar
	sem_init(&new_process, 0, 0);
	//sem_init(&replanificar, 0, 0);
	sem_init(&give_me_a_new_process, 0, 1);
	sem_init(&mutex_listos, 0, 1);
	sem_init(&mutex_ejecucion, 0, 1);
	sem_init(&sem_pausar_planificacion, 0, 1);
	sem_init(&sem_pausar_algoritmo, 0, 1);

	config_file = config_create("planificador.conf");
	//Configuro los logs a utilizar
	configure_logger();
	configure_logger_consola();

	//creo los threads a utilizar
	pthread_t thread_poll;
	pthread_t thread_planificador;

	//Creo las listas a utilizar
	listos = list_create();
	bloqueados = list_create();
	ejecucion = list_create();
	terminados = list_create();
	muertos = list_create();
	claves_tomadas = list_create();
	//Creo el diccionario de claves bloqueadas
	claves_bloqueadas = dictionary_create();

	//creo la conexion con el coord(datos obtenidos desde planificador.conf)
	socket_coord = create_client(config_get_string_value(config_file, "ip_coordinador"),config_get_string_value(config_file, "puerto_coordinador"));
	algoritmo_elegido =  config_get_int_value(config_file, "algoritmo_de_planificacion");
	//hago detach de los threads utilizados
	pthread_create(&thread_poll, NULL, (void*) generate_server, NULL);
	pthread_detach(thread_poll);
	pthread_create(&thread_planificador, NULL, (void*) generate_planning, NULL);
	pthread_detach(thread_planificador);
	consola();
/*
	pthread_create(&thread_consola, NULL, (void*) generate_console, NULL);
	pthread_detach(thread_consola);*/

    getchar();
	return 0;
}

void generate_server(){
	log_info(logger,"Creating Server \n");
	create_server(32, 3 * 60 * 1000, THREAD_CONNECTION, config_get_int_value(config_file, "puerto_de_escucha"));
}

void generate_planning(){
	printf("Entre al thread de planificacion \n");
	switch(algoritmo_elegido) {
			case SJFSD:
				printf("Algoritmo seleccionado SJFSD\n");
				get_keys_bloqueadas_de_entrada(socket_coord);
				sjfsd();
				break;
			case SJFCD:
				printf("Algoritmo seleccionado SJFCD\n");
				get_keys_bloqueadas_de_entrada(socket_coord);
				sjfcd();
				break;
			case HRRN:
				printf("Algoritmo seleccionado HRRN\n");
				get_keys_bloqueadas_de_entrada(socket_coord);
				hrrn();
				break;
			default:
				log_error(logger,"El algoritmo seleccionado no existe\n");
				break;
	}
	/*
	char * config_plani = (char*) calloc(sizeof(char), strlen(config_get_string_value(config_file, "algoritmo_de_planificacion")) + 1);
	//char *config_plani = malloc(sizeof(char));
	strcpy(config_plani, config_get_string_value(config_file, "algoritmo_de_planificacion"));
	if(strcmp(config_plani,"hrrn")==0){
		free(config_plani);
		printf("HRRN\n");
		get_keys_bloqueadas_de_entrada(socket_coord);
		hrrn();
	}
	if(strcmp(config_plani,"sjfsd")==0){
		free(config_plani);
		printf("SJFSD\n");
		get_keys_bloqueadas_de_entrada(socket_coord);
		sjfsd();
	}
	if(strcmp(config_plani,"sjfcd")==0){
		free(config_plani);
		printf("SJFCD\n");
		get_keys_bloqueadas_de_entrada(socket_coord);
		sjfcd();
	}
	else{
		free(config_plani);
		printf("ERROR\n");
	}*/
	//while(1);
	printf("adios mundo cruel \n");
}

void generate_console(){
	consola();
}
