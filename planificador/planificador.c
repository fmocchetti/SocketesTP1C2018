#include "socket.h"
#include "protocolo.h"
#include "algoritmos.h"
#include <pthread.h>

void generate_poll();
void generate_planning();




#define IDENTIDAD "planificador"

int main (int argc, char *argv[])
{
	//inicializo los semaforos a utilizar
	sem_init(&new_process, 0, 0);
	sem_init(&mutex_listos, 0, 1);
	//creo los threads a utilizar
	pthread_t thread_poll;
	pthread_t thread_planificador;
	//Creo las listas a utilizar
	listos = list_create();
	bloqueados = list_create();
	ejecucion = list_create();
	terminados = list_create();

	//Configuro el log a utilizar
		configure_logger();

	pthread_create(&thread_poll, NULL, (void*) generate_poll, NULL);
	pthread_detach(thread_poll);
	pthread_create(&thread_planificador, NULL, (void*) generate_planning, NULL);
	pthread_detach(thread_planificador);



	printf("%s\n",IDENTIDAD);// borrar
    getchar();
	//Creo el server
	//create_server(32, 3 * 60 * 1000);
	return 0;
}

void generate_poll(){
	printf("Entre al thread del poll \n");
	create_server(32, 3 * 60 * 1000);
}

void generate_planning(){
	printf("Entre al thread de planificacion \n");
	fifo();
	//while(1);
	printf("holaaaaaa \n");
}

