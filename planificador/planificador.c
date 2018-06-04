/*
 * planificador.c
 *
 *  Created on: 12 may. 2018
 *      Author: pdelucchi
 */

#include "socket.h"
#include "protocolo.h"
#include "algoritmos.h"
#include <pthread.h>


void generate_poll();
void generate_planning();
void generate_coord_connection();

#define IDENTIDAD "planificador"



int main (int argc, char *argv[])
{

	//inicializo los semaforos a utilizar
	sem_init(&new_process, 0, 0);
	//sem_init(&replanificar, 0, 0);
	sem_init(&give_me_a_new_process, 0, 1);
	sem_init(&mutex_listos, 0, 1);
	sem_init(&mutex_ejecucion, 0, 1);

	config_file = config_create("planificador.conf");

	//creo los threads a utilizar
	pthread_t thread_poll;
	pthread_t thread_planificador;
	pthread_t thread_coord_connection;
	//Creo las listas a utilizar
	listos = list_create();
	bloqueados = list_create();
	ejecucion = list_create();
	terminados = list_create();
	//Creo el diccionario de claves bloqueadas
	claves_bloqueadas = dictionary_create();


	//Configuro el log a utilizar
		configure_logger();

	pthread_create(&thread_poll, NULL, (void*) generate_poll, NULL);
	pthread_detach(thread_poll);
	pthread_create(&thread_planificador, NULL, (void*) generate_planning, NULL);
	pthread_detach(thread_planificador);
	/*
	pthread_create(&thread_coord_connection, NULL, (void*) generate_coord_connection, NULL);
	pthread_detach(thread_coord_connection);*/


	printf("%s\n",IDENTIDAD);// borrar


    getchar();
	return 0;
}

void generate_poll(){
	printf("Entre al thread del poll \n");
	create_server(32, 3 * 60 * 1000);
}

void generate_planning(){
	printf("Entre al thread de planificacion \n");
	char *config_plani = malloc(sizeof(char));
	strcpy(config_plani, config_get_string_value(config_file, "algoritmo_de_planificacion"));
	printf("DICE %s\n",config_get_string_value(config_file, "algoritmo_de_planificacion"));
	//strcpy(config_plani,"sjfcd");

	/*if(strcmp(config_plani,"fifo")){
		printf("FIFO\n");
		fifo();
	}*/
	if(strcmp(config_plani,"sjfsd")){
		printf("SJFSD\n");
		sjfsd();
	}
	if(strcmp(config_plani,"sjfcd")){
		printf("SJFCD\n");
		sjfcd();
	}
	else{
		printf("ERROR\n");
	}
	//while(1);
	printf("adios mundo cruel \n");
}

void generate_coord_connection(){
	printf("Entre al thread del coord \n");
	int socket_coord = create_client("127.0.0.1","12345");
}

