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


void generate_poll();
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
	pthread_t thread_consola;

	//Creo las listas a utilizar
	listos = list_create();
	bloqueados = list_create();
	ejecucion = list_create();
	terminados = list_create();
	//Creo el diccionario de claves bloqueadas
	claves_bloqueadas = dictionary_create();

	//creo la conexion con el coord(datos obtenidos desde planificador.conf)
	//socket_coord = create_client(config_get_string_value(config_file, "ip_coordinador"),config_get_string_value(config_file, "puerto_coordinador"));

	//hago detach de los threads utilizados
	pthread_create(&thread_poll, NULL, (void*) generate_poll, NULL);
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

void generate_poll(){
	printf("Entre al thread del poll \n");
	create_server(32, 3 * 60 * 1000);
}

void generate_planning(){
	printf("Entre al thread de planificacion \n");
	char *config_plani = malloc(sizeof(char));
	strcpy(config_plani, config_get_string_value(config_file, "algoritmo_de_planificacion"));
	if(strcmp(config_plani,"fifo")==0){
		printf("FIFO\n");
		fifo();
	}
	if(strcmp(config_plani,"sjfsd")==0){
		printf("SJFSD\n");
		sjfsd();
	}
	if(strcmp(config_plani,"sjfcd")==0){
		printf("SJFCD\n");
		sjfcd();
	}
	else{
		printf("ERROR\n");
	}
	//while(1);
	printf("adios mundo cruel \n");
}

void generate_console(){
	consola();
}



